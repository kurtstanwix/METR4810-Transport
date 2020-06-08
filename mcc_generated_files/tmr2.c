#include <stdio.h>
#include "tmr2.h"

void (*TMR2_InterruptHandler)(void) = NULL;
void TMR2_CallBack(void);

void TMR2_Initialize (void)
{
    // Inititialise timer count to 0
    TMR2 = 0x00;
    //Period = 0.001 s; Frequency = 4000000 Hz; PR2 3999; 
    //PR2 = 0xF9F; // 1ms
    //PR2 = 0x18F; // 100us 4MHz
    //PR2 = 0x63F; // 100us 16MHz
    //PR2 = 0x27; // 10us 4MHz
    PR2 = 0x9F; // 10us 16MHz
    //TCKPS 1:1; T32 16 Bit; TON enabled; TSIDL disabled; TCS FOSC/2; TGATE disabled; 
    T2CON = 0x0000;
    
    //_TCKPS1 = 0;
    //_TCKPS0 = 0;

    if (TMR2_InterruptHandler == NULL) {
        TMR2_SetInterruptHandler(&TMR2_CallBack);
    }

    IFS0bits.T2IF = false;
    IEC0bits.T2IE = true;
}

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt () {
    if (TMR2_InterruptHandler) { 
           TMR2_InterruptHandler(); 
    }
    
    IFS0bits.T2IF = false;
}

void TMR2_Period16BitSet(uint16_t value) {
    PR2 = value;
}

uint16_t TMR2_Period16BitGet(void) {
    return PR2;
}

void TMR2_Counter16BitSet (uint16_t value) {
    TMR2 = value;
}

uint16_t TMR2_Counter16BitGet(void) {
    return(TMR2);
}

void __attribute__ ((weak)) TMR2_CallBack(void) {
}

void  TMR2_SetInterruptHandler(void (*InterruptHandler)(void)) { 
    IEC0bits.T2IE = false;
    TMR2_InterruptHandler = InterruptHandler; 
    IEC0bits.T2IE = true;
}

void TMR2_Start(void) {
    /*Enable the interrupt*/
    IEC0bits.T2IE = true;

    /* Start the Timer */
    T2CONbits.TON = 1;
}

void TMR2_Stop( void ) {
    /* Stop the Timer */
    T2CONbits.TON = false;

    /*Disable the interrupt*/
    IEC0bits.T2IE = false;
}