#include <stdio.h>
#include <stdbool.h>
#include "two_timers_PWM.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr4.h"
#include "mcc_generated_files/tmr5.h"
#include "util.h"

#define Two_Timers_PWM_SetPin(STATE) IO_RB13_SetPin(STATE)

void __attribute__ ((weak)) Two_Timers_PWM_Period_Handler(void);
void __attribute__ ((weak)) Two_Timers_PWM_Duty_Handler(void);

bool enabled = false;
uint16_t period = 100;

void Two_Timers_PWM_Initialise (void) {
    TMR_FUNCTION(TWO_TIMERS_PWM_PERIOD_TMR, SetInterruptHandler(&Two_Timers_PWM_Period_Handler));
    TMR_FUNCTION(TWO_TIMERS_PWM_PERIOD_TMR, Initialize());
    Two_Timers_PWM_Period_Set(period);
    TMR_FUNCTION(TWO_TIMERS_PWM_DUTY_TMR, SetInterruptHandler(&Two_Timers_PWM_Duty_Handler));
    TMR_FUNCTION(TWO_TIMERS_PWM_DUTY_TMR, Initialize());
    //Two_Timers_PWM_Duty_Cycle_Set(50);
    TMR_FUNCTION(TWO_TIMERS_PWM_DUTY_TMR, Period16BitSet(100));
}

void Two_Timers_PWM_Enable(void) {
    if (!enabled) {
        Two_Timers_PWM_SetPin(PIN_STATE_ON);
        TMR_FUNCTION(TWO_TIMERS_PWM_DUTY_TMR, Start());
        TMR_FUNCTION(TWO_TIMERS_PWM_PERIOD_TMR, Start());
        enabled = true;
    }
}

void Two_Timers_PWM_Disable(void) {
    if (enabled) {
        Two_Timers_PWM_SetPin(PIN_STATE_OFF);
        TMR_FUNCTION(TWO_TIMERS_PWM_DUTY_TMR, Stop());
        TMR_FUNCTION(TWO_TIMERS_PWM_PERIOD_TMR, Stop());
        TMR_FUNCTION(TWO_TIMERS_PWM_DUTY_TMR, Counter16BitSet(0));
        TMR_FUNCTION(TWO_TIMERS_PWM_PERIOD_TMR, Counter16BitSet(0));
        enabled = false;
    }
}

void Two_Timers_PWM_Period_Set(uint16_t us) {
    period = us;
    TMR_FUNCTION(TWO_TIMERS_PWM_PERIOD_TMR, Period16BitSet(us * (TMR_FUNCTION(TWO_TIMERS_PWM_PERIOD_TMR, FrequencyGet()) / 1000000) - 1));
}

void Two_Timers_PWM_Duty_Cycle_Set(uint8_t percent) {
    TMR_FUNCTION(TWO_TIMERS_PWM_DUTY_TMR, Period16BitSet( \
            (uint16_t)((uint32_t)period * percent / 100) * \
            (TMR_FUNCTION(TWO_TIMERS_PWM_PERIOD_TMR, FrequencyGet()) / 1000000) - 1));
}

void __attribute__ ((weak)) Two_Timers_PWM_Period_Handler(void) {
    TMR_FUNCTION(TWO_TIMERS_PWM_DUTY_TMR, Start());
    Two_Timers_PWM_SetPin(PIN_STATE_ON);
}

void __attribute__ ((weak)) Two_Timers_PWM_Duty_Handler(void) {
    Two_Timers_PWM_SetPin(PIN_STATE_OFF);
    TMR_FUNCTION(TWO_TIMERS_PWM_DUTY_TMR, Stop());
}