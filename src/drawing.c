#include "drawing.h"
#include <stdint.h>
#include <directfb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<time.h>
#include <stdint.h>
#include <directfb.h>

static IDirectFBSurface *primary = NULL;
static IDirectFB *dfbInterface = NULL;
static int screenWidth = 0;
static int screenHeight = 0;
static DFBSurfaceDescription surfaceDesc;
static int initialized = 0;

void fillBlack()
{
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

void initDirectFB()
{
    /* fetch the DirectFB interface */
    DFBCHECK(DirectFBCreate(&dfbInterface));
    /* tell the DirectFB to take the full screen for this application */
    DFBCHECK(dfbInterface->SetCooperativeLevel(dfbInterface, DFSCL_FULLSCREEN));


    /* create primary surface with double buffering enabled */

    surfaceDesc.flags = DSDESC_CAPS;
    surfaceDesc.caps = DSCAPS_PRIMARY | DSCAPS_FLIPPING;
    DFBCHECK(dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &primary));


    /* fetch the screen size */
    DFBCHECK(primary->GetSize(primary, &screenWidth, &screenHeight));

    fillBlack();
}

void deinitDirectFB()
{
    primary->Release(primary);
    dfbInterface->Release(dfbInterface);
}


void timerFunction()
{
    fillBlack();
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
    primary->Flip(primary,
                  /*region to be updated, NULL for the whole surface*/NULL,
                  /*flip flags*/0);
}

void setTimer(int32_t interval)
{
    struct sigevent signalEvent;
    //reći OS-u da notifikaciju šalje prozivanjem specificirane funkcije iz posebne niti
    signalEvent.sigev_notify = SIGEV_THREAD;
    //funkcija koju će OS prozvati kada interval istekne
    signalEvent.sigev_notify_function = timerFunction;
    //argumenti funkcije
    signalEvent.sigev_value.sival_ptr = NULL;
    //atributi niti - if NULL default attributes are applied
    signalEvent.sigev_notify_attributes = NULL;
    timer_create(/*sistemski sat za merenje vremena*/ CLOCK_REALTIME,
                 /*podešavanja timer-a*/ &signalEvent,
                 /*mesto gde će se smestiti ID novog timer-a*/ &timerId);

    struct itimerspec timerSpec;
    struct itimerspec timerSpecOld;
    int32_t timerFlags = 0;

    //brisanje strukture pre setovanja vrednosti
    memset(&timerSpec, 0, sizeof (timerSpec));

    //specificiranje vremenskih podešavanja timer-a
    timerSpec.it_value.tv_sec = interval; //3 seconds timeout
    timerSpec.it_value.tv_nsec = 0;
}

void drawTextInfo(int32_t service_number)
{
    char buffer[5];
    sprintf(buffer, "%d", service_number);


    /* rectangle drawing */

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





