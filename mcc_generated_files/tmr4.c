#include <stdio.h>
#include "tmr4.h"

void (*TMR4_InterruptHandler)(void) = NULL;

void TMR4_Initialize(void) {
    //TMR4 0; 
    TMR4 = 0x00;
    //Period = 0.000005 s; Frequency = 16000000 Hz; PR4 9; 
    PR4 = 0x09;
    //TCKPS 1:8; T32 16 Bit; TON enabled; TSIDL disabled; TCS FOSC/2; TGATE disabled; 
    T4CON = 0x0010;

    IFS1bits.T4IF = false;
    IEC1bits.T4IE = true;
}

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt() {
    TMR4_InterruptHandler();

    IFS1bits.T4IF = false;
}

void TMR4_Period16BitSet(uint16_t value) {
    PR4 = value;
}

uint16_t TMR4_Period16BitGet(void) {
    return PR4;
}

void TMR4_Counter16BitSet(uint16_t value) {
    TMR4 = value;
}

void TMR4_SetInterruptHandler(void (*InterruptHandler)(void)) {
    IEC1bits.T4IE = false;
    TMR4_InterruptHandler = InterruptHandler;
    IEC1bits.T4IE = true;
}

void TMR4_Start(void) {
    IEC1bits.T4IE = true;
    T4CONbits.TON = 1;
}

void TMR4_Stop(void) {
    T4CONbits.TON = false;
    IEC1bits.T4IE = false;
}