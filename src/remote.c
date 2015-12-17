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
#include "remote.h"
#include <stdio.h>
#include <stdint.h>
#include <linux/input.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include "drawing.h"

static int32_t inputFileDesc;
// 
void* remote_control_thread(void* nn)
{
    const char* dev = "/dev/input/event0";
    char deviceName[20];
    struct input_event* eventBuf;
    uint32_t eventCnt;
    uint32_t i;
    uint32_t service_number = 0;
    uint32_t tmp_number;
    inputFileDesc = open(dev, O_RDWR);
    if (inputFileDesc == -1)
    {
        printf("Error while opening device (%s) !", strerror(errno));
        return;
    }

    ioctl(inputFileDesc, EVIOCGNAME(sizeof (deviceName)), deviceName);
    printf("RC device opened succesfully [%s]\n", deviceName);

    eventBuf = malloc(NUM_EVENTS * sizeof (struct input_event));
    if (!eventBuf)
    {
        printf("Error allocating memory !");
        return;
    }

    while (NON_STOP)
    {
        /* read input eventS */
        if (getKeys(NUM_EVENTS, (uint8_t*) eventBuf, &eventCnt))
        {
            printf("Error while reading input events !");
            return;
        }

        for (i = 0; i < eventCnt; i++)
        {
            if (eventBuf[i].value == 1 && eventBuf[i].type == 1)
            {
                switch (eventBuf[i].code)
                {
                case REMOTE_BTN_P_PLUS:
                    service_number++;
                    break;
                case REMOTE_BTN_P_MINUS:
                    if (service_number != 0) service_number--;
                    break;
                case REMOTE_BTN_INFO:
                    printf("Service number: %d\n", service_number);
                    drawTextInfo(service_number);
                    break;
                case REMOTE_BTN_EXIT:
                    return;
                default:
                    tmp_number = remote_check_service_number(eventBuf[i].code);
                    if (tmp_number != -1)
                    {
                        service_number = tmp_number;
                        printf("Service number: %d\n", tmp_number);
                    }
                }
            }

        }

    }
    free(eventBuf);
}

int32_t remote_check_service_number(int32_t code)
{
    int32_t service_number = -1;
    switch (code)
    {
    case REMOTE_BTN_NUM_1:
        service_number = 1;
        break;
    case REMOTE_BTN_NUM_2:
        service_number = 2;
        break;
    case REMOTE_BTN_NUM_3:
        service_number = 3;
        break;
    case REMOTE_BTN_NUM_4:
        service_number = 4;
        break;
    case REMOTE_BTN_NUM_5:
        service_number = 5;
        break;
    case REMOTE_BTN_NUM_6:
        service_number = 6;
        break;
    case REMOTE_BTN_NUM_7:
        service_number = 7;
        break;
    case REMOTE_BTN_NUM_8:
        service_number = 8;
        break;
    case REMOTE_BTN_NUM_9:
        service_number = 9;
        break;
    case REMOTE_BTN_NUM_0:
        service_number = 0;
        break;
    }
    return service_number;
}

int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventsRead)
{
    int32_t ret = 0;

    /* read input events and put them in buffer */
    ret = read(inputFileDesc, buf, (size_t) (count * (int) sizeof (struct input_event)));
    if (ret <= 0)
    {
        printf("Error code %d", ret);
        return ERROR;
    }
    /* calculate number of read events */
    *eventsRead = ret / (int) sizeof (struct input_event);

    return NO_ERROR;
}
