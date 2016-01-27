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
 * \file remote.c
 * \brief
 * Ovaj modul predstavlja drajver za daljinski upravljac..
 * 
 * @Author Milan Maric
 * \notes
 *
 *****************************************************************************/

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
#include "tdp_api.h"

static int32_t inputFileDesc;
static Remote_Control_Callback sectionNumberCallback;
static Remote_Control_Callback volumeCallback;
static Remote_Control_Callback infoCallback;

/****************************************************************************
 *
 * @brief
 *Funkcija koja cita detektuje dogadjaje sa ulaza
 *
 * @param
 * count - [in] broj dogadjaja koji ce se ucitati
 * buf - [out] memorijska lokacija u koju ce biti upisani dogadjaji
 * eventsRead - [out] broj dogadjaja koji su ucitani
 *****************************************************************************/
int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventRead);
/****************************************************************************
 *
 * @brief
 * Fukcija koja se koristi za registovanje callback funkcije koja ce biti pozvana u slucaju promjene programa
 *
 * @param
 * remote_ControllCallback - [in] pokazivac na funkciju koja ce biti pozvana
 *
 *
 *
 *****************************************************************************/
void registerServiceNumberRemoteCallBack(Remote_Control_Callback remote_ControllCallback)
{
    sectionNumberCallback = remote_ControllCallback;
}

/****************************************************************************
 *
 * @brief
 * Fukcija koja se koristi za registovanje callback funkcije koja ce biti pozvana u slucaju promjene jacine zvuka
 *
 * @param
 * remote_ControllCallback - [in] pokazivac na funkciju koja ce biti pozvana
 *
 *
 *
 *****************************************************************************/
void registerVolumeRemoteCallback(Remote_Control_Callback remote_ControllCallback)
{
    volumeCallback = remote_ControllCallback;
}

/****************************************************************************
 *
 * @brief
 * Fukcija koja se koristi za registovanje callback funkcije koja ce biti pozvana u slucaju pritiska info dugmeta
 *
 * @param
 * remote_ControllCallback - [in] pokazivac na funkciju koja ce biti pozvana
 *
 *
 *
 *****************************************************************************/
void registerInfoButtonCallback(Remote_Control_Callback remote_ControllCallback)
{
    infoCallback = remote_ControllCallback;
}

/****************************************************************************
 *
 * @brief
 * Fukncija koja se treba kreirati kao poseban thread, a predstavlja beskonacnu petlju koja se vrti, 
 * i u slucaju pritiska dugmeta na daljinskom upravljacu aktivira se odgovarajuca callback funkcija.
 *
 *
 *
 *
 *****************************************************************************/
void* remoteControlThread(void* nn)
{
    const char* dev = "/dev/input/event0";
    char deviceName[20];
    struct input_event* eventBuf;
    uint32_t eventCnt;
    uint32_t i;
    uint32_t service_number = 1;
    uint32_t tmp_number;
    uint32_t tmp_number2;
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
    tmp_number = service_number;

    while (NON_STOP)
    {
        if (getKeys(NUM_EVENTS, (uint8_t*) eventBuf, &eventCnt))
        {
            printf("Error while reading input events !");
            return;
        }

        for (i = 0; i < eventCnt; i++)
        {
            if (eventBuf[i].value == 1 && eventBuf[i].type == 1)
            {
                tmp_number2 = service_number;
                switch (eventBuf[i].code)
                {
                case REMOTE_BTN_PROGRAM_PLUS:
                    if (sectionNumberCallback(service_number + 1) == NO_ERROR)
                    {
                        service_number++;
                    }
                    break;
                case REMOTE_BTN_PROGRAM_MINUS:

                    if (sectionNumberCallback(service_number - 1) == NO_ERROR)
                    {
                        service_number--;
                    }
                    break;
                case REMOTE_BTN_VOLUME_PLUS:
                    if (volumeCallback != NULL)
                    {
                        volumeCallback(VOLUME_PLUS);
                    }
                    break;
                case REMOTE_BTN_VOLUME_MINUS:
                    if (volumeCallback != NULL)
                    {
                        volumeCallback(VOLUME_MINUS);
                    }
                    break;
                case REMOTE_BTN_MUTE:
                    printf(" MUTE\n");
                    if (volumeCallback != NULL)
                    {
                        volumeCallback(VOLUME_MUTE);
                    }
                    break;
                case REMOTE_BTN_INFO:
                    if (infoCallback != NULL)
                    {
                        infoCallback(1);
                    }
                    break;
                case REMOTE_BTN_EXIT:
                    free(eventBuf);
                    return;
                default:
                    tmp_number = remoteCheckServiceNumberCode(eventBuf[i].code);
                    if (tmp_number != -1)
                    {
                        //  printf("****Service number: %d tmp_number\n", service_number);
                        if (sectionNumberCallback(tmp_number) == NO_ERROR)
                        {
                            service_number = tmp_number;
                        }
                    }
                }
            }
        }

    }
}
/****************************************************************************
 *
 * @brief
 * Fukcija koja u zavisnosti od pritisnutog dugmeta (broja) na daljinskom upravljacu
 * vraca vrijednost koja je pritisnuta
 *
 * @param
 * code - [in] kod pritisnutog dugmeta
 *
 *
 *
 *****************************************************************************/
int32_t remoteCheckServiceNumberCode(int32_t code)
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

/****************************************************************************
 *
 * @brief
 *Funkcija koja cita detektuje dogadjaje sa ulaza
 *
 * @param
 * count - [in] broj dogadjaja koji ce se ucitati
 * buf - [out] memorijska lokacija u koju ce biti upisani dogadjaji
 * eventsRead - [out] broj dogadjaja koji su ucitani
 *****************************************************************************/
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
