#ifndef remote_h
#define remote_h

#include <stdint.h>


#define NUM_EVENTS  5

#define NON_STOP    1

#define REMOTE_BTN_NUM_1 2
#define REMOTE_BTN_NUM_2 3
#define REMOTE_BTN_NUM_3 4
#define REMOTE_BTN_NUM_4 5
#define REMOTE_BTN_NUM_5 6
#define REMOTE_BTN_NUM_6 7
#define REMOTE_BTN_NUM_7 8
#define REMOTE_BTN_NUM_8 9
#define REMOTE_BTN_NUM_9 10
#define REMOTE_BTN_NUM_0 11

#define REMOTE_BTN_TEXT 371
#define REMOTE_BTN_SUBT 370
#define REMOTE_BTN_AUDIO 392

#define REMOTE_BTN_V_PLUS 63
#define REMOTE_BTN_V_MINUS 64
#define REMOTE_BTN_P_PLUS 62
#define REMOTE_BTN_P_MINUS 61

#define REMOTE_BTN_INFO 358
#define REMOTE_BTN_EXIT 102

/* error codes */
#define NO_ERROR 		0
#define ERROR			1


int32_t getKeys(int32_t count, uint8_t* buf, int32_t* eventRead);
void* remote_control_thread(void*);

#endif