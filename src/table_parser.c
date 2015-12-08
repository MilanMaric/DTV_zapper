#include "tables.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
//9
void parseServiceInfoArray(uint8_t *buffer,PatServiceInfo patServiceInfoArray[],uint16_t section_length){

  int brojPidova=(section_length-10)/4;
  int kraj;
  if (brojPidova<MAX_NUM_OF_PIDS){
    kraj=brojPidova;
  }
  else{
	  kraj=MAX_NUM_OF_PIDS;
  }
  int i=0;
  printf("\nkraj %d\n",kraj);
  for(i=0;i<kraj;i++){
    patServiceInfoArray[i].program_number=(uint16_t) ((*(buffer+i*4+8) << 8) + *(buffer+i*4+9));
    patServiceInfoArray[i].pid=(uint16_t) (((*(buffer+i*4+10) << 8) + *(buffer+i*4+11)) & 0x1FFF);
  }
}
//26
void parseHeader(uint8_t *buffer,PatHeader* patHeader){
  (*patHeader).table_id=(uint8_t) (*(buffer+0));
  (*patHeader).section_syntax_indicator=(uint8_t) (*(buffer+1)<<7);
  (*patHeader).section_length=(uint16_t) (((*(buffer+1) << 8) + *(buffer+2)) & 0x0FFF);
  (*patHeader).transport_stream_id=(uint16_t) ((*(buffer+3) << 8) + *(buffer+4));
  (*patHeader).version_number=(uint8_t) ((*(buffer+5) >> 1) & 0x1F);
  (*patHeader).current_next_indicator=(uint8_t) (*(buffer+5) & 0x01);
  (*patHeader).section_number=(uint8_t) (*(buffer+6));
  (*patHeader).last_section_number=(uint8_t) (*(buffer+6));
}
//37
void dumpHeader(PatHeader* patHeader){
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

void parseTable(uint8_t *buffer,PatTable* table){
	parseHeader(buffer,table->patHeader);
	dumpHeader(table->patHeader);
	parseServiceInfoArray(buffer,table->patServiceInfoArray,table->patHeader->section_length);
	dumpHeader(table->patHeader);
	(*table).serviceInfoCount=(uint8_t)((*(*table).patHeader).section_length-10)/4;
}



void dumpServiceInfo(PatServiceInfo* patServiceInfo){
  printf("Program number: %d,pid: %d\n", patServiceInfo->program_number,patServiceInfo->pid);
}
void dumpTable(PatTable* table){
  int i=0;
  dumpHeader((table->patHeader));
  printf("\n<<<<<<<<<<<<<<<< Pat service info >>>>>>>>>>>>>\n");
 for(i=0;i<table->serviceInfoCount;i++)
  dumpServiceInfo(&(table->patServiceInfoArray[i]));
}


void parsePmt(uint8_t *buffer,PmtTable* table){
  parsePmtHeader(buffer,table->pmtHeader);
  parsePmtServiceInfoArray(buffer,table->pmtServiceInfoArray);
}

void parsePmtHeader(uint8_t *buffer,PmtHeader* pmtHeader){
  (*pmtHeader).table_id=(uint8_t) (*(buffer+0));
  (*pmtHeader).section_syntax_indicator=(uint8_t) (*(buffer+1)<<7);
  (*pmtHeader).section_length=(uint16_t) (((*(buffer+1) << 8) + *(buffer+2)) & 0x0FFF);
  (*pmtHeader).program_number=(uint16_t) ((*(buffer+3) << 8) + *(buffer+4));
  (*pmtHeader).version_number=(uint8_t) ((*(buffer+5) >> 1) & 0x1F);
  (*pmtHeader).current_next_indicator=(uint8_t) (*(buffer+5) & 0x01);
  (*pmtHeader).section_number=(uint8_t) (*(buffer+6));
  (*pmtHeader).last_section_number=(uint8_t) (*(buffer+6));
  (*pmtHeader).pcr_pid=(uint16_t) (((*(buffer+7) << 8) + *(buffer+8)) & 0x1FFF);
  (*pmtHeader).program_info_length=(uint16_t) (((*(buffer+9) << 8) + *(buffer+10)) & 0x0FFF);
}

void parsePmtServiceInfoArray(uint8_t *buffer,PmtServiceInfo pmtServiceInfoArray[MAX_NUM_OF_PIDS]){
  uint8_t section_length=(uint16_t) (((*(buffer+1) << 8) + *(buffer+2)) & 0x0FFF);
  uint16_t program_info_length=(uint16_t) (((*(buffer+9) << 8) + *(buffer+10)) & 0x0FFF);
  int kraj=section_length-1;
  int poc=program_info_length+3+9;
  int i=0;
  for(i=0;poc<=kraj;i++){
    pmtServiceInfoArray[i].stream_type=(uint8_t) (*(buffer+poc));
    poc++;
    pmtServiceInfoArray[i].el_pid=(uint16_t) (((*(buffer+poc) << 8) + *(buffer+poc+1)) & 0x1FFF);
    poc+=2;
    pmtServiceInfoArray[i].es_info_length=(uint16_t) (((*(buffer+poc) << 8) + *(buffer+poc+1)) & 0x0FFF);
    poc+=2;
    poc+=pmtServiceInfoArray[i].es_info_length;
  }
}


void dumpPmtTable(PmtTable* pmtTable){
   printf("Table id: %d\n",pmtTable->pmtHeader->table_id);
  printf("Section syntax indicator id: %d\n",pmtTable->pmtHeader->section_syntax_indicator);
  printf("section_length: %d\n",pmtTable->pmtHeader->section_length);
  printf("program_number: %d\n",pmtTable->pmtHeader->program_number);
  printf("version_number: %d\n",pmtTable->pmtHeader->version_number);
  printf("current_next_indicator: %d\n",pmtTable->pmtHeader->current_next_indicator);
  printf("section_number: %d\n",pmtTable->pmtHeader->section_number);
  printf("last_section_number: %d\n",pmtTable->pmtHeader->last_section_number);
  printf("pcr_pid: %d\n",pmtTable->pmtHeader->pcr_pid);
  printf("program_info_length: %d\n",pmtTable->pmtHeader->program_info_length);
}

