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
#ifndef remote_h
#define remote_h

#include <stdint.h>


#define NUM_EVENTS  5

#define NON_STOP    1

typedef enum remoteButton_Code
{
    REMOTE_BTN_NUM_1 = 2,
    REMOTE_BTN_NUM_2 = 3,
    REMOTE_BTN_NUM_3 = 4,
    REMOTE_BTN_NUM_4 = 5,
    REMOTE_BTN_NUM_5 = 6,
    REMOTE_BTN_NUM_6 = 7,
    REMOTE_BTN_NUM_7 = 8,
    REMOTE_BTN_NUM_8 = 9,
    REMOTE_BTN_NUM_9 = 10,
    REMOTE_BTN_NUM_0 = 11,
    REMOTE_BTN_TEXT = 371,
    REMOTE_BTN_SUBT = 370,
    REMOTE_BTN_AUDIO = 392,

    REMOTE_BTN_V_PLUS = 63,
    REMOTE_BTN_V_MINUS = 64,
    REMOTE_BTN_P_PLUS = 62,
    REMOTE_BTN_P_MINUS = 61,

    REMOTE_BTN_INFO = 358,
    REMOTE_BTN_EXIT = 102
} remoteButtonCode;


typedef int32_t(*Remote_Control_Callback)(uint32_t status);

void registerRemoteCallBacks(Remote_Control_Callback remoteControllCallback);

void* remote_control_thread(void*);

#endif