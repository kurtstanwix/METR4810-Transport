#ifndef _SOFTWARE_PWM_H
#define _SOFTWARE_PWM_H

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#define SOFTWARE_PWM_TMR 2

void Software_PWM_Initialize (void);

void Software_PWM_Enable(uint8_t pwmIndex);
void Software_PWM_Disable(uint8_t pwmIndex);

void Software_PWM_Reset(uint8_t pwmIndex);
void Software_PWM_Period_Set(uint8_t pwmIndex, uint32_t period);
void Software_PWM_Duty_Cycle_Set(uint8_t pwmIndex, uint16_t dutyCycle);

#endif // SOFTWARE_PWM_H