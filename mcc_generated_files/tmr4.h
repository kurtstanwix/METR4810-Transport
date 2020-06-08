#ifndef _TMR4_H
#define _TMR4_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "clock.h"

#define TMR4_INTERRUPT_TICKER_FACTOR    1

/*
 * Valid values are:
 * 256, 64, 8, 1
 * Do not set otherwise
 */
#define TMR4_PRESCALER 8

#define TMR4_FrequencyGet() (CLOCK_PeripheralFrequencyGet() / TMR4_PRESCALER)

void TMR4_Initialize(void);

void TMR4_Period16BitSet(uint16_t value);

uint16_t TMR4_Period16BitGet(void);

void TMR4_Counter16BitSet(uint16_t value);

uint16_t TMR4_Counter16BitGet(void);

void TMR4_SetInterruptHandler(void (* InterruptHandler)(void));

void TMR4_Start(void);

void TMR4_Stop(void);

#endif //_TMR4_H