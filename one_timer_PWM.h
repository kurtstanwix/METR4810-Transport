/**
  TMR2 Generated Driver API Header File 

  @Company
    Microchip Technology Inc.

  @File Name
    tmr2.h

  @Summary
    This is the generated header file for the TMR2 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides APIs for driver for TMR2. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.166.1
        Device            :  PIC24FV16KA301
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.41
        MPLAB 	          :  MPLAB X v5.30
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#ifndef _ONE_TIMER_PWM_H
#define _ONE_TIMER_PWM_H

/**
  Section: Included Files
*/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>



#ifndef PWM_TMR_WRAPPER
#define PWM_TMR_WRAPPER(NUM, FUNC) TMR ## NUM ## _ ## FUNC
#endif
#ifndef PWM_TMR_WRAPPER2
#define PWM_TMR_WRAPPER2(NUM, FUNC) PWM_TMR_WRAPPER(NUM, FUNC)
#endif

#define ONE_TIMER_PWM_TMR 4
#define One_Timer_PWM_TMR_Function(FUNCTION) PWM_TMR_WRAPPER2(ONE_TIMER_PWM_TMR, FUNCTION)


/**
  Section: Interface Routines
*/
void One_Timer_PWM_Enable(void);
void One_Timer_PWM_Disable(void);

void One_Timer_PWM_Period_Set(uint16_t period);
void One_Timer_PWM_Duty_Cycle_Set(uint8_t percent);

void One_Timer_PWM_Initialise(void);


#endif //_TMR2_H
    
/**
 End of File
*/
