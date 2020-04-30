/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef _SOFTWARE_PWM_H
#define	_SOFTWARE_PWM_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define HARDWARE_PWM1_OC_NUM 2
#define HARDWARE_PWM2_OC_NUM 3

#define HARDWARE_PWM_TMR 3

#define HARDWARE_PWM_INITIALISE()
void Hardware_PWM_Initialise(void);

void Hardware_PWM_Period_Set_us(uint8_t pwmNum, uint32_t us);

void Hardware_PWM_Pulse_Width_Set_us(uint8_t pwmNum, uint32_t us);

void Hardware_PWM_Duty_Cycle_Set(uint8_t pwmNum, uint16_t percent);

void Hardware_PWM_Start(uint8_t pwmNum);

void Hardware_PWM_Stop(uint8_t pwmNum);

void Hardware_PWM_Enable(void);

void Hardware_PWM_Disable(void);

#endif	// _SOFTWARE_PWM_H

