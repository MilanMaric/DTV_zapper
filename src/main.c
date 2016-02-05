/****************************************************************************
 *
 * Univerzitet u Banjoj Luci, Elektrotehnicki fakultet
 *
 * -----------------------------------------------------
 * Ispitni zadatak iz predmeta:
 *
 * MULTIMEDIJALNI SISTEMI
 * -----------------------------------------------------
 * DTV zapper
 * -----------------------------------------------------
 *
 * \file main.c
 * \brief
 * Daatoteka glavnog podprograma, odakle se vrsi inicijalizacija svih modula
 * 
 * @Author Milan Maric
 * \notes
 *
 *****************************************************************************/

#include <stdio.h>
#include <directfb.h>
#include <pthread.h>
#include <stdint.h>
#include "remote.h"
#include "drawing.h"
#include "table_parser.h"
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

    deviceDeInit(&handle);
    deinitDirectFB();
    return 0;
}
