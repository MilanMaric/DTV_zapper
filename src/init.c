#include "tdp_api.h"
#include "tables.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


//15
static pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t patCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t patMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t pmtCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t pmtMutex = PTHREAD_MUTEX_INITIALIZER;

static PatTable* patTable;
static PmtTable* pmtTable;

static struct timespec lockStatusWaitTime;
static struct timeval now;
//22

static uint32_t playerHandle;
static uint32_t filterHandle;
static uint32_t sourceHandle;
static uint32_t streamHandle;

int32_t tunerStatusCallback(t_LockStatus status)
{
    if(status == STATUS_LOCKED)
    {
        pthread_mutex_lock(&statusMutex);
        pthread_cond_signal(&statusCondition);
        pthread_mutex_unlock(&statusMutex);
        printf("\n%s -----TUNER LOCKED-----\n",__FUNCTION__);
    }
    else
    {
        printf("\n%s -----TUNER NOT LOCKED-----\n",__FUNCTION__);
    }
    return 0;
}


int32_t pat_Demux_Section_Filter_Callback(uint8_t *buffer){
  printf("%s running\n",__FUNCTION__);
  parsePatTable(buffer,patTable);
  if(patTable->patHeader->table_id==0x00){
      pthread_mutex_lock(&patMutex);
        pthread_cond_signal(&patCondition);
        pthread_mutex_unlock(&patMutex);
  }
  return 0;
}

int32_t pmt_Demux_Section_Filter_Callback(uint8_t *buffer){
  parsePmt(buffer,pmtTable);
  if(pmtTable->pmtHeader->table_id==0x02){
    pthread_mutex_lock(&pmtMutex);
        pthread_cond_signal(&pmtCondition);
        pthread_mutex_unlock(&pmtMutex);
  }
  return 0;
}

int32_t initPmtParsing(){
  printf("%s: started\n",__FUNCTION__);
  pmtTable=(PmtTable*) malloc(sizeof(PmtTable));
    pmtTable->pmtHeader=(PmtHeader*) malloc(sizeof(PmtHeader));
   
    printf("PID: %d\n",patTable->patServiceInfoArray[1].pid);
    
    gettimeofday(&now,NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec+10;
    
    if(Demux_Set_Filter(playerHandle,patTable->patServiceInfoArray[1].pid,0x02,&filterHandle)){
       printf("\n%s:ERROR Set filter failure!\n",__FUNCTION__);
      Player_Source_Close(playerHandle, sourceHandle);
      Player_Deinit(playerHandle);
      Tuner_Deinit();
      return -1;
    }
    
 
    if(Demux_Register_Section_Filter_Callback(pmt_Demux_Section_Filter_Callback)){
      printf("\n%s:ERROR Register Section filter failure!\n",__FUNCTION__);
      Demux_Free_Filter(playerHandle,filterHandle);
      Player_Source_Close(playerHandle, sourceHandle);
      Player_Deinit(playerHandle);
      Tuner_Deinit();
      return -1;
    }
    
    pthread_mutex_lock(&pmtMutex);
     if(ETIMEDOUT == pthread_cond_timedwait(&pmtCondition, &pmtMutex, &lockStatusWaitTime))
    {
        printf("\n%s:ERROR Lock timeout exceeded!\n",__FUNCTION__);
        Demux_Free_Filter(playerHandle,filterHandle);
      Player_Source_Close(playerHandle, sourceHandle);
      Player_Deinit(playerHandle);
      Tuner_Deinit();
        return -1;
    }
    pthread_mutex_unlock(&pmtMutex);
    dumpPmtTable(pmtTable);
    Demux_Unregister_Section_Filter_Callback(pmt_Demux_Section_Filter_Callback);
    Demux_Free_Filter(playerHandle,sourceHandle);
    printf("%s ended",__FUNCTION__);
    return 0;
}

int32_t initPatParsing(){
  printf("%s: started\n",__FUNCTION__);
  patTable=(PatTable*) malloc(sizeof(patTable));
  patTable->patHeader=(PatHeader*) malloc(sizeof(PatHeader));
  printf("%s: PAT allocated\n",__FUNCTION__);
  if(Demux_Set_Filter(playerHandle,0x00,0,&filterHandle)){
      printf("%s: Demux_Set_Filter failed\n",__FUNCTION__);
      Player_Source_Close(playerHandle, sourceHandle);
      Player_Deinit(playerHandle);
      Tuner_Deinit();
      return -1;
    }
   printf("%s: Demux_Set_Filter\n",__FUNCTION__);
   gettimeofday(&now,NULL);
   lockStatusWaitTime.tv_sec = now.tv_sec+10;
    
    if(Demux_Register_Section_Filter_Callback(pat_Demux_Section_Filter_Callback)){
      Demux_Free_Filter(playerHandle,filterHandle);
      Player_Source_Close(playerHandle, sourceHandle);
      Player_Deinit(playerHandle);
      Tuner_Deinit();
      return -1;
    }
    printf("%s: registered callback\n",__FUNCTION__);
    
    pthread_mutex_lock(&patMutex);
    if(ETIMEDOUT == pthread_cond_timedwait(&patCondition, &patMutex, &lockStatusWaitTime))
    {
      printf("\n%s:ERROR Lock timeout exceeded!\n",__FUNCTION__);
      Demux_Free_Filter(playerHandle,filterHandle);
      Player_Source_Close(playerHandle, sourceHandle);
      Player_Deinit(playerHandle);
      Tuner_Deinit();
        return -1;
    }
    pthread_mutex_unlock(&patMutex);   
    printf("%s: pat parsed\n",__FUNCTION__);
    Demux_Unregister_Section_Filter_Callback(pat_Demux_Section_Filter_Callback);
    printf("%s: unregistered callback\n",__FUNCTION__);
    Demux_Free_Filter(playerHandle,sourceHandle);
    printf("%s: Demux_Free_Filter\n",__FUNCTION__);
   
    dumpPatTable(patTable);
    return 0;
}

int deviceInit()
{    
    gettimeofday(&now,NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec+10;
    printf("%s: started\n",__FUNCTION__);
    /*Initialize tuner device*/
    if(Tuner_Init())
    {
        printf("\n%s : ERROR Tuner_Init() fail\n", __FUNCTION__);
        return -1;
    }
//45
printf("%s: after Tuner_Init\n",__FUNCTION__);
    /* Register tuner status callback */
    if(Tuner_Register_Status_Callback(tunerStatusCallback))
    {
		printf("\n%s : ERROR Tuner_Register_Status_Callback() fail\n", __FUNCTION__);
	}
    printf("%s: After Tuner_Register_Status_Callback(tunerStatusCallback)\n",__FUNCTION__);
    /*Lock to frequency*/
    if(!Tuner_Lock_To_Frequency(DESIRED_FREQUENCY, BANDWIDTH, DVB_T))
    {
        printf("\n%s: INFO Tuner_Lock_To_Frequency(): %d Hz - success!\n",__FUNCTION__,DESIRED_FREQUENCY);
    }
    else
    {
        printf("\n%s: ERROR Tuner_Lock_To_Frequency(): %d Hz - fail!\n",__FUNCTION__,DESIRED_FREQUENCY);
        Tuner_Deinit();
        return -1;
    }
//63  
    /* Wait for tuner to lock*/
    pthread_mutex_lock(&statusMutex);
    if(ETIMEDOUT == pthread_cond_timedwait(&statusCondition, &statusMutex, &lockStatusWaitTime))
    {
        printf("\n%s:ERROR Lock timeout exceeded!\n",__FUNCTION__);
        Tuner_Deinit();
        return -1;
    }
    pthread_mutex_unlock(&statusMutex);
    printf("%s: Tuner locked\n",__FUNCTION__);
    
    /**TO DO:**/
    /*Initialize player, set PAT pid to demultiplexer and register section filter callback*/
    
    if(Player_Init(&playerHandle)){
      Tuner_Deinit();
      return -1;
    }
    printf("%s: Player inited\n",__FUNCTION__);
//80
    if(Player_Source_Open(playerHandle,&sourceHandle)){
      Player_Deinit(playerHandle);
      Tuner_Deinit();
      return -1;
    }
    printf("%s: Player_Source_Open\n",__FUNCTION__);
    
    if(Player_Stream_Create(playerHandle,sourceHandle,VIDEO_PID,VIDEO_TYPE_MPEG2,&streamHandle)){
      printf("%s Player_Source_Open failed",__FUNCTION__);
      Player_Source_Close(playerHandle, sourceHandle);
      Player_Deinit(playerHandle);
      Tuner_Deinit();
      return -1;
    }
    printf("%s: Player_Stream_Create\n",__FUNCTION__);
    initPatParsing();
    initPmtParsing();
    return 0;
}

void deviceDeInit(){
  Player_Stream_Remove(playerHandle,sourceHandle,streamHandle);
    Demux_Free_Filter(playerHandle,filterHandle);
    Player_Source_Close(playerHandle, sourceHandle);
    Player_Deinit(playerHandle);
}


