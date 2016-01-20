/****************************************************************************
 *
 * Univerzitet u Banjoj Luci, Elektrotehnicki fakultet
 *
 * -----------------------------------------------------
 * Ispitni zadatak iz predmeta:
 *
 * MULTIMEDIJALNI SISTEMI
 * -----------------------------------------------------
 * Naslov zadatka (npr. DVB Sniffer za EIT/SDT)
 * -----------------------------------------------------
 *
 * \file table_parser.c
 * \brief
 * Ovaj modul realizuje parsiranje PMT,PAT i EIT tabela, uz postojanje fukcija za
 * ispis sadrzaja na standardni izlaz.
 * 
 * @Author Milan Maric
 * \notes
 *
 *****************************************************************************/
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
    DFBCHECK(DirectFBInit(&argc, &argv));
    initDirectFB();
    if (argc == 2)
    {
        if (parseConfig(&parms, argv[1]) == ERROR)
        {
            printf("%s : ERROR while parsing configuration\n", __FUNCTION__);
            deinitDirectFB();
            return ERROR;
        }
    }
    else
    {
        if (parseConfig(&parms, "/home/my_config/config.ini") == ERROR)
        {
            printf("%s : ERROR while parsing configuration\n", __FUNCTION__);
            deinitDirectFB();
            return ERROR;
        }
    }
 //   dumpConfig(&parms);
    pthread_t remote_thread;
    if (deviceInit(&parms, &handle) == ERROR)
    {
        printf("%s : ERROR while init \n", __FUNCTION__);
        deinitDirectFB();
        deviceDeInit(&handle);
        return ERROR;
    }

    registerServiceNumberRemoteCallBack(remoteServiceCallback);
    registerVolumeRemoteCallback(remoteVolumeCallback);
    registerInfoButtonCallback(remoteInfoCallback);
    pthread_create(&remote_thread, NULL, &remoteControlThread, NULL);

    pthread_join(remote_thread, NULL);
    fillBlack();

    deviceDeInit(&handle);
    deinitDirectFB();
    return 0;
}
