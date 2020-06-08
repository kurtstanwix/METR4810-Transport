#include <stdio.h>
#include "tmr3.h"

void TMR3_Initialize(void) {
    //TMR3 0; 
    TMR3 = 0x00;
    //Period = 0 s; Frequency = 16000000 Hz; PR3 0; 
    PR3 = 0x00;
    //TCKPS 1:8; TON enabled; TSIDL disabled; TCS FOSC/2; TGATE disabled; 
    T3CON = 0x0000;
    // Set the prescaler vased on prescaler variable
    switch (TMR3_PRESCALER) {
        case 256:
            T3CONbits.TCKPS = 0b11;
            break;
        case 64:
            T3CONbits.TCKPS = 0b10;
            break;
        case 8:
            T3CONbits.TCKPS = 0b01;
            break;
        case 1:
            T3CONbits.TCKPS = 0b00;
    }
    T3CONbits.TON = 1;
}

void TMR3_Period16BitSet(uint16_t value) {
    PR3 = value;
}

uint16_t TMR3_Period16BitGet(void) {
    return PR3;
}

void TMR3_Counter16BitSet(uint16_t value) {
    TMR3 = value;
}

uint16_t TMR3_Counter16BitGet(void) {
    return TMR3;
}

void TMR3_Start(void) {
    T3CONbits.TON = 1;
}

void TMR3_Stop(void) {
    T3CONbits.TON = false;
}