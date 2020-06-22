#ifndef _HARDWARE_PWM_H
#define	_HARDWARE_PWM_H

#include <xc.h>
#include "tmr.h"

#define HARDWARE_PWM1_OC_NUM 3 // Using OC3 as PWM1 (Motor pin 1)
#define HARDWARE_PWM2_OC_NUM 2 // Using OC2 as PWM2 (Motor pin 2)

#define HARDWARE_PWM_TMR 3 // Using TMR3 for the PWM's

#define HARDWARE_PWM_TxIP TMR_TxIP(HARDWARE_PWM_TMR) // Sets the interrupt priority

void Hardware_PWM_Initialise(void);

void Hardware_PWM_Period_Set_us(uint8_t pwmNum, uint32_t us);

void Hardware_PWM_Pulse_Width_Set_us(uint8_t pwmNum, uint32_t us);

void Hardware_PWM_Duty_Cycle_Set(uint8_t pwmNum, uint16_t percent);

void Hardware_PWM_Start(uint8_t pwmNum);

void Hardware_PWM_Stop(uint8_t pwmNum);

void Hardware_PWM_Enable(void);

void Hardware_PWM_Disable(void);

#endif	// _HARDWARE_PWM_H

