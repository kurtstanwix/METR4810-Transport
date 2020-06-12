#ifndef UTIL_H
#define	UTIL_H

#include <stdint.h>
#include <stdbool.h>

//#ifdef __USER_CONTROL
#define __USER_DEBUG
//#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void init_debug(void);
void print_debug(char *toPrint, uint8_t length, bool newLine);
bool compare_strings(uint8_t *toCheck, char *command, uint8_t commandLength);

#define PREPROCESSOR_STITCH_HELPER(A, B) A ## B
#define PREPROCESSOR_STITCH(A, B) PREPROCESSOR_STITCH_HELPER(A, B)

#endif