#include "tdp_api.h"
#include "tables.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define DESIRED_FREQUENCY 818000000	/* Tune frequency in Hz */
#define BANDWIDTH 8    				/* Bandwidth in Mhz */
#define VIDEO_PID 101				/* Channel video pid */
#define AUDIO_PID 103				/* Channel audio pid */
//15
static pthread_cond_t statusCondition = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t statusMutex = PTHREAD_MUTEX_INITIALIZER;

static int32_t tunerStatusCallback(t_LockStatus status);

PatTable* patTable;
PmtTable* pmtTable;
//22
int32_t my_Demux_Section_Filter_Callback(uint8_t *buffer){
  dumpTable(patTable);
  parseTable(buffer,patTable);
  return 0;
}

int32_t pmt_Table_Section_Filter_Callback(uint8_t *buffer){
  parsePmt(buffer,pmtTable);
  dumpPmtTable(pmtTable);
  return 0;
}

int main()
{    
	struct timespec lockStatusWaitTime;
	struct timeval now;
	
	uint32_t playerHandle;
        uint32_t filterHandle;
        uint32_t sourceHandle;
	uint32_t streamHandle;
	
	patTable=(PatTable*) malloc(sizeof(patTable));
	patTable->patHeader=(PatHeader*) malloc(sizeof(PatHeader));
    
    gettimeofday(&now,NULL);
    lockStatusWaitTime.tv_sec = now.tv_sec+10;
       
    /*Initialize tuner device*/
    if(Tuner_Init())
    {
        printf("\n%s : ERROR Tuner_Init() fail\n", __FUNCTION__);
        return -1;
    }
//45
    /* Register tuner status callback */
    if(Tuner_Register_Status_Callback(tunerStatusCallback))
    {
		printf("\n%s : ERROR Tuner_Register_Status_Callback() fail\n", __FUNCTION__);
	}
    
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
   
    /**TO DO:**/
    /*Initialize player, set PAT pid to demultiplexer and register section filter callback*/
    
    if(Player_Init(&playerHandle)){
      Tuner_Deinit();
      return -1;
    }
//80
    if(Player_Source_Open(playerHandle,&sourceHandle)){
      Player_Deinit(playerHandle);
      Tuner_Deinit();
      return -1;
    }
    if(Demux_Set_Filter(playerHandle,0x00,0,&filterHandle)){
      Player_Source_Close(playerHandle, sourceHandle);
      Player_Deinit(playerHandle);
      Tuner_Deinit();
      return -1;
    }
    if(Demux_Register_Section_Filter_Callback(my_Demux_Section_Filter_Callback)){
      Demux_Free_Filter(playerHandle,filterHandle);
      Player_Source_Close(playerHandle, sourceHandle);
      Player_Deinit(playerHandle);
      Tuner_Deinit();
      return -1;
    }
    fflush(stdin);
    getchar();
    
    
    dumpTable(patTable);
    //Demux_Free_Filter(playerHandle,filterHandle);
    pmtTable=(PmtTable*) malloc(sizeof(PmtTable));
    pmtTable->pmtHeader=(PmtHeader*) malloc(sizeof(PmtHeader));
    printf("PMT allocated\n\n");
    Demux_Unregister_Section_Filter_Callback(my_Demux_Section_Filter_Callback);
    Demux_Free_Filter(playerHandle,sourceHandle);
    printf("PID: %d\n",patTable->patServiceInfoArray[1].pid);
    if(Demux_Set_Filter(playerHandle,patTable->patServiceInfoArray[1].pid,0x02,&filterHandle)){
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
    fflush(stdin);
    getchar();
    printf("\n Table ID: %d\n",pmtTable->pmtHeader->table_id);
    
    Player_Stream_Create(playerHandle,sourceHandle,VIDEO_PID,VIDEO_TYPE_MPEG2,&streamHandle);
	/* Wait for a while */
	
    fflush(stdin);
    getchar();
    
 
    
    /**TO DO:**/
    /*Deinitialization*/
    Player_Stream_Remove(playerHandle,sourceHandle,streamHandle);
    Demux_Free_Filter(playerHandle,filterHandle);
    Player_Source_Close(playerHandle, sourceHandle);
    Player_Deinit(playerHandle);
//117    
    /*Deinitialize tuner device*/
    Tuner_Deinit();
  
    return 0;
}

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
//139
/**TO DO:**/
/*Parse PAT Table*/
