#ifndef TABLES_H_
#define TABLES_H_

#include <stdint.h>

#define MAX_NUM_OF_PIDS 20
//7
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
//19
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
//32
void parsePat(uint8_t *buffer,PatTable* table);
void parseHeader(uint8_t *buffer,PatHeader* patHeader);
void parseServiceInfoArray(uint8_t *buffer,PatServiceInfo patServiceInfoArray[MAX_NUM_OF_PIDS],uint16_t section_length);
void dumpTable(PatTable* table);


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
//19
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
//32
void parsePmt(uint8_t *buffer,PmtTable* table);
void parsePmtHeader(uint8_t *buffer,PmtHeader* pmtHeader);
void parsePmtServiceInfoArray(uint8_t *buffer,PmtServiceInfo pmtServiceInfoArray[MAX_NUM_OF_PIDS]);
void dumpPmtTable(PmtTable* pmtTable);
#endif