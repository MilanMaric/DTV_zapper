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
    uint8_t serviceInfoCount;
} PmtTable;

typedef struct _Handles
{
    uint32_t sourceHandle;
    uint32_t playerHandle;
    uint32_t filterHandle;
    uint32_t streamHandle;
} DeviceHandle;

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

int deviceInit(config_parameters *parms, DeviceHandle *handle);
void deviceDeInit();

#endif