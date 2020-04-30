/**
  PIN MANAGER Generated Driver File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the generated manager file for the PIC24 / dsPIC33 / PIC32MM MCUs device.  This manager
    configures the pins direction, initial state, analog setting.

  @Description:
    This source file provides implementations for PIN MANAGER.
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

#ifndef _PIN_MANAGER_H
#define _PIN_MANAGER_H
/**
    Section: Includes
*/
#include <xc.h>

/**
    Section: Device Pin Macros
*/

#define PIN_STATE_OFF 0
#define PIN_STATE_ON 1

#define PIN_DIRECTION_OUTPUT 0
#define PIN_DIRECTION_INPUT 1

// Do not use as IO, RA0 is the ICSP PGC
#define IO_RA0_SetPin(STATE)          (_LATA0 = STATE)
#define IO_RA0_Toggle()           (_LATA0 ^= 1)
#define IO_RA0_GetValue()         _RA0
#define IO_RA0_SetDirection(DIR)  (_TRISA0 = DIR)

// Do not use as IO, RA1 is the ICSP PGD
#define IO_RA1_SetPin(STATE)          (_LATA1 = STATE)
#define IO_RA1_Toggle()           (_LATA1 ^= 1)
#define IO_RA1_GetValue()         _RA1
#define IO_RA1_SetDirection(DIR)  (_TRISA1 = DIR)

#define IO_RA2_SetPin(STATE)          (_LATA2 = STATE)
#define IO_RA2_Toggle()           (_LATA2 ^= 1)
#define IO_RA2_GetValue()         _RA2
#define IO_RA2_SetDirection(DIR)  (_TRISA2 = DIR)

#define IO_RA3_SetPin(STATE)          (_LATA3 = STATE)
#define IO_RA3_Toggle()           (_LATA3 ^= 1)
#define IO_RA3_GetValue()         _RA3
#define IO_RA3_SetDirection(DIR)  (_TRISA3 = DIR)

#define IO_RA4_SetPin(STATE)          (_LATA4 = STATE)
#define IO_RA4_Toggle()           (_LATA4 ^= 1)
#define IO_RA4_GetValue()         _RA4
#define IO_RA4_SetDirection(DIR)  (_TRISA4 = DIR)


// Do not use as IO, RB0 is UART2 Tx
#define IO_RB0_SetPin(STATE)          (_LATB0 = STATE)
#define IO_RB0_Toggle()           (_LATB0 ^= 1)
#define IO_RB0_GetValue()         _RB0
#define IO_RB0_SetDirection(DIR)  (_TRISB0 = DIR)

// Do not use as IO, RB1 is UART2 Rx
#define IO_RB1_SetPin(STATE)          (_LATB1 = STATE)
#define IO_RB1_Toggle()           (_LATB1 ^= 1)
#define IO_RB1_GetValue()         _RB1
#define IO_RB1_SetDirection(DIR)  (_TRISB1 = DIR)

// Do not use as IO, RB2 is UART1 Rx
#define IO_RB2_SetPin(STATE)          (_LATB2 = STATE)
#define IO_RB2_Toggle()           (_LATB2 ^= 1)
#define IO_RB2_GetValue()         _RB2
#define IO_RB2_SetDirection(DIR)  (_TRISB2 = DIR)

#define IO_RB4_SetPin(STATE)          (_LATB4 = STATE)
#define IO_RB4_Toggle()           (_LATB4 ^= 1)
#define IO_RB4_GetValue()         _RB4
#define IO_RB4_SetDirection(DIR)  (_TRISB4 = DIR)

// Do not use as IO, RB7 is UART1 Tx
#define IO_RB7_SetPin(STATE)          (_LATB7 = STATE)
#define IO_RB7_Toggle()           (_LATB7 ^= 1)
#define IO_RB7_GetValue()         _RB7
#define IO_RB7_SetDirection(DIR)  (_TRISB7 = DIR)

#define IO_RB8_SetPin(STATE)          (_LATB8 = STATE)
#define IO_RB8_Toggle()           (_LATB8 ^= 1)
#define IO_RB8_GetValue()         _RB8
#define IO_RB8_SetDirection(DIR)  (_TRISB8 = DIR)

#define IO_RB9_SetPin(STATE)          (_LATB9 = STATE)
#define IO_RB9_Toggle()           (_LATB9 ^= 1)
#define IO_RB9_GetValue()         _RB9
#define IO_RB9_SetDirection(DIR)  (_TRISB9 = DIR)

#define IO_RB12_SetPin(STATE)          (_LATB12 = STATE)
#define IO_RB12_Toggle()           (_LATB12 ^= 1)
#define IO_RB12_GetValue()         _RB12
#define IO_RB12_SetDirection(DIR)  (_TRISB12 = DIR)

#define IO_RB13_SetPin(STATE)          (_LATB13 = STATE)
#define IO_RB13_Toggle()           (_LATB13 ^= 1)
#define IO_RB13_GetValue()         _RB13
#define IO_RB13_SetDirection(DIR)  (_TRISB13 = DIR)

#define IO_RB14_SetPin(STATE)          (_LATB14 = STATE)
#define IO_RB14_Toggle()           (_LATB14 ^= 1)
#define IO_RB14_GetValue()         _RB14
#define IO_RB14_SetDirection(DIR)  (_TRISB14 = DIR)

#define IO_RB15_SetPin(STATE)          (_LATB15 = STATE)
#define IO_RB15_Toggle()           (_LATB15 ^= 1)
#define IO_RB15_GetValue()         _RB15
#define IO_RB15_SetDirection(DIR)  (_TRISB15 = DIR)


/**
    Section: Function Prototypes
*/
/**
  @Summary
    Configures the pin settings of the PIC24FV16KA301

  @Description
    This is the generated manager file for the PIC24 / dsPIC33 / PIC32MM MCUs device.  This manager
    configures the pins direction, initial state, analog setting.

  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Example
    <code>
    void SYSTEM_Initialize(void)
    {
        // Other initializers are called from this function
        PIN_MANAGER_Initialize();
    }
    </code>

*/
void PIN_MANAGER_Initialize (void);



#endif
