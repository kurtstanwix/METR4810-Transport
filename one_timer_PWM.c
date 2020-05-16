
/**
  TMR2 Generated Driver API Source File 

  @Company
    Microchip Technology Inc.

  @File Name
    tmr2.c

  @Summary
    This is the generated source file for the TMR2 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for TMR2. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.166.1
        Device            :  PIC24FV16KA301
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.41
        MPLAB             :  MPLAB X v5.30
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

/**
  Section: Included Files
*/

#include <stdio.h>
#include <stdbool.h>
#include "one_timer_PWM.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr4.h"

#define One_Timer_PWM_SetPin(STATE) IO_RB13_SetPin(STATE)

#define STATE_HIGH 1
#define STATE_LOW 0
/**
  Section: Data Type Definitions
*/

/**
 Section: File specific functions
*/
void __attribute__ ((weak)) One_Timer_PWM_Handler(void);

bool enabled1 = false;
uint8_t state = STATE_HIGH;

uint16_t period_us = 100;
uint8_t dutyPercent = 50;

uint16_t periodVal = 0;
uint16_t lowVal = 0;
uint16_t highVal = 0;

/**
  Section: Driver Interface
*/

void One_Timer_PWM_Initialise (void) {
    //pwms[0] = Software_PWM_Create(10000, 500, &Software_PWM0_SetPin);
    //pwms[1] = Software_PWM_Create(20000, 500, &Software_PWM1_SetPin);
    //pwms[2] = Software_PWM_Create(10000, 500, &Software_PWM2_SetPin);
    //pwms[3] = Software_PWM_Create(20000, 500, &Software_PWM3_SetPin);
    //pwms[4] = Software_PWM_Create(10000, 500, &Software_PWM4_SetPin);
    One_Timer_PWM_TMR_Function(SetInterruptHandler(&One_Timer_PWM_Handler));
    One_Timer_PWM_TMR_Function(Initialize());
    One_Timer_PWM_Period_Set(period_us);
    //Two_Timers_PWM_Duty_Cycle_Set(50);
    //One_Timer_PWM_TMR_Function(Period16BitSet(100));
    //Software_PWM_Function(Start());
}


void One_Timer_PWM_Enable(void) {
    if (!enabled1) {
        state = STATE_HIGH;
        One_Timer_PWM_SetPin(PIN_STATE_ON);
        One_Timer_PWM_TMR_Function(Start());
        enabled1 = true;
    }
}

void One_Timer_PWM_Disable(void) {
    if (enabled1) {
        One_Timer_PWM_SetPin(PIN_STATE_OFF);
        One_Timer_PWM_TMR_Function(Stop());
        One_Timer_PWM_TMR_Function(Counter16BitSet(0));
        enabled1 = false;
    }
}

void One_Timer_PWM_Period_Set(uint16_t us) {
    period_us = us;
    periodVal = us * (One_Timer_PWM_TMR_Function(FrequencyGet()) / 1000000) - 1;
    One_Timer_PWM_Duty_Cycle_Set(dutyPercent);
}

void One_Timer_PWM_Duty_Cycle_Set(uint8_t percent) {
    dutyPercent = percent;
    highVal = (uint16_t)((uint32_t)period_us * percent / 100) * \
            (One_Timer_PWM_TMR_Function(FrequencyGet()) / 1000000) - 1;
    lowVal = periodVal - highVal;
}

/*
void TMR2_Period16BitSet( uint16_t value )
{
    /* Update the counter values *
    PR2 = value;
    /* Reset the status information *
    tmr2_obj.timerElapsed = false;
}

uint16_t TMR2_Period16BitGet( void )
{
    return( PR2 );
}

void TMR2_Counter16BitSet ( uint16_t value )
{
    /* Update the counter values *
    TMR2 = value;
    /* Reset the status information *
    tmr2_obj.timerElapsed = false;
}

uint16_t TMR2_Counter16BitGet( void )
{
    return( TMR2 );
}
*/

void __attribute__ ((weak)) One_Timer_PWM_Handler(void) {
    if (state == STATE_HIGH) {
        One_Timer_PWM_SetPin(PIN_STATE_OFF);
        state = STATE_LOW;
        One_Timer_PWM_TMR_Function(Period16BitSet(lowVal));
    } else {
        One_Timer_PWM_SetPin(PIN_STATE_ON);
        state = STATE_HIGH;
        One_Timer_PWM_TMR_Function(Period16BitSet(highVal));
    }
}

/*
bool TMR2_GetElapsedThenClear(void)
{
    bool status;
    
    status = tmr2_obj.timerElapsed;

    if(status == true)
    {
        tmr2_obj.timerElapsed = false;
    }
    return status;
}

int TMR2_SoftwareCounterGet(void)
{
    return tmr2_obj.count;
}

void TMR2_SoftwareCounterClear(void)
{
    tmr2_obj.count = 0; 
}
*/
/**
 End of File
*/