#ifndef _TWO_TIMERS_PWM_H
#define _TWO_TIMERS_PWM_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define TWO_TIMERS_PWM_PERIOD_TMR 5
#define TWO_TIMERS_PWM_DUTY_TMR 4

void Two_Timers_PWM_Enable(void);
void Two_Timers_PWM_Disable(void);

void Two_Timers_PWM_Period_Set(uint16_t period);
void Two_Timers_PWM_Duty_Cycle_Set(uint8_t percent);

void Two_Timers_PWM_Initialise(void);


#endif // _TWO_TIMERS_PWM_H