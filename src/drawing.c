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
uint8_t black = 0;

int16_t settedTimer = 0;

void fillTransparent()
{
    // printf("fill black\n");
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

void fillBlack()
{
    DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
                               /*red*/ 0x00,
                               /*green*/ 0x00,
                               /*blue*/ 0xff,
                               /*alpha*/ 0xff));
    primary->FillRectangle(/*surface to draw on*/ primary,
                           /*upper left x coordinate*/ 0,
                           /*upper left y coordinate*/ 0,
                           /*rectangle width*/ screenWidth,
                           /*rectangle height*/ screenHeight);
    primary->Flip(primary,
                  /*region to be updated, NULL for the whole surface*/NULL,
                  /*flip flags*/0);
    DFBCHECK(primary->SetColor(/*surface to draw on*/ primary,
                               /*red*/ 0x00,
                               /*green*/ 0x00,
                               /*blue*/ 0xff,
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
    /* DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc20, &fontInterface20));
     DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc48, &fontInterface48));
     fontInterface20->Release(fontInterface20);
     fontInterface48->Release(fontInterface48);*/
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
    //   printf("%s started\n", __FUNCTION__);
    if (black == 0)
    {
        fillTransparent();
    }
    else
    {
        fillBlack();
    }
    primary->Flip(primary, NULL, 0);
    fillTransparent();
    memset(&timerSpec, 0, sizeof (timerSpec));
    timer_settime(timerId, 0, &timerSpec, &timerSpecOld);
    settedTimer = 0;
    //   printf("%s ended\n", __FUNCTION__);

}

void setTimer(int32_t interval)
{
    struct sigevent signalEvent;
    memset(&timerSpec, 0, sizeof (timerSpec));
    signalEvent.sigev_notify = SIGEV_THREAD;
    signalEvent.sigev_notify_function = &timerFunction;
    signalEvent.sigev_value.sival_ptr = NULL;
    signalEvent.sigev_notify_attributes = NULL;
    timerSpec.it_value.tv_sec = interval; //3 seconds timeout
    timerSpec.it_value.tv_nsec = 0;
    if (settedTimer)
        timer_delete(timerId);
    settedTimer = 1;
    timer_create(CLOCK_REALTIME, &signalEvent, &timerId);
    timer_settime(timerId, timerFlags, &timerSpec, &timerSpecOld);
}

void drawTextInfo(int32_t service_number, uint16_t vpid, uint16_t apid, uint8_t tel)
{
    char buffer[50];
    int x;
    int y;
    char teletekst[] = "TXT";
    /* rectangle drawing */
    if (vpid)
    {
        fillTransparent();
        black = 0;
    }
    else
    {
        fillBlack();
        black = 1;
    }
    x = 1 * screenWidth / 4;
    y = 5 * screenHeight / 8;
    DFBCHECK(primary->SetColor(primary, 0x00, 0xFF, 0x00, 0xff));
    primary->FillRectangle(primary, x, y, 2 * screenWidth / 4, 2 * screenHeight / 8);
    /* create the font and set the created font for primary surface text drawing */
    DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc48, &fontInterface48));
    DFBCHECK(primary->SetFont(primary, fontInterface48));
    sprintf(buffer, "Channel %d", service_number);
    fontDesc48.flags = DFDESC_HEIGHT;
    fontDesc48.height = 48;
    /* draw the text */
    DFBCHECK(primary->SetColor(primary, 0xFF, 0xFF, 0xFF, 0x00));
    x = x + 58;
    y = y + 58;
    DFBCHECK(primary->DrawString(primary, buffer, -1, x, y, DSTF_LEFT));
    if (tel)
    {
        DFBCHECK(primary->DrawString(primary, teletekst, -1, 2 * screenWidth / 4 + x - 150, y, DSTF_LEFT));
        fontInterface48->Release(fontInterface48);
    }

    DFBCHECK(dfbInterface->CreateFont(dfbInterface, "/home/galois/fonts/DejaVuSans.ttf", &fontDesc20, &fontInterface20));
    DFBCHECK(primary->SetFont(primary, fontInterface20));

    fontDesc20.flags = DFDESC_HEIGHT;
    fontDesc20.height = 20;
    /* draw the text */
    sprintf(buffer, "Video PID %d", vpid);
    DFBCHECK(primary->SetColor(primary, 0xFF, 0xFF, 0xFF, 0x00));
    y = y + 58;
    DFBCHECK(primary->DrawString(primary, buffer, -1, x, y, DSTF_LEFT));
    sprintf(buffer, "Audio PID %d", apid);
    DFBCHECK(primary->SetColor(primary, 0xFF, 0xFF, 0xFF, 0x00));
    y = y + 20;
    DFBCHECK(primary->DrawString(primary, buffer, -1, x, y, DSTF_LEFT));
    fontInterface20->Release(fontInterface20);
    primary->Flip(primary, NULL, 0);
    setTimer(3);
}

void drawVolume(int32_t volume)
{
    IDirectFBImageProvider *provider;
    IDirectFBSurface *surface = NULL;
    int32_t surfaceHeight, surfaceWidth;
    char buffer[50];
    sprintf(buffer, "volume_%d.png", volume);
    if (black == 0)
    {
        fillTransparent();
    }
    else
    {
        fillBlack();
    }
    // printf("%s : buffer %s\n", __FUNCTION__, buffer);
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
    DFBCHECK(primary->Blit(primary, surface, NULL, 50, 50));
    primary->Flip(primary, NULL, 0);
    setTimer(3);
}







