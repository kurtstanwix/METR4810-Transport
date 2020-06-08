#include <stdio.h>
#include "tmr5.h"

void (*TMR5_InterruptHandler)(void) = NULL;

void TMR5_Initialize (void) {
    //TMR5 0; 
    TMR5 = 0x00;
    //Period = 0.0001 s; Frequency = 16000000 Hz; PR5 199; 
    PR5 = 0xC7;
    //TCKPS 1:8; TON enabled; TSIDL disabled; TCS FOSC/2; TGATE disabled; 
    T5CON = 0x0010;

    IFS1bits.T5IF = false;
    IEC1bits.T5IE = true;
}

void __attribute__((interrupt, no_auto_psv)) _T5Interrupt(){
    TMR5_InterruptHandler();
    IFS1bits.T5IF = false;
}

void TMR5_Period16BitSet(uint16_t value) {
    PR5 = value;
}

uint16_t TMR5_Period16BitGet(void) {
    return PR5;
}

void TMR5_Counter16BitSet(uint16_t value) {
    TMR5 = value;
}

void TMR5_SetInterruptHandler(void (*InterruptHandler)(void)) { 
    IEC1bits.T5IE = false;
    TMR5_InterruptHandler = InterruptHandler; 
    IEC1bits.T5IE = true;
}

void TMR5_Start(void) {
    IEC1bits.T5IE = true;
    T5CONbits.TON = 1;
}

void TMR5_Stop(void) {
    T5CONbits.TON = false;
    IEC1bits.T5IE = false;
}