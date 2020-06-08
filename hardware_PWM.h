#ifndef _HARDWARE_PWM_H
#define	_HARDWARE_PWM_H

#include <xc.h>

#define HARDWARE_PWM1_OC_NUM 2
#define HARDWARE_PWM2_OC_NUM 3

#define HARDWARE_PWM_TMR 3

void Hardware_PWM_Initialise(void);

void Hardware_PWM_Period_Set_us(uint8_t pwmNum, uint32_t us);

void Hardware_PWM_Pulse_Width_Set_us(uint8_t pwmNum, uint32_t us);

void Hardware_PWM_Duty_Cycle_Set(uint8_t pwmNum, uint16_t percent);

void Hardware_PWM_Start(uint8_t pwmNum);

void Hardware_PWM_Stop(uint8_t pwmNum);

void Hardware_PWM_Enable(void);

void Hardware_PWM_Disable(void);

#endif	// _HARDWARE_PWM_H

