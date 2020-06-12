#include <stdio.h>
#include "mcc_generated_files/clock.h"
#include "ms_timer.h"
#include "tmr.h"
#include "util.h"

volatile ms_time_t currentTime;
volatile ms_time_t delayEndTime;

#define MS_TIMER_TMR_REGISTER TMR_TMR_REGISTER(MS_TIMER_TMR)
#define MS_TIMER_PR_REGISTER TMR_PR_REGISTER(MS_TIMER_TMR)
#define MS_TIMER_TxCON_REGISTER TMR_TxCON_REGISTER(MS_TIMER_TMR)
#define MS_TIMER_TxCONbits TMR_TxCONbits(MS_TIMER_TMR)
#define MS_TIMER_TxIF TMR_TxIF(MS_TIMER_TMR)
#define MS_TIMER_TxIE TMR_TxIE(MS_TIMER_TMR)
#define MS_TIMER_TxInterrupt TMR_TxInterrupt(MS_TIMER_TMR)


/*
 * Valid values are:
 * 256, 64, 8, 1
 * Do not set otherwise
 */
#define MS_TIMER_PRESCALER 8

void MS_TIMER_Initialize(void) {
    //TMR1 0; 
    MS_TIMER_TMR_REGISTER = 0x00;
    //Period = 0.001 s; Frequency = 4000000 Hz; PR1 499; 
    MS_TIMER_PR_REGISTER = 0x7CF;
    //TCKPS 1:8; TON enabled; TSIDL disabled; TCS FOSC/2; TECS SOSC; TSYNC disabled; TGATE disabled; 
    MS_TIMER_TxCON_REGISTER = 0x0000;
    switch (MS_TIMER_PRESCALER) {
        case 256:
            MS_TIMER_TxCONbits.TCKPS = 0b11;
            break;
        case 64:
            MS_TIMER_TxCONbits.TCKPS = 0b10;
            break;
        case 8:
            MS_TIMER_TxCONbits.TCKPS = 0b01;
            break;
        case 1:
            MS_TIMER_TxCONbits.TCKPS = 0b00;
    }
    MS_TIMER_TxCONbits.TON = 1; // Disable for now

    delayEndTime = 0;

    MS_TIMER_TxIF = false;
    MS_TIMER_TxIE = true;
}

/*
float MS_TIMER_PeriodGetNS(void) {
    return (MS_TIMER_PR_REGISTER + 1) * MS_TIMER_PRESCALER * (1000000000 / CLOCK_PeripheralFrequencyGet());
}

float MS_TIMER_TickFrequencyGet(void) {
    return CLOCK_PeripheralFrequencyGet() / MS_TIMER_PRESCALER;
}
 */

void MS_TIMER_Delay_ms(uint16_t ms) {
    delayEndTime = currentTime + ms;
    while (currentTime < delayEndTime); // Wait desired milliseconds
}

ms_time_t MS_TIMER_get_time_ms(void) {
    return currentTime;
}

uint16_t MS_TIMER_get_TMR(void) {
    return MS_TIMER_TMR_REGISTER;
}

us_time_t MS_TIMER_get_time_us(void) {
    return currentTime * 1000 + __builtin_divsd(((uint32_t) MS_TIMER_TMR_REGISTER * 1000), 1999);
}

void __attribute__((interrupt, no_auto_psv)) MS_TIMER_TxInterrupt() {
    currentTime++;
    MS_TIMER_TxIF = 0; // Clear interrupt flag
}