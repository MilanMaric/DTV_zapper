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
#include "tables.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

static int32_t tunerStatusCallback(t_LockStatus status);

PatTable* patTable;
PmtTable* pmtTable[MAX_NUM_OF_PIDS];

int32_t pat_Table_Section_Filter_Callback(uint8_t *buffer)
{
    dumpTable(patTable);
    return 0;
}

int32_t pmt_Table_Section_Filter_Callback(uint8_t *buffer)
{
    parsePmt(buffer, pmtTable);
    dumpPmtTable(pmtTable);
    return 0;
}

struct timespec lockStatusWaitTime;
struct timeval now;

uint32_t playerHandle;
uint32_t filterHandle;
uint32_t sourceHandle;
uint32_t streamHandle;

int16_t initParsing()
{
    gettimeofday(&now, NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec + 10;
    //Tuner init
    if (Tuner_Init())
    {
        printf("\n%s : ERROR Tuner_Init() fail\n", __FUNCTION__);
        return INIT_ERROR;
    }
    //Tuner register callback
    if (Tuner_Register_Status_Callback(tunerStatusCallback))
    {
        printf("\n%s : ERROR Tuner_Register_Status_Callback() fail\n", __FUNCTION__);
        return INIT_ERROR;
    }
    //Tuner lock
    if (!Tuner_Lock_To_Frequency(DESIRED_FREQUENCY, BANDWIDTH, DVB_T))
    {
        printf("\n%s: INFO Tuner_Lock_To_Frequency(): %d Hz - success!\n", __FUNCTION__, DESIRED_FREQUENCY);

    }
    else
    {
        printf("\n%s: ERROR Tuner_Lock_To_Frequency(): %d Hz - fail!\n", __FUNCTION__, DESIRED_FREQUENCY);
        Tuner_Deinit();
        return INIT_ERROR;
    }
    //wait until tuner is locked
    pthread_mutex_lock(&statusMutex);
    if (ETIMEDOUT == pthread_cond_timedwait(&statusCondition, &statusMutex, &lockStatusWaitTime))
    {
        printf("\n%s:ERROR Lock timeout exceeded!\n", __FUNCTION__);
        Tuner_Deinit();
        return INIT_ERROR;
    }
    pthread_mutex_unlock(&statusMutex);
    //Player init
    if (Player_Init(&playerHandle))
    {
        Tuner_Deinit();
        return INIT_ERROR;
    }
    //Player open source
    if (Player_Source_Open(playerHandle, &sourceHandle))
    {
        Player_Deinit(playerHandle);
        Tuner_Deinit();
        return INIT_ERROR;
    }
}

int32_t init_Parse_Pat()
{
    patTable = (PatTable*) malloc(sizeof (patTable));
    patTable->patHeader = (PatHeader*) malloc(sizeof (PatHeader));
    if (Demux_Set_Filter(playerHandle, 0x00, 0, &filterHandle))
    {
        Player_Source_Close(playerHandle, sourceHandle);
        Player_Deinit(playerHandle);
        Tuner_Deinit();
        return INIT_ERROR;
    }
    if (Demux_Register_Section_Filter_Callback(pat_Table_Section_Filter_Callback))
    {
        Demux_Free_Filter(playerHandle, filterHandle);
        Player_Source_Close(playerHandle, sourceHandle);
        Player_Deinit(playerHandle);
        Tuner_Deinit();
        return INIT_ERROR;
    }
    
}

int32_t init_Parse_Pmt(int32_t service_number)
{
    pmtTable[service_number] = (PmtTable**) malloc(sizeof (PmtTable*)*patTable->serviceInfoCount);
    pmtTable[service_number].pmtHeader = (PmtHeader*) malloc(sizeof (PmtHeader));
    printf("PMT allocated\n\n");
    Demux_Unregister_Section_Filter_Callback(pat_Table_Section_Filter_Callback);
    Demux_Free_Filter(playerHandle, sourceHandle);
    printf("PID: %d\n", patTable->patServiceInfoArray[service_number].pid);
    if (Demux_Set_Filter(playerHandle, patTable->patServiceInfoArray[service_number].pid, 0x02, &filterHandle))
    {
        Player_Source_Close(playerHandle, sourceHandle);
        Player_Deinit(playerHandle);
        Tuner_Deinit();
        return -1;
    }
    if(Demux_Register_Section_Filter_Callback(pmt_Table_Section_Filter_Callback)){
      Demux_Free_Filter(playerHandle,filterHandle);
      Player_Source_Close(playerHandle, sourceHandle);
      Player_Deinit(playerHandle);
      Tuner_Deinit();
      return -1;
    }
}

void parsePatServiceInfoArray(uint8_t *buffer, PatServiceInfo patServiceInfoArray[], uint16_t section_length)
{
    int brojPidova = (section_length - 10) / 4;
    int kraj;
    int i = 0;
    if (brojPidova < MAX_NUM_OF_PIDS)
    {
        kraj = brojPidova;
    }
    else
    {
        kraj = MAX_NUM_OF_PIDS;
    }

    for (i = 0; i < kraj; i++)
    {
        patServiceInfoArray[i].program_number = (uint16_t) ((*(buffer + i * 4 + 8) << 8) + *(buffer + i * 4 + 9));
        patServiceInfoArray[i].pid = (uint16_t) (((*(buffer + i * 4 + 10) << 8) + *(buffer + i * 4 + 11)) & 0x1FFF);
    }
}


void parsePatHeader(uint8_t *buffer, PatHeader* patHeader)
{
    (*patHeader).table_id = (uint8_t) (*(buffer + 0));
    (*patHeader).section_syntax_indicator = (uint8_t) (*(buffer + 1) << 7);
    (*patHeader).section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);
    (*patHeader).transport_stream_id = (uint16_t) ((*(buffer + 3) << 8) + *(buffer + 4));
    (*patHeader).version_number = (uint8_t) ((*(buffer + 5) >> 1) & 0x1F);
    (*patHeader).current_next_indicator = (uint8_t) (*(buffer + 5) & 0x01);
    (*patHeader).section_number = (uint8_t) (*(buffer + 6));
    (*patHeader).last_section_number = (uint8_t) (*(buffer + 6));
}


void dumpPatHeader(PatHeader* patHeader)
{
    printf("\n<<<<<<<<<<<<<<<< Pat header >>>>>>>>>>>>>\n");
    printf("Table id:%d\n", (*patHeader).table_id);
    printf("Section syntax: %d\n", (*patHeader).section_syntax_indicator);
    printf("Section length: %d\n", (*patHeader).section_length);
    printf("Transport stream ID: %d\n", (*patHeader).transport_stream_id);
    printf("Version number: %d\n", (*patHeader).version_number);
    printf("Current next indicator: %d\n", (*patHeader).current_next_indicator);
    printf("Section number: %d\n", (*patHeader).section_number);
    printf("Last section number: %d", (*patHeader).last_section_number);
}

void parsePatTable(uint8_t *buffer, PatTable* table)
{
    parseHeader(buffer, table->patHeader);
    dumpHeader(table->patHeader);
    parseServiceInfoArray(buffer, table->patServiceInfoArray, table->patHeader->section_length);
    dumpHeader(table->patHeader);
    (*table).serviceInfoCount = (uint8_t) ((*(*table).patHeader).section_length - 10) / 4;
}

void dumpServiceInfo(PatServiceInfo* patServiceInfo)
{
    printf("Program number: %d,pid: %d\n", patServiceInfo->program_number, patServiceInfo->pid);
}

void dumpPatTable(PatTable* table)
{
    int i = 0;
    dumpHeader((table->patHeader));
    printf("\n<<<<<<<<<<<<<<<< Pat service info >>>>>>>>>>>>>\n");
    for (i = 0; i < table->serviceInfoCount; i++)
        dumpServiceInfo(&(table->patServiceInfoArray[i]));
}

void parsePmt(uint8_t *buffer, PmtTable* table)
{
    parsePmtHeader(buffer, table->pmtHeader);
    parsePmtServiceInfoArray(buffer, table->pmtServiceInfoArray);
}

void parsePmtHeader(uint8_t *buffer, PmtHeader* pmtHeader)
{
    (*pmtHeader).table_id = (uint8_t) (*(buffer + 0));
    (*pmtHeader).section_syntax_indicator = (uint8_t) (*(buffer + 1) << 7);
    (*pmtHeader).section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);
    (*pmtHeader).program_number = (uint16_t) ((*(buffer + 3) << 8) + *(buffer + 4));
    (*pmtHeader).version_number = (uint8_t) ((*(buffer + 5) >> 1) & 0x1F);
    (*pmtHeader).current_next_indicator = (uint8_t) (*(buffer + 5) & 0x01);
    (*pmtHeader).section_number = (uint8_t) (*(buffer + 6));
    (*pmtHeader).last_section_number = (uint8_t) (*(buffer + 6));
    (*pmtHeader).pcr_pid = (uint16_t) (((*(buffer + 7) << 8) + *(buffer + 8)) & 0x1FFF);
    (*pmtHeader).program_info_length = (uint16_t) (((*(buffer + 9) << 8) + *(buffer + 10)) & 0x0FFF);
}

void parsePmtServiceInfoArray(uint8_t *buffer, PmtServiceInfo pmtServiceInfoArray[MAX_NUM_OF_PIDS])
{
    uint8_t section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);
    uint16_t program_info_length = (uint16_t) (((*(buffer + 9) << 8) + *(buffer + 10)) & 0x0FFF);
    int kraj = section_length - 1;
    int poc = program_info_length + 3 + 9;
    int i = 0;
    for (i = 0; poc <= kraj; i++)
    {
        pmtServiceInfoArray[i].stream_type = (uint8_t) (*(buffer + poc));
        poc++;
        pmtServiceInfoArray[i].el_pid = (uint16_t) (((*(buffer + poc) << 8) + *(buffer + poc + 1)) & 0x1FFF);
        poc += 2;
        pmtServiceInfoArray[i].es_info_length = (uint16_t) (((*(buffer + poc) << 8) + *(buffer + poc + 1)) & 0x0FFF);
        poc += 2;
        poc += pmtServiceInfoArray[i].es_info_length;
    }
}

void dumpPmtTable(PmtTable* pmtTable)
{
    printf("Table id: %d\n", pmtTable->pmtHeader->table_id);
    printf("Section syntax indicator id: %d\n", pmtTable->pmtHeader->section_syntax_indicator);
    printf("section_length: %d\n", pmtTable->pmtHeader->section_length);
    printf("program_number: %d\n", pmtTable->pmtHeader->program_number);
    printf("version_number: %d\n", pmtTable->pmtHeader->version_number);
    printf("current_next_indicator: %d\n", pmtTable->pmtHeader->current_next_indicator);
    printf("section_number: %d\n", pmtTable->pmtHeader->section_number);
    printf("last_section_number: %d\n", pmtTable->pmtHeader->last_section_number);
    printf("pcr_pid: %d\n", pmtTable->pmtHeader->pcr_pid);
    printf("program_info_length: %d\n", pmtTable->pmtHeader->program_info_length);
}

