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
 * \file drawing.h
 * \brief
 * Ovaj modul se kroisti za iscrtavanje informacija na ekran.
 * 
 * @Author Milan Maric
 * \notes
 *
 *****************************************************************************/

#ifndef drawing_h
#define drawing_h


#include <stdint.h>
#include <directfb.h>



/* helper macro for error checking */
#define DFBCHECK(x...)                                      \
{                                                           \
DFBResult err = x;                                          \
                                                            \
if (err != DFB_OK)                                          \
  {                                                         \
    fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ );  \
    DirectFBErrorFatal( #x, err );                          \
  }                                                         \
}

/****************************************************************************
 *
 * @brief
 * Fukcija koja se kroisti za iscrtavanje informacija o trenutnom programu
 *
 * @param
 * service_number - [in] btoj trenutnog programa
 * vpid - [in] PID video streama
 * apid - [in] PID audio streama
 * teletekst - [in] vrijednost da li program sadrzi teletekst ili ne (0 = ne,>0 da)
 *****************************************************************************/
void drawTextInfo(int32_t service_number, uint16_t vpid, uint16_t apid, uint8_t teletekst);

/****************************************************************************
 *
 * @brief
 * Fukcija koja se koristi za inicijalizaciju directFB komponenti
 * 
 *****************************************************************************/
void initDirectFB();

/****************************************************************************
 *
 * @brief
 * Fukcija koja se koristi za deinicijalizaciju directFB komponenti
 * 
 *****************************************************************************/
void deinitDirectFB();

/****************************************************************************
 *
 * @brief
 * Funkcija koja se koristi za iscrtavanje informacije o jacini zvuka
 *
 * @param
 * volume -[in] vrijednost renutne jacine zvuka (od 0 do 9)
 *****************************************************************************/
void drawVolume(int32_t volume);

/****************************************************************************
 *
 * @brief
 * Fukcija koja se koristi za popunjavanje ekrana netransparentnom bojom 
 * (u slucaju da kanal ne sadrzi video)
 * 
 *****************************************************************************/
void fillBlack();

/****************************************************************************
 *
 * @brief
 * Fukcija koja se koristi za popunjavanje ekrana transparentnom bojom
 * (brisanje iscrtanog sadrzaja)
 * 
 *****************************************************************************/
void fillTransparent();
#endif