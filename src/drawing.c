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
#include "drawing.h"
#include <stdint.h>
#include <directfb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>
#include <directfb.h>

static IDirectFBSurface *primary = NULL;
static IDirectFB *dfbInterface = NULL;
static int screenWidth = 0;
static int screenHeight = 0;
static DFBSurfaceDescription surfaceDesc;
static int initialized = 0;
timer_t timerId;
struct itimerspec timerSpec;
struct itimerspec timerSpecOld;
int32_t timerFlags = 0;

IDirectFBFont *fontInterface48 = NULL;
DFBFontDescription fontDesc48;
IDirectFBFont *fontInterface20 = NULL;
DFBFontDescription fontDesc20;

int16_t settedTimer=0;

void fillBlack()
{
    printf("fill black\n");
    DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
                               /*red*/ 0x00,
                               /*green*/ 0x00,
                               /*blue*/ 0x00,
                               /*alpha*/ 0x00));
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

    //  fillBlack();
}

void deinitDirectFB()
{
    primary->Release(primary);
    dfbInterface->Release(dfbInterface);
    timer_delete(timerId);
}

void timerFunction()
{
    printf("%s started\n", __FUNCTION__);
    fillBlack();
    primary->Flip(primary,NULL,0);
    fillBlack();
    memset(&timerSpec, 0, sizeof (timerSpec));
    timer_settime(timerId, 0, &timerSpec, &timerSpecOld);
    settedTimer=0;
    printf("%s ended\n", __FUNCTION__);

}

void setTimer(int32_t interval)
{
    struct sigevent signalEvent;

    printf("%s started\n", __FUNCTION__);
    //brisanje strukture pre setovanja vrednosti
    memset(&timerSpec, 0, sizeof (timerSpec));

    //reći OS-u da notifikaciju šalje prozivanjem specificirane funkcije iz posebne niti
    signalEvent.sigev_notify = SIGEV_THREAD;
    //funkcija koju će OS prozvati kada interval istekne
    signalEvent.sigev_notify_function = &timerFunction;
    //argumenti funkcije
    signalEvent.sigev_value.sival_ptr = NULL;
    //atributi niti - if NULL default attributes are applied
    signalEvent.sigev_notify_attributes = NULL;
    //specificiranje vremenskih podešavanja timer-a
    timerSpec.it_value.tv_sec = interval; //3 seconds timeout
    timerSpec.it_value.tv_nsec = 0;
    if(settedTimer)
    timer_delete(timerId);
    settedTimer=1;
    timer_create(/*sistemski sat za merenje vremena*/ CLOCK_REALTIME,
                 /*podešavanja timer-a*/ &signalEvent,
                 /*mesto gde će se smestiti ID novocreatefontg timer-a*/ &timerId);
    timer_settime(timerId, timerFlags, &timerSpec, &timerSpecOld);
    printf("%s ended\n", __FUNCTION__);

}

void drawTextInfo(int32_t service_number, uint16_t vpid, uint16_t apid)
{
    char buffer[50];
    int x;
    int y;



    /* rectangle drawing */

    fillBlack();
    x = 1 * screenWidth / 4;
    y = 5 * screenHeight / 8;

    DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
                               /*red*/ 0x00,
                               /*green*/ 0xFF,
                               /*blue*/ 0x00,
                               /*alpha*/ 0x88));
    primary->FillRectangle(/*surface to draw on*/ primary,
                           /*upper left x coordinate*/ x,
                           /*upper left y coordinate*/ y,
                           /*rectangle width*/ 2 * screenWidth / 4,
                           /*rectangle height*/ 2 * screenHeight / 8);
    /* create the font and set the created font for primary surface text drawing */




    DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc48, &fontInterface48));
    DFBCHECK(primary->SetFont(primary, fontInterface48));

    sprintf(buffer, "Channel %d\0", service_number);
    fontDesc48.flags = DFDESC_HEIGHT;
    fontDesc48.height = 48;
    /* draw the text */
    DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
                               /*red*/ 0xFF,
                               /*green*/ 0xFF,
                               /*blue*/ 0xFF,
                               /*alpha*/ 0x00));
    x = x + 58;
    y = y + 58;
    DFBCHECK(primary->DrawString(primary,
                                 /*text to be drawn*/ buffer,
                                 /*number of bytes in the string, -1 for NULL terminated strings*/ -1,
                                 /*x coordinate of the lower left corner of the resulting text*/ x,
                                 /*y coordinate of the lower left corner of the resulting text*/ y,
                                 /*in case of multiple lines, allign text to left*/ DSTF_LEFT));

    fontInterface48->Release(fontInterface48);


    DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc20, &fontInterface20));
    DFBCHECK(primary->SetFont(primary, fontInterface20));

    fontDesc20.flags = DFDESC_HEIGHT;
    fontDesc20.height = 20;
    /* draw the text */
    sprintf(buffer, "Video PID %d\0", vpid);
    DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
                               /*red*/ 0xFF,
                               /*green*/ 0xFF,
                               /*blue*/ 0xFF,
                               /*alpha*/ 0x00));
    y = y + 58;
    DFBCHECK(primary->DrawString(primary,
                                 /*text to be drawn*/ buffer,
                                 /*number of bytes in the string, -1 for NULL terminated strings*/ -1,
                                 /*x coordinate of the lower left corner of the resulting text*/ x,
                                 /*y coordinate of the lower left corner of the resulting text*/ y,
                                 /*in case of multiple lines, allign text to left*/ DSTF_LEFT));
    sprintf(buffer, "Audio PID %d\0", apid);
    DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
                               /*red*/ 0xFF,
                               /*green*/ 0xFF,
                               /*blue*/ 0xFF,
                               /*alpha*/ 0x00));
    y = y + 20;
    DFBCHECK(primary->DrawString(primary,
                                 /*text to be drawn*/ buffer,
                                 /*number of bytes in the string, -1 for NULL terminated strings*/ -1,
                                 /*x coordinate of the lower left corner of the resulting text*/ x,
                                 /*y coordinate of the lower left corner of the resulting text*/ y,
                                 /*in case of multiple lines, allign text to left*/ DSTF_LEFT));
    fontInterface20->Release(fontInterface20);

    primary->Flip(primary,
                  /*region to be updated, NULL for the whole surface*/NULL,
                  /*flip flags*/0);
    printf("createfont\n");

    setTimer(3);
}


void drawVolume(int32_t volume)
{
    IDirectFBImageProvider *provider;
    IDirectFBSurface *surface = NULL;
    int32_t surfaceHeight, surfaceWidth;
    char buffer[50];
    sprintf(buffer, "volume_%d.png\0", volume);
    fillBlack();
    printf("%s : buffer %s\n",__FUNCTION__,buffer);
    /* create the image provider for the specified file */
    DFBCHECK(dfbInterface->CreateImageProvider(dfbInterface, buffer, &provider));
    /* get surface descriptor for the surface where the image will be rendered */
    DFBCHECK(provider->GetSurfaceDescription(provider, &surfaceDesc));
    /* create the surface for the image */
    DFBCHECK(dfbInterface->CreateSurface(dfbInterface, &surfaceDesc, &surface));
    /* render the image to the surface */
    DFBCHECK(provider->RenderTo(provider, surface, NULL));

    /* cleanup the provider after rendering the image to the surface */
    provider->Release(provider);

    /* fetch the logo size and add (blit) it to the screen */
    DFBCHECK(surface->GetSize(surface, &surfaceWidth, &surfaceHeight));
    DFBCHECK(primary->Blit(primary,
                           /*source surface*/ surface,
                           /*source region, NULL to blit the whole surface*/ NULL,
                           /*destination x coordinate of the upper left corner of the image*/50,
                           /*destination y coordinate of the upper left corner of the image*/screenHeight - surfaceHeight - 50));
    primary->Flip(primary,NULL,0);
    setTimer(3);
}







