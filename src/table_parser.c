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
#include "tables.h"
#include "remote.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void parsePatServiceInfoArray(uint8_t *buffer, PatServiceInfo patServiceInfoArray[], uint16_t section_length)
{
    int brojPidova = (section_length - 10) / 4;
    int kraj;
    int i = 0;
    if (brojPidova < MAX_NUM_OF_PIDS)
    {
        kraj = brojPidova;
    }
    else
    {
        kraj = MAX_NUM_OF_PIDS;
    }

    for (i = 0; i < kraj; i++)
    {
        patServiceInfoArray[i].program_number = (uint16_t) ((*(buffer + i * 4 + 8) << 8) + *(buffer + i * 4 + 9));
        patServiceInfoArray[i].pid = (uint16_t) (((*(buffer + i * 4 + 10) << 8) + *(buffer + i * 4 + 11)) & 0x1FFF);
    }
}

void parsePatHeader(uint8_t *buffer, PatHeader* patHeader)
{
    (*patHeader).table_id = (uint8_t) (*(buffer + 0));
    (*patHeader).section_syntax_indicator = (uint8_t) (*(buffer + 1) << 7);
    (*patHeader).section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);
    (*patHeader).transport_stream_id = (uint16_t) ((*(buffer + 3) << 8) + *(buffer + 4));
    (*patHeader).version_number = (uint8_t) ((*(buffer + 5) >> 1) & 0x1F);
    (*patHeader).current_next_indicator = (uint8_t) (*(buffer + 5) & 0x01);
    (*patHeader).section_number = (uint8_t) (*(buffer + 6));
    (*patHeader).last_section_number = (uint8_t) (*(buffer + 6));
}

void dumpPatHeader(PatHeader* patHeader)
{
    printf("\n<<<<<<<<<<<<<<<< Pat header >>>>>>>>>>>>>\n");
    printf("Table id:%d\n", (*patHeader).table_id);
    printf("Section syntax: %d\n", (*patHeader).section_syntax_indicator);
    printf("Section length: %d\n", (*patHeader).section_length);
    printf("Transport stream ID: %d\n", (*patHeader).transport_stream_id);
    printf("Version number: %d\n", (*patHeader).version_number);
    printf("Current next indicator: %d\n", (*patHeader).current_next_indicator);
    printf("Section number: %d\n", (*patHeader).section_number);
    printf("Last section number: %d", (*patHeader).last_section_number);
}

void parsePatTable(uint8_t *buffer, PatTable* table)
{
    printf("%s started\n", __FUNCTION__);
    parsePatHeader(buffer, table->patHeader);
    printf("%s header parsed\n", __FUNCTION__);
    parsePatServiceInfoArray(buffer, table->patServiceInfoArray, table->patHeader->section_length);
    printf("%s service info parsed\n", __FUNCTION__);
    (*table).serviceInfoCount = (uint8_t) ((*(*table).patHeader).section_length - 10) / 4;
}

void dumpPatServiceInfo(PatServiceInfo* patServiceInfo)
{
    printf("Program number: %d,pid: %d\n", patServiceInfo->program_number, patServiceInfo->pid);
}

void dumpPatTable(PatTable* table)
{
    int i = 0;
    dumpPatHeader((table->patHeader));
    printf("\n<<<<<<<<<<<<<<<< Pat service info >>>>>>>>>>>>>\n");
    for (i = 0; i < table->serviceInfoCount; i++)
        dumpPatServiceInfo(&(table->patServiceInfoArray[i]));
}

void parsePmt(uint8_t *buffer, PmtTable* table)
{
    parsePmtHeader(buffer, table->pmtHeader);
    parsePmtServiceInfoArray(buffer, table->pmtServiceInfoArray, &((*table).streamCount), &(table->teletekst));
}

void parsePmtHeader(uint8_t *buffer, PmtHeader* pmtHeader)
{
    (*pmtHeader).table_id = (uint8_t) (*(buffer + 0));
    (*pmtHeader).section_syntax_indicator = (uint8_t) (*(buffer + 1) >> 7);
    (*pmtHeader).section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);
    (*pmtHeader).program_number = (uint16_t) ((*(buffer + 3) << 8) + *(buffer + 4));
    (*pmtHeader).version_number = (uint8_t) ((*(buffer + 5) >> 1) & 0x1F);
    (*pmtHeader).current_next_indicator = (uint8_t) (*(buffer + 5) & 0x01);
    (*pmtHeader).section_number = (uint8_t) (*(buffer + 6));
    (*pmtHeader).last_section_number = (uint8_t) (*(buffer + 7));
    (*pmtHeader).pcr_pid = (uint16_t) (((*(buffer + 8) << 8) + *(buffer + 9)) & 0x1FFF);
    (*pmtHeader).program_info_length = (uint16_t) (((*(buffer + 10) << 8) + *(buffer + 11)) & 0x0FFF);
}

void parsePmtServiceInfoArray(uint8_t *buffer, PmtServiceInfo pmtServiceInfoArray[], uint8_t* broj, uint8_t* teletekst)
{
    uint8_t section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2)) & 0x0FFF);
    uint16_t program_info_length = (uint16_t) (((*(buffer + 10) << 8) + *(buffer + 11)) & 0x0FFF);
    int kraj = section_length - 1;
    int poc = program_info_length + 3 + 9;
    int i = 0;
    teletekst = 0;
    for (i = 0; poc < kraj; i++)
    {
        pmtServiceInfoArray[i].stream_type = (uint8_t) (*(buffer + poc));
        poc++;
        pmtServiceInfoArray[i].el_pid = (uint16_t) (((*(buffer + poc) << 8) + *(buffer + poc + 1)) & 0x1FFF);
        poc += 2;
        pmtServiceInfoArray[i].es_info_length = (uint16_t) (((*(buffer + poc) << 8) + *(buffer + poc + 1)) & 0x0FFF);
        poc += 2;
        *teletekst = (*(buffer + poc));
        printf("---:: %d\n", *teletekst);
        poc += pmtServiceInfoArray[i].es_info_length;
    }
    *broj = i;
}

void dumpPmtTable(PmtTable* pmtTable)
{
    int i = 0;
    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<< PMT TABLE >>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    printf("Table id: %d\n", pmtTable->pmtHeader->table_id);
    printf("Service info count: %d\n", pmtTable->streamCount);
    printf("Section syntax indicator id: %d\n", pmtTable->pmtHeader->section_syntax_indicator);
    printf("section_length: %d\n", pmtTable->pmtHeader->section_length);
    printf("program_number: %d\n", pmtTable->pmtHeader->program_number);
    printf("version_number: %d\n", pmtTable->pmtHeader->version_number);
    printf("current_next_indicator: %d\n", pmtTable->pmtHeader->current_next_indicator);
    printf("section_number: %d\n", pmtTable->pmtHeader->section_number);
    printf("last_section_number: %d\n", pmtTable->pmtHeader->last_section_number);
    printf("pcr_pid: %d\n", pmtTable->pmtHeader->pcr_pid);
    printf("program_info_length: %d\n", pmtTable->pmtHeader->program_info_length);
    for (i = 0; i < pmtTable->streamCount; i++)
    {
        printf("Service Type: %d el_pid: %d es_info_length %d\n", pmtTable->pmtServiceInfoArray[i].stream_type, pmtTable->pmtServiceInfoArray[i].el_pid, pmtTable->pmtServiceInfoArray[i].es_info_length);
    }
}

void parseEitTable(uint8_t* buffer, EitTable* table)
{
    (*table).table_id = (uint8_t) (*(buffer + 0));
    (*table).section_syntax_indicator = (uint8_t) (*(buffer + 1) << 7);
    (*table).section_length = (uint16_t) (((*(buffer + 1) << 8) + *(buffer + 2))
            & 0x0FFF);
    (*table).service_id = (uint16_t) ((*(buffer + 3) << 8) + *(buffer + 4));
    (*table).version_number = (uint8_t) *(buffer + 5) & 0x3E;
    (*table).current_next_indicator = (uint8_t) *(buffer + 5) & 0x01;
    (*table).section_number = (uint8_t) *(buffer + 6);
    (*table).last_section_number = (uint8_t) *(buffer + 7);
    (*table).transport_stream_id = (uint8_t) (*(buffer + 8) << 8)
            + *(buffer + 9);

}

void dumpEitTable(EitTable* table)
{
    printf("\n<<<<<<<<<<<<<<<<EIT table>>>>>>>>>>>>>>>>>>\n");
    printf("Table id: %d\n", table->table_id);
    printf("Section syntax indicator %d\n", table->section_syntax_indicator);
    printf("Section length %d\n", table->section_length);
    printf("Service id %d\n", table->service_id);
    printf("Version number %d\n", table->version_number);
    printf("current_next_indicator %d\n", table->current_next_indicator);
    printf("section_number %d\n", table->section_number);
    printf("last_section_number %d\n", table->last_section_number);
    printf("transport_stream_id %d\n", table->transport_stream_id);
    printf("<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>\n");
}
