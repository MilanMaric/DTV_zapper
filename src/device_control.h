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
 * \file device_control.h
 * \brief
 * Ovaj modul se koristi za kontrolu uredjaja.
 * 
 * @Author Milan Maric
 * \notes
 *
 *****************************************************************************/

#ifndef DEVICE_CONTROL_H
#define	DEVICE_CONTROL_H

typedef struct _Handles
{
    uint32_t sourceHandle;
    uint32_t playerHandle;
    uint32_t filterHandle;
    uint32_t vStreamHandle;
    uint32_t aStreamHandle;
} DeviceHandle;

/****************************************************************************
 *
 * @brief
 * Funkcija koja omogucava inicijalizaciju uredjaja. Ova funkcija poziva odgovarajuce
 * funkcije za dobavljanje i parsiranje PAT i PMT tabela.
 *
 * @param parms - [in] struktura parametara procitanih iz konfiguracione datoteke
 * @param handle - [out] struktura odgovarajucih handle vrijednosti
 * @return NO_ERROR, ako nema greske, ERROR, u slucaju greske
 *****************************************************************************/
int deviceInit(config_parameters *parms, DeviceHandle *handle);

/****************************************************************************
 *
 * @brief
 * Funkcija koja omogucava deinicijalizaciju uredjaja.
 *
 * @param handle - [out] handle vrijednosti koje ce biti izmjenjene prilikom 
 * deinicijalizacije uredjaja
 *****************************************************************************/
void deviceDeInit(DeviceHandle *handle);

/****************************************************************************
 *
 * @brief
 * Funkcija koja ce biti pozvana kao callback funkcija pri promjeni programa.
 *
 * @param service_number - [in] redni broj programa (pocevsi od 1)
 * @return NO_ERROR, ako nema greske, ERROR, u slucaju greske
 *****************************************************************************/
int32_t remoteServiceCallback(uint32_t service_number);

/****************************************************************************
 *
 * @brief
 * Funkcija koja ce biti pozvana kao callback funkcija pri promjeni jacine zvuka.
 *
 * @param service_number - [in] VOLUME_UP, VOLUME_DOWN ili MUTE
 * @return NO_ERROR, ako nema greske, ERROR, u slucaju greske
 *****************************************************************************/
int32_t remoteVolumeCallback(uint32_t service);

/****************************************************************************
 *
 * @brief
 * Funkcija koja ce biti pozvana kao callback funkcija pri pritisku info tastera.
 *
 * @param code - [in] redni broj programa (pocevsi od 1)
 * @return NO_ERROR, ako nema greske, ERROR, u slucaju greske
 *****************************************************************************/
int32_t remoteInfoCallback(uint32_t code);

#endif	/* DEVICE_CONTROL_H */

