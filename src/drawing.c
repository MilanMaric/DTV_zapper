#include "drawing.h"
#include <stdint.h>
#include <directfb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<time.h>
int32_t *argcg;
char*** argvg;

void setProgramParams(int32_t* argci, char*** argvi){
argcg=argci;
argvg=argvi;
}

 IDirectFBSurface *primary = NULL;
    IDirectFB *dfbInterface = NULL;
    int screenWidth = 0;
     int screenHeight = 0;
	DFBSurfaceDescription surfaceDesc;
	 int initialized=0;
	

void initDirectFB(){
    printf("%d %s\n",*argcg,**(argvg));
    /* initialize DirectFB */
    
	DFBCHECK(DirectFBInit(argcg, argvg));
    /* fetch the DirectFB interface */
	DFBCHECK(DirectFBCreate(&dfbInterface));
    /* tell the DirectFB to take the full screen for this application */
	DFBCHECK(dfbInterface->SetCooperativeLevel(dfbInterface, DFSCL_FULLSCREEN));
	
    
    /* create primary surface with double buffering enabled */
    
	surfaceDesc.flags = DSDESC_CAPS;
	surfaceDesc.caps = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
	DFBCHECK (dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &primary));
    
    
    /* fetch the screen size */
    DFBCHECK (primary->GetSize(primary, &screenWidth, &screenHeight));
    
    
    /* clear the screen before drawing anything (draw black full screen rectangle)*/
    
    DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
                               /*red*/ 0x00,
                               /*green*/ 0x00,
                               /*blue*/ 0x00,
                               /*alpha*/ 0xff));
	DFBCHECK(primary->FillRectangle(/*surface to draw on*/ primary,
                                    /*upper left x coordinate*/ 0,
                                    /*upper left y coordinate*/ 0,
                                    /*rectangle width*/ screenWidth,
                                    /*rectangle height*/ screenHeight));
}

void deinitDirectFB(){
  primary->Release(primary);
	dfbInterface->Release(dfbInterface);
}
timer_t timerId;

void timerFunction(){
  fillBlack();
  primary->Flip(primary,
                           /*region to be updated, NULL for the whole surface*/NULL,
                           /*flip flags*/0);
}

void postaviTajmer(){
  struct sigevent signalEvent;
signalEvent.sigev_notify=SIGEV_THREAD;
signalEvent.sigev_notify_function=timerFunction;
signalEvent.sigev_value.sival_ptr=NULL;
signalEvent.sigev_notify_attributes=NULL;
timer_create(CLOCK_REALTIME,&signalEvent,&timerId);
struct itimespec timerSpec;
struct itemrspec timerSpecOld;
int32_t timerFlags=0;
memset(&timerSpec,0,sizeof(struct itimespec));
timerSpec.it_value.tv_sec=3;
timerSpec.it_value.tv_nsec=0;
timer_settime(timerId,timerFlags,&timerSpec,&timerSpecOld);
}

void drawTextInfo(int32_t service_number){
  char buffer[5];
  //itoa (service_number,buffer,10);
  
      
    /* rectangle drawing */
  sprintf(buffer,"%d",service_number);
    fillBlack();
    
    /* draw the text */
     
    DFBCHECK(primary->SetColor(primary, 0xff, 0x80, 0x80, 0xff));
	DFBCHECK(primary->DrawString(primary,
                                 /*text to be drawn*/ buffer,
                                 /*number of bytes in the string, -1 for NULL terminated strings*/ -1,
                                 /*x coordinate of the lower left corner of the resulting text*/ 500,
                                 /*y coordinate of the lower left corner of the resulting text*/ 500,
                                 /*in case of multiple lines, allign text to left*/ DSTF_LEFT));
	primary->Flip(primary,
                           /*region to be updated, NULL for the whole surface*/NULL,
                           /*flip flags*/0);
	
	

	
}


void fillBlack(){
  DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
                               /*red*/ 0x00,
                               /*green*/ 0x00,
                               /*blue*/ 0x00,
                               /*alpha*/ 0xff));
    primary->FillRectangle(/*surface to draw on*/ primary,
                                    /*upper left x coordinate*/ 0,
                                    /*upper left y coordinate*/ 0,
                                    /*rectangle width*/ screenWidth,
                                    /*rectangle height*/ screenHeight);
}



