#ifndef UTIL_H
#define	UTIL_H

#include <stdint.h>
#include <stdbool.h>

//#define __USER_DEBUG

void init_debug(void);
void print_debug(char *toPrint, uint8_t length);
bool compare_strings(uint8_t *toCheck, char *command, uint8_t commandLength);

#endif

