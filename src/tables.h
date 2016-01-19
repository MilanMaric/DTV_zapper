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
 * @Author Petar Petrovic
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
#ifndef TABLES_H_
#define TABLES_H_

#include <stdint.h>
#include "config_parser.h"

#define MAX_NUM_OF_PIDS 20
#define PARSING_ERROR -1
#define INIT_ERROR -1

typedef struct _PatHeader
{
    uint8_t table_id;
    uint8_t section_syntax_indicator;
    uint16_t section_length;
    uint16_t transport_stream_id;
    uint8_t version_number;
    uint8_t current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;
} PatHeader;

typedef struct _PatServiceInfo
{
    uint16_t program_number;
    uint16_t pid;
} PatServiceInfo;

typedef struct _PatTable
{
    PatServiceInfo patServiceInfoArray[MAX_NUM_OF_PIDS];
    uint8_t serviceInfoCount;
    PatHeader* patHeader;
} PatTable;

typedef struct _PmtHeader
{
    uint8_t table_id;
    uint8_t section_syntax_indicator;
    uint16_t section_length;
    uint16_t program_number;
    uint8_t version_number;
    uint8_t current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;
    uint16_t pcr_pid;
    uint16_t program_info_length;
} PmtHeader;

typedef struct _PmtServiceInfo
{
    uint8_t stream_type;
    uint16_t el_pid;
    uint16_t es_info_length;
} PmtServiceInfo;

typedef struct _PmtTable
{
    PmtHeader* pmtHeader;
    PmtServiceInfo pmtServiceInfoArray[MAX_NUM_OF_PIDS];
    uint8_t streamCount;
} PmtTable;

typedef struct _EitTable
{
    uint8_t table_id; //8 	Indicates to which table this section belongs, in this case EIT.
    uint8_t section_syntax_indicator; //1 Indicates whether a sub-table structure including CRC check is used.
    uint16_t section_length; // 12	The length of the section in bytes. This length starts immediately after this field and includes the CRC
    uint16_t service_id; //16	Indicates on which service this EIT table informs.
    uint8_t version_number; //	5	Value between 0 to 31. A higher value indicates that the information has changed.
    uint8_t current_next_indicator; //	1	This is to indicate whether a section is 'valid now' or 'valid in future'
    uint8_t section_number; //	8	Value between 0x00 - 0xFF. Used to indicate the sections of a table. Up to 256 sections
    uint8_t last_section_number; //	8	The number of the last section, so the receiver know when the table is completely received.
    uint16_t transport_stream_id; //	16	Indicates on what TS this EIT table information provides.
    uint16_t original_network_id; //	12	The ID of the original network where this transport stream originated.
    uint8_t segment_last_section_number; //	8	This 8 bit field gives the number of the last section of this segment of the sub-table. If the sub-table is not segmented, then this field must have the same value as the last section number field.
    uint8_t last_table_id; //	8	This 8 bit field indicates the last used table_id.
    uint16_t event_id; //	16	This 16 bit field indicates the event id of the event for which information is given. Within a service this id must be unique.
    //start time	40	This 40 bit field gives the start time and date in UTC and MJD of the event. The first 16 bits represent the 16 bits MJD,then the 24-bit UTC as 6 digits in 4-bit BCD
    uint32_t duration; //	24	This 24 bit field indicates the length of the event in hours, minutes, seconds as 4 bits BCD. for instance 02:25:30 is encoded as 0x022530
    uint8_t running_status; //	3	This field gives information about the status of the event, 000 = undefined, 001 = not running, 010 = start in a few seconds, 011 = pause, 100 = running, 101 - 111 reserved for future use. In the case of an NVOD reference event, the running status will be put to '0'
    uint16_t descriptor_loop_length; //12	The length of the descriptor loop.
} EitTable;

void parseEitTable(uint8_t* buffer, EitTable* table);
void dumpEitTable(EitTable* table);

void parsePatServiceInfoArray(uint8_t *buffer, PatServiceInfo patServiceInfoArray[], uint16_t section_length);
void parsePatHeader(uint8_t *buffer, PatHeader* patHeader);
void parsePatTable(uint8_t *buffer, PatTable* table);
void parsePmt(uint8_t *buffer, PmtTable* table);
void parsePmtHeader(uint8_t *buffer, PmtHeader* pmtHeader);
void parsePmtServiceInfoArray(uint8_t *buffer, PmtServiceInfo pmtServiceInfoArray[], uint8_t* broj);
void dumpPatTable(PatTable* table);
void dumpPmtTable(PmtTable* pmtTable);
void dumpPatHeader(PatHeader* patHeader);
void dumpPatServiceInfo(PatServiceInfo* patServiceInfo);




#endif