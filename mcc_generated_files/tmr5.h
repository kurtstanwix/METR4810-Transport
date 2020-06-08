#ifndef _TMR5_H
#define _TMR5_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "clock.h"

#define TMR5_INTERRUPT_TICKER_FACTOR    1

/*
 * Valid values are:
 * 256, 64, 8, 1
 * Do not set otherwise
 */
#define TMR5_PRESCALER 8

#define TMR5_FrequencyGet() (CLOCK_PeripheralFrequencyGet() / TMR5_PRESCALER)

void TMR5_Initialize (void);

void TMR5_Period16BitSet( uint16_t value );

uint16_t TMR5_Period16BitGet( void );

void TMR5_Counter16BitSet ( uint16_t value );

uint16_t TMR5_Counter16BitGet( void );

void TMR5_SetInterruptHandler(void (* InterruptHandler)(void));

void TMR5_Start( void );

void TMR5_Stop( void );

#endif //_TMR5_H