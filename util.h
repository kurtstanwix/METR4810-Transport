#ifndef UTIL_H
#define	UTIL_H

#include <stdint.h>
#include <stdbool.h>

// Uncomment to debug over PC UART
//#define __USER_DEBUG

// Min and Max functions
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Status LED on the board
#define STATUS_LED_SetDirection(DIRECTION) IO_RA2_SetDirection(DIRECTION)
#define STATUS_LED_SetPin(STATE) IO_RA2_SetPin(STATE)

void init_debug(void);
void print_debug(char *toPrint, uint8_t length, bool newLine);
bool compare_strings(uint8_t *toCheck, char *str, uint8_t strLen);

// A macro to stitch together text and other defines
#define PREPROCESSOR_STITCH_HELPER(A, B) A ## B
#define PREPROCESSOR_STITCH(A, B) PREPROCESSOR_STITCH_HELPER(A, B)

#endif