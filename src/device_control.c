/*
 The MIT License (MIT)

Copyright (c) 2015 Milan Marić

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include "tdp_api.h"
#include "tables.h"
#include "remote.h"
#include "config_parser.h"
#include "device_control.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


static pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t patCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t patMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t pmtCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t pmtMutex = PTHREAD_MUTEX_INITIALIZER;
static uint8_t parsedTag = 0;

PatTable* patTable;
PmtTable** pmtTable;

int32_t indicator = 0;

int32_t tunerStatusCallback(t_LockStatus status)
{
    if (status == STATUS_LOCKED)
    {
        pthread_mutex_lock(&statusMutex);
        pthread_cond_signal(&statusCondition);
        pthread_mutex_unlock(&statusMutex);
        printf("\n%s -----TUNER LOCKED-----\n", __FUNCTION__);
    }
    else
    {
        printf("\n%s -----TUNER NOT LOCKED-----\n", __FUNCTION__);
    }
    return NO_ERROR;
}

int32_t pat_Demux_Section_Filter_Callback(uint8_t *buffer)
{

    printf("%s running\n", __FUNCTION__);
    parsePatTable(buffer, patTable);
    printf("%s patTable parsed\n",__FUNCTION__);
    if (patTable->patHeader->table_id == 0x00)
    {
        pthread_mutex_lock(&patMutex);
        pthread_cond_signal(&patCondition);
        pthread_mutex_unlock(&patMutex);
    }
    return NO_ERROR;
}

int32_t pmt_Demux_Section_Filter_Callback(uint8_t *buffer)
{
    parsePmt(buffer, pmtTable[indicator]);
    if (pmtTable[indicator]->pmtHeader->table_id == 0x02)
    {
        pthread_mutex_lock(&pmtMutex);
        pthread_cond_signal(&pmtCondition);
        pthread_mutex_unlock(&pmtMutex);
    }
    return NO_ERROR;
}

int32_t initPmtParsing(DeviceHandle* handle, uint16_t pid)
{
    static struct timespec lockStatusWaitTime;
    static struct timeval now;
    printf("%s: started\n", __FUNCTION__);
    if (pmtTable[indicator] != NULL)
    {
        if (pmtTable[indicator]->pmtHeader != NULL)
            free(pmtTable[indicator]->pmtHeader);
        free(pmtTable);
    }
    pmtTable[indicator] = (PmtTable*) malloc(sizeof (PmtTable));
    pmtTable[indicator]->pmtHeader = (PmtHeader*) malloc(sizeof (PmtHeader));

    printf("PID: %d\n", patTable->patServiceInfoArray[1].pid);

    gettimeofday(&now, NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec + 10;

    if (Demux_Set_Filter(handle->playerHandle, pid, 0x02, &(handle->filterHandle)))
    {
        printf("\n%s:ERROR Set filter failure!\n", __FUNCTION__);
        return ERROR;
    }

    if (Demux_Register_Section_Filter_Callback(pmt_Demux_Section_Filter_Callback))
    {
        printf("\n%s:ERROR Register Section filter failure!\n", __FUNCTION__);
        Demux_Free_Filter(handle->playerHandle, handle->filterHandle);
        return ERROR;
    }

    pthread_mutex_lock(&pmtMutex);
    if (ETIMEDOUT == pthread_cond_timedwait(&pmtCondition, &pmtMutex, &lockStatusWaitTime))
    {
        printf("\n%s:ERROR Lock timeout exceeded!\n", __FUNCTION__);
        Demux_Free_Filter(handle->playerHandle, handle->filterHandle);
        return ERROR;
    }
    pthread_mutex_unlock(&pmtMutex);
    dumpPmtTable(pmtTable[indicator]);
    Demux_Unregister_Section_Filter_Callback(pmt_Demux_Section_Filter_Callback);
    Demux_Free_Filter(handle->playerHandle, handle->sourceHandle);
    printf("%s ended", __FUNCTION__);
    return NO_ERROR;
}

int32_t initPatParsing(DeviceHandle *handle)
{

    static struct timespec lockStatusWaitTime;
    static struct timeval now;
    printf("%s: started\n", __FUNCTION__);
    //free memory if patTable is allocated
    if (patTable != NULL)
    {
        if (patTable->patHeader != NULL)
            free(patTable->patHeader);
        free(patTable);
    }
    // allocate memory for PAT
    patTable = (PatTable*) malloc(sizeof (PatTable));
    patTable->patHeader = (PatHeader*) malloc(sizeof (PatHeader));
    printf("%s: PAT allocated\n", __FUNCTION__);
    // set Demux filter for pat table
    // PAT pid=0x00,table_id=0
    if (Demux_Set_Filter(handle->playerHandle, 0x00, 0, &(handle->filterHandle)) == ERROR)
    {
        printf("%s: Demux_Set_Filter failed\n", __FUNCTION__);
        return ERROR;
    }

    printf("%s: Demux_Set_Filter\n", __FUNCTION__);
    gettimeofday(&now, NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec + 10;
    if (Demux_Register_Section_Filter_Callback(pat_Demux_Section_Filter_Callback))
    {
        Demux_Free_Filter(handle->playerHandle, handle->filterHandle);
        return ERROR;
    }
    printf("%s: registered callback\n", __FUNCTION__);
    pthread_mutex_lock(&patMutex);
    //timed waiting for while patTable is parsing
    if (ETIMEDOUT == pthread_cond_timedwait(&patCondition, &patMutex, &lockStatusWaitTime))
    {
        printf("\n%s:ERROR Lock timeout exceeded!\n", __FUNCTION__);
        Demux_Free_Filter(handle->playerHandle, handle->filterHandle);
        return ERROR;
    }
    pthread_mutex_unlock(&patMutex);
    printf("%s: pat parsed\n", __FUNCTION__);
    Demux_Unregister_Section_Filter_Callback(pat_Demux_Section_Filter_Callback);
    printf("%s: unregistered callback\n", __FUNCTION__);
    Demux_Free_Filter(handle->playerHandle, handle->sourceHandle);
    printf("%s: Demux_Free_Filter\n", __FUNCTION__);
    dumpPatTable(patTable);
    return NO_ERROR;
}

int deviceInit(config_parameters *parms, DeviceHandle *handle)
{
    static struct timespec lockStatusWaitTime;
    static struct timeval now;
    int i;
    printf("%s: started\n", __FUNCTION__);
    /*Initialize tuner device*/
    if (Tuner_Init())
    {
        printf("\n%s : ERROR Tuner_Init() fail\n", __FUNCTION__);
        return -1;
    }

    printf("%s: after Tuner_Init\n", __FUNCTION__);
    /* Register tuner status callback */
    if (Tuner_Register_Status_Callback(tunerStatusCallback))
    {
        printf("\n%s : ERROR Tuner_Register_Status_Callback() fail\n", __FUNCTION__);
    }
    printf("%s: After Tuner_Register_Status_Callback(tunerStatusCallback)\n", __FUNCTION__);
    /*Lock to frequency*/
    if (!Tuner_Lock_To_Frequency(parms->frequency, parms->bandwidth, parms->module))
    {
        printf("\n%s: INFO Tuner_Lock_To_Frequency(): %d Hz - success!\n", __FUNCTION__, parms->frequency);
    }
    else
    {
        printf("\n%s: ERROR Tuner_Lock_To_Frequency(): %d Hz - fail!\n", __FUNCTION__, parms->frequency);
        Tuner_Deinit();
        return -1;
    }
    /* Wait for tuner to lock*/
    pthread_mutex_lock(&statusMutex);
    if (ETIMEDOUT == pthread_cond_timedwait(&statusCondition, &statusMutex, &lockStatusWaitTime))
    {
        printf("\n%s:ERROR Lock timeout exceeded!\n", __FUNCTION__);
        Tuner_Deinit();
        return -1;
    }
    pthread_mutex_unlock(&statusMutex);
    printf("%s: Tuner locked\n", __FUNCTION__);


    if (Player_Init(&(handle->playerHandle)))
    {
        Tuner_Deinit();
        return -1;
    }
    printf("%s: Player inited\n", __FUNCTION__);

    if (Player_Source_Open(handle->playerHandle, &(handle->sourceHandle)))
    {
        Player_Deinit(handle->playerHandle);
        Tuner_Deinit();
        return -1;
    }
    printf("%s: Player_Source_Open\n", __FUNCTION__);

    if (Player_Stream_Create(handle->playerHandle, handle->sourceHandle, parms->vPid, parms->vType, &handle->streamHandle))
    {
        printf("%s Player_Source_Open failed", __FUNCTION__);
        Player_Source_Close(handle->playerHandle, handle->sourceHandle);
        Player_Deinit(handle->playerHandle);
        Tuner_Deinit();
        return ERROR;
    }
    printf("%s: Player_Stream_Create\n", __FUNCTION__);
    if (initPatParsing(handle) == NO_ERROR)
    {
        return ERROR;
    }
    for (i = 0; i < patTable->serviceInfoCount; i++)
    {
        indicator = i;
        if (initPmtParsing(handle, patTable->patServiceInfoArray[i].pid) == NO_ERROR)
            return ERROR;
    }
    parsedTag = 1;
    return NO_ERROR;
}

void deviceDeInit(DeviceHandle *handle)
{
    int i = 0;
    parsedTag = 0;
    Player_Stream_Remove(handle->playerHandle, handle->sourceHandle, handle->streamHandle);
    Demux_Free_Filter(handle->playerHandle, handle->filterHandle);
    Player_Source_Close(handle->playerHandle, handle->sourceHandle);
    Player_Deinit(handle->playerHandle);
    Tuner_Deinit();
    if (pmtTable != NULL)
    {
        for (i = 0; i < patTable->serviceInfoCount; i++)
        {
            if (pmtTable[i]->pmtHeader != NULL)
                free(pmtTable[i]->pmtHeader);
            free(pmtTable[i]);
        }
        free(pmtTable);
    }
    if (patTable != NULL)
    {
        if (patTable->patHeader != NULL)
            free(patTable->patHeader);
        free(patTable);
    }
}

uint32_t remoteServiceCallback(uint16_t service_number)
{
    if (parsedTag == 0)
    {
        printf("%s:Pmt sections are not ready yet!!!", __FUNCTION__);
        return ERROR;
    }
    if (service_number > 0 && service_number < patTable->serviceInfoCount)
        dumpPmtTable(pmtTable[service_number]);
    return NO_ERROR;
}

uint32_t remoteVolumeCallback(uint16_t service)
{
    if (service == VOLUME_PLUS)
        printf("Volume plus");
    if (service == VOLUME_MINUS)
        printf("Volume minus");
}

uint8_t getParsedTag()
{
    return parsedTag;
}