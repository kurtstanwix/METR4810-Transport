#include <stdio.h>
#include <stdbool.h>
#include "one_timer_PWM.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr4.h"
#include "util.h"

#define One_Timer_PWM_SetPin(STATE) IO_RB13_SetPin(STATE)

#define STATE_HIGH 1
#define STATE_LOW 0

void __attribute__ ((weak)) One_Timer_PWM_Handler(void);

bool enabled = false;
uint8_t state = STATE_HIGH;

uint16_t period_us = 100;
uint8_t dutyPercent = 50;

uint16_t periodVal = 0;
uint16_t lowVal = 0;
uint16_t highVal = 0;

void One_Timer_PWM_Initialise (void) {
    TMR_FUNCTION(ONE_TIMER_PWM_TMR, SetInterruptHandler(&One_Timer_PWM_Handler));
    TMR_FUNCTION(ONE_TIMER_PWM_TMR, Initialize());
    One_Timer_PWM_Period_Set(period_us);
}

void One_Timer_PWM_Enable(void) {
    if (!enabled) {
        state = STATE_HIGH;
        One_Timer_PWM_SetPin(PIN_STATE_ON);
        TMR_FUNCTION(ONE_TIMER_PWM_TMR, Start());
        enabled = true;
    }
}

void One_Timer_PWM_Disable(void) {
    if (enabled) {
        One_Timer_PWM_SetPin(PIN_STATE_OFF);
        TMR_FUNCTION(ONE_TIMER_PWM_TMR, Stop());
        TMR_FUNCTION(ONE_TIMER_PWM_TMR, Counter16BitSet(0));
        enabled = false;
    }
}

void One_Timer_PWM_Period_Set(uint16_t us) {
    period_us = us;
    periodVal = us * (TMR_FUNCTION(ONE_TIMER_PWM_TMR, FrequencyGet()) / 1000000) - 1;
    One_Timer_PWM_Duty_Cycle_Set(dutyPercent);
}

void One_Timer_PWM_Duty_Cycle_Set(uint8_t percent) {
    dutyPercent = percent;
    highVal = (uint16_t)((uint32_t)period_us * percent / 100) * \
            (TMR_FUNCTION(ONE_TIMER_PWM_TMR, FrequencyGet()) / 1000000) - 1;
    lowVal = periodVal - highVal;
}

void __attribute__ ((weak)) One_Timer_PWM_Handler(void) {
    if (state == STATE_HIGH) {
        One_Timer_PWM_SetPin(PIN_STATE_OFF);
        state = STATE_LOW;
        TMR_FUNCTION(ONE_TIMER_PWM_TMR, Period16BitSet(lowVal));
    } else {
        One_Timer_PWM_SetPin(PIN_STATE_ON);
        state = STATE_HIGH;
        TMR_FUNCTION(ONE_TIMER_PWM_TMR, Period16BitSet(highVal));
    }
}