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

#ifndef CONFIG_PARSER_H
#define	CONFIG_PARSER_H


#define MAXLEN 80
#define MAX_VAL_LEN 10
#define CONFIG_FILE_PATH "config.ini"
#include "tdp_api.h"
#include "tables.h"

typedef struct config_parameters_s
{
    uint32_t frequency;
    uint32_t bandwidth;
    uint32_t aPid;
    uint32_t vPid;
    t_Module module;
    tStreamType aType;
    tStreamType vType;
}
config_parameters;

int32_t parseConfig(config_parameters * parms, char* config_file_path);
void dumpConfig(const config_parameters * const parms);
void initDefaultValues(config_parameters* parms);
tStreamType str2AudioType(char* aType);
tStreamType str2VideoType(char* vType);
t_Module str2TModule(char* module);

#endif	/* CONFIG_PARSER_H */

