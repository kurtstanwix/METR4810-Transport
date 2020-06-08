#ifndef _ONE_TIMER_PWM_H
#define _ONE_TIMER_PWM_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define ONE_TIMER_PWM_TMR 4

void One_Timer_PWM_Initialise(void);

void One_Timer_PWM_Enable(void);
void One_Timer_PWM_Disable(void);

void One_Timer_PWM_Period_Set(uint16_t period);
void One_Timer_PWM_Duty_Cycle_Set(uint8_t percent);

#endif //_TMR2_H