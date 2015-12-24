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
#include <stdio.h>
#include <directfb.h>
#include <pthread.h>
#include <stdint.h>
#include "remote.h"
#include "drawing.h"
#include "tables.h"
#include "remote.h"
#include "config_parser.h"
#include "device_control.h"

int32_t main(int32_t argc, char** argv)
{
    DeviceHandle handle;
    config_parameters parms;
    /* initialize DirectFB */
    //  DFBCHECK(DirectFBInit(&argc, &argv));
    // initDirectFB();
    if (parseConfig(&parms, argv[1]) == ERROR)
    {
        printf("%s : ERROR while parsing configuration\n", __FUNCTION__);
        return ERROR;
    }
    dumpConfig(&parms);
    pthread_t remote_thread;
    pthread_create(&remote_thread, NULL, &remoteControlThread, NULL);
    if (deviceInit(&parms, &handle) == ERROR)
    {
        printf("%s : ERROR while init \n", __FUNCTION__);
        return ERROR;
    }
    
    //deinitDirectFB();
    pthread_join(remote_thread, NULL);
    deviceDeInit(&handle);
    return 0;
}
