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
 * \file config_parser.h
 * \brief
 * Ovaj modul se kroisti za iscrtavanje informacija na ekran.
 * 
 * @Author Milan Maric
 * \notes
 *
 *****************************************************************************/

#ifndef CONFIG_PARSER_H
#define	CONFIG_PARSER_H


#define MAXLEN 80
#define MAX_VAL_LEN 10
#define CONFIG_FILE_PATH "config.ini"
#include "tdp_api.h"

#define MHZ 1000000U

typedef struct config_parameters_s {
    uint32_t frequency;
    uint32_t bandwidth;
    uint32_t aPid;
    uint32_t vPid;
    t_Module module;
    tStreamType aType;
    tStreamType vType;
}
config_parameters;

/****************************************************************************
 *
 * @brief
 * Fukcija koja se koristi za parsiranje konfiguracione datoteke
 *
 * @param
 * parms - [out] struktura u koju ce se upisati parametri
 * config_file_path - [in] putanja do konfiguracione datoteke
* @return
NO_ERROR, ako nema greske
*
ERROR, u slucaju greske
*
*****************************************************************************/
int32_t parseConfig(config_parameters * parms, char* config_file_path);

/****************************************************************************
 *
 * @brief
 * Funkcija koja ispisuje konfiguracione parametre na standardni izlaz
 *
 * @param
 * parms - [in] parametri koji ce biti ispisani
 *****************************************************************************/
void dumpConfig(const config_parameters * const parms);

/****************************************************************************
 *
 * @brief
 * Funkcija koja ispisuje konfiguracione parametre na standardni izlaz
 *
 * @param
 * parms - [in] parametri koji ce biti ispisani
 *****************************************************************************/
void initDefaultValues(config_parameters* parms);

tStreamType str2AudioType(char* aType);
tStreamType str2VideoType(char* vType);
t_Module str2TModule(char* module);

#endif	/* CONFIG_PARSER_H */

