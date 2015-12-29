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

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include "config_parser.h"
#include "tdp_api.h"
#include "tables.h"

char* trim(char * s)
{
    char *s1 = s, *s2 = &s[strlen(s) - 1];
    while ((isspace(*s2)) && (s2 >= s1))
        s2--;
    *(s2 + 1) = '\0';
    /* Trim left side */
    while ((isspace(*s1)) && (s1 < s2))
        s1++;

    /* Copy finished string */
    strcpy(s, s1);
    return s;
}

int32_t parseConfig(config_parameters * parms, char* config_file_path)
{
    char *s, buff[256];
    FILE *fp = fopen(config_file_path, "r");
    if (fp == NULL)
    {
        printf("%s file can't be opened\n", __FUNCTION__);
        return;
    }
    while ((s = fgets(buff, sizeof buff, fp)) != NULL)
    {
        if (buff[0] == '\n' || buff[0] == '#')
            continue;
        char name[MAXLEN], value[MAXLEN];
        s = strtok(buff, "=");
        if (s == NULL)
            continue;
        else
            strncpy(name, s, MAXLEN);
        s = strtok(NULL, "=");
        if (s == NULL)
            continue;
        else
            strncpy(value, s, MAX_VAL_LEN);
        trim(value);

        if (strcmp(name, "frequency") == 0)
            sscanf(value, "%d", &(parms->frequency));
        else if (strcmp(name, "bandwidth") == 0)
            sscanf(value, "%d", &(parms->bandwidth));
        else if (strcmp(name, "aPID") == 0)
            sscanf(value, "%d", &(parms->aPid));
        else if (strcmp(name, "vPID") == 0)
            sscanf(value, "%d", &(parms->vPid));
        else if (strcmp(name, "module") == 0)
        {
            parms->module = str2TModule(value);
        }
        else if (strcmp(name, "aType") == 0)
        {
            parms->aType = str2AudioType(value);
            if (parms->aType < 0 || parms->aType == AUDIO_TYPE_UNSUPPORTED)
                return ERROR;
        }
        else if (strcmp(name, "vType") == 0)
        {
            parms->vType = str2VideoType(value);
            if (parms->vType < 0 || parms->vType == VIDEO_TYPE_WMV3 + 1)
                return ERROR;
        }
        else
            printf("WARNING: %s/%s: Unknown name/value pair!\n",
                   name, value);
    }

    /* Close file */
    fclose(fp);
}

void dumpConfig(const config_parameters * const parms)
{
    printf("frequency:%d\n", parms->frequency);
    printf("bandwidth:%d\n", parms->bandwidth);
    printf("module:%d\n", parms->module);
    printf("vPid:%d\n", parms->vPid);
    printf("aPid:%d\n", parms->aPid);
    printf("vType:%d\n", parms->vType);
    printf("aType:%d\n", parms->aType);
}

void initDefaultValues(config_parameters* parms)
{
    if (parms == NULL)
    {
        parms = (config_parameters*) malloc(sizeof (config_parameters));
    }
    parms->frequency = 470;
    parms->bandwidth = 8;
    parms->aPid = 101;
    parms->vPid = 101;
    parms->module = DVB_T;
    parms->aType = AUDIO_TYPE_DOLBY_AC3;
    parms->vType = VIDEO_TYPE_MPEG2;
}

tStreamType str2AudioType(char* aType)
{
    printf("%s : %s", __FUNCTION__, aType);
    if (strcmp(aType, "ac3") == 0)
        return AUDIO_TYPE_DOLBY_AC3;
    if (strcmp(aType, "plus") == 0)
        return AUDIO_TYPE_DOLBY_PLUS;
    if (strcmp(aType, "true_hd"))
        return AUDIO_TYPE_DOLBY_TRUE_HD;
    if (strcmp(aType, "lpcm_sd"))
        return AUDIO_TYPE_LPCM_SD;
    if (strcmp(aType, "lpcm_bd"))
        return AUDIO_TYPE_LPCM_BD;
    if (strcmp(aType, "lpcm_hd"))
        return AUDIO_TYPE_LPCM_HD;
    if (strcmp(aType, "mlp"))
        return AUDIO_TYPE_MLP;
    if (strcmp(aType, "dts"))
        return AUDIO_TYPE_DTS;
    if (strcmp(aType, "dts_hd"))
        return AUDIO_TYPE_DTS_HD;
    if (strcmp(aType, "mpeg_audio"))
        return AUDIO_TYPE_MPEG_AUDIO;
    if (strcmp(aType, "mp3"))
        return AUDIO_TYPE_MP3;
    if (strcmp(aType, "he_aac"))
        return AUDIO_TYPE_HE_AAC;
    if (strcmp(aType, "wma"))
        return AUDIO_TYPE_WMA;
    if (strcmp(aType, "wma_pro"))
        return AUDIO_TYPE_WMA_PRO;
    if (strcmp(aType, "wma_lossless"))
        return AUDIO_TYPE_WMA_LOSSLESS;
    if (strcmp(aType, "raw_pcm"))
        return AUDIO_TYPE_RAW_PCM;
    if (strcmp(aType, "sdds"))
        return AUDIO_TYPE_SDDS;
    if (strcmp(aType, "dd_dcv"))
        return AUDIO_TYPE_DD_DCV;
    if (strcmp(aType, "dra"))
        return AUDIO_TYPE_DRA;
    if (strcmp(aType, "dra_ext"))
        return AUDIO_TYPE_DRA_EXT;
    if (strcmp(aType, "dra_lbr"))
        return AUDIO_TYPE_DTS_LBR;
    if (strcmp(aType, "dts_hres"))
        return AUDIO_TYPE_DTS_HRES;
    if (strcmp(aType, "lpcm_sesf"))
        return AUDIO_TYPE_LPCM_SESF;
    if (strcmp(aType, "dv_sd"))
        return AUDIO_TYPE_DV_SD;
    if (strcmp(aType, "vorbis"))
        return AUDIO_TYPE_VORBIS;
    if (strcmp(aType, "flac"))
        return AUDIO_TYPE_FLAC;
    if (strcmp(aType, "raw_aac"))
        return AUDIO_TYPE_RAW_AAC;
    if (strcmp(aType, "ra8"))
        return AUDIO_TYPE_RA8;
    if (strcmp(aType, "raac"))
        return AUDIO_TYPE_RAAC;
    if (strcmp(aType, "adpcm"))
        return AUDIO_TYPE_ADPCM;
    if (strcmp(aType, "spdif_input"))
        return AUDIO_TYPE_SPDIF_INPUT;
    if (strcmp(aType, "G711A"))
        return AUDIO_TYPE_G711A;
    if (strcmp(aType, "G711U"))
        return AUDIO_TYPE_G711U;
    if (strcmp(aType, "raw_signed_pcm"))
        return AUDIO_RAW_SIGNED_PCM;
    if (strcmp(aType, "raw_unsigned_pcm"))
        return AUDIO_RAW_UNSIGNED_PCM;
    if (strcmp(aType, "amr_wb"))
        return AUDIO_AMR_WB;
    if (strcmp(aType, "amr_nb"))
        return AUDIO_AMR_NB;
    return AUDIO_TYPE_UNSUPPORTED;
}

tStreamType str2VideoType(char* vType)
{
    printf("%s : %s", __FUNCTION__, vType);
    if (strcmp(vType, "mpeg2") == 0)
        return VIDEO_TYPE_MPEG2;
    if (strcmp(vType, "h264") == 0)
        return VIDEO_TYPE_H264;
    if (strcmp(vType, "vci") == 0)
        return VIDEO_TYPE_VC1;
    if (strcmp(vType, "mpeg4") == 0)
        return VIDEO_TYPE_MPEG4;
    if (strcmp(vType, "mpeg1") == 0)
        return VIDEO_TYPE_MPEG1;
    if (strcmp(vType, "jpeg") == 0)
        return VIDEO_TYPE_JPEG;
    if (strcmp(vType, "div3") == 0)
        return VIDEO_TYPE_DIV3;
    if (strcmp(vType, "div4") == 0)
        return VIDEO_TYPE_DIV4;
    if (strcmp(vType, "dx50") == 0)
        return VIDEO_TYPE_DX50;
    if (strcmp(vType, "mvc") == 0)
        return VIDEO_TYPE_MVC;
    if (strcmp(vType, "wmv3") == 0)
        return VIDEO_TYPE_WMV3;
    if (strcmp(vType, "divx") == 0)
        return VIDEO_TYPE_DIVX;
    if (strcmp(vType, "div5") == 0)
        return VIDEO_TYPE_DIV5;
    if (strcmp(vType, "rv30") == 0)
        return VIDEO_TYPE_RV30;
    if (strcmp(vType, "rv40") == 0)
        return VIDEO_TYPE_RV40;
    if (strcmp(vType, "vp60") == 0)
        return VIDEO_TYPE_VP6;
    if (strcmp(vType, "sorenson") == 0)
        return VIDEO_TYPE_SORENSON;
    if (strcmp(vType, "h263") == 0)
        return VIDEO_TYPE_H263;
    if (strcmp(vType, "jpeg_signle") == 0)
        return VIDEO_TYPE_JPEG_SINGLE;
    if (strcmp(vType, "vp8") == 0)
        return VIDEO_TYPE_VP8;
    if (strcmp(vType, "vp6f") == 0)
        return VIDEO_TYPE_VP6F;
    return VIDEO_TYPE_VP6F + 1;
}

t_Module str2TModule(char* module)
{
    if (strcmp(module, "DVT-T"))
        return DVB_T;
    if (strcmp(module, "DVT-T2"))
        return DVB_T2;
}


