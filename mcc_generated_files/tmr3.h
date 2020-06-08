#ifndef _TMR3_H
#define _TMR3_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "clock.h"

/*
 * Valid values are:
 * 256, 64, 8, 1
 * Do not set otherwise
 */
#define TMR3_PRESCALER 8

#define TMR3_FrequencyGet() (float)(CLOCK_PeripheralFrequencyGet() / TMR3_PRESCALER)

void TMR3_Initialize(void);

void TMR3_Period16BitSet(uint16_t value);

uint16_t TMR3_Period16BitGet(void);

void TMR3_Counter16BitSet(uint16_t value);

uint16_t TMR3_Counter16BitGet(void);

void TMR3_Start(void);

void TMR3_Stop(void);

#endif //_TMR3_H