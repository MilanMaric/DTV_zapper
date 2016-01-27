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
 * \file remote.h
 * \brief
 * Ovaj modul predstavlja drajver za daljinski upravljac..
 * 
 * @Author Milan Maric
 * \notes
 *
 *****************************************************************************/
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
    REMOTE_BTN_VOLUME_PLUS = 63,
    REMOTE_BTN_VOLUME_MINUS = 64,
    REMOTE_BTN_PROGRAM_PLUS = 62,
    REMOTE_BTN_PROGRAM_MINUS = 61,
    REMOTE_BTN_INFO = 358,
    REMOTE_BTN_EXIT = 102,
    REMOTE_BTN_MUTE = 60
} remoteButtonCode;

typedef enum
{
    VOLUME_PLUS = 1,
    VOLUME_MINUS,
    VOLUME_MUTE
} RemoteVolumeCode;

typedef int32_t(*Remote_Control_Callback)(uint32_t code);
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
void registerServiceNumberRemoteCallBack(Remote_Control_Callback remote_ControllCallback);

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
void registerVolumeRemoteCallback(Remote_Control_Callback remoteControllCallback);

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
void registerInfoButtonCallback(Remote_Control_Callback remoteControllCallback);


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
void* remoteControlThread(void*);

#endif