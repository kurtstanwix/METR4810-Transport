#ifndef _TMR2_H
#define _TMR2_H

#include <xc.h>
#include <stdint.h>

#define TMR2_INTERRUPT_TICKER_FACTOR    1

void TMR2_Initialize (void);

void TMR2_Period16BitSet(uint16_t value);

uint16_t TMR2_Period16BitGet(void);

void TMR2_Counter16BitSet (uint16_t value);

uint16_t TMR2_Counter16BitGet(void);

void TMR2_SetInterruptHandler(void (*InterruptHandler)(void));

void TMR2_Start(void);

void TMR2_Stop(void);

#endif //_TMR2_H