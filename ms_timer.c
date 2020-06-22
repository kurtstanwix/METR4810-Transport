#include <stdio.h>
#include "mcc_generated_files/clock.h"
#include "ms_timer.h"
#include "tmr.h"
#include "util.h"

// Stores the current time
volatile ms_time_t currentTime;
// Time when a delay call ends
volatile ms_time_t delayEndTime;

// Register definitions for ability to switch timer modules
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

/**
 * Initialises the TMR module for the millisecond timer
 */
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
    MS_TIMER_TxCONbits.TON = 1;

    delayEndTime = 0;

    MS_TIMER_TxIF = false;
    MS_TIMER_TxIE = true;
}

/**
 * Waits the specified amount of time in milliseconds before returning
 * @param ms time in milliseconds to wait
 */
void MS_TIMER_Delay_ms(uint16_t ms) {
    delayEndTime = currentTime + ms;
    while (currentTime < delayEndTime); // Wait desired milliseconds
}

/**
 * Get the current time since system start in milliseconds
 * @return current time since system start in milliseconds
 */
ms_time_t MS_TIMER_get_time_ms(void) {
    return currentTime;
}

/**
 * Get the timer module's current count register
 * @return TMR register value of the timer module
 */
uint16_t MS_TIMER_get_TMR(void) {
    return MS_TIMER_TMR_REGISTER;
}

/**
 * Get the current time since system start in microseconds. as the timer is configured
 * for accurate millisecond counts, this may be inaccurate by up to 1000 microseconds
 * @return current time since system start in microseconds
 */
us_time_t MS_TIMER_get_time_us(void) {
    return currentTime * 1000 + __builtin_divsd(((uint32_t) MS_TIMER_TMR_REGISTER * 1000), 1999);
}

/**
 * The interrupt used to update the timer value
 */
void __attribute__((interrupt, no_auto_psv)) MS_TIMER_TxInterrupt() {
    currentTime++;
    MS_TIMER_TxIF = 0; // Clear interrupt flag
}