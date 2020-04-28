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

#define IO_RA2_SetPin(STATE)          (_LATA2 = STATE)
#define IO_RA2_Toggle()           (_LATA2 ^= 1)
#define IO_RA2_GetValue()         _RA2
#define IO_RA2_SetDigitalInput()  (_TRISA2 = 1)
#define IO_RA2_SetDigitalOutput() (_TRISA2 = 0)

#define IO_RA3_SetPin(STATE)          (_LATA3 = STATE)
#define IO_RA3_Toggle()           (_LATA3 ^= 1)
#define IO_RA3_GetValue()         _RA3
#define IO_RA3_SetDigitalInput()  (_TRISA3 = 1)
#define IO_RA3_SetDigitalOutput() (_TRISA3 = 0)

#define IO_RA4_SetPin(STATE)          (_LATA4 = STATE)
#define IO_RA4_Toggle()           (_LATA4 ^= 1)
#define IO_RA4_GetValue()         _RA4
#define IO_RA4_SetDigitalInput()  (_TRISA4 = 1)
#define IO_RA4_SetDigitalOutput() (_TRISA4 = 0)


#define IO_RB4_SetPin(STATE)          (_LATB4 = STATE)
#define IO_RB4_Toggle()           (_LATB4 ^= 1)
#define IO_RB4_GetValue()         _RB4
#define IO_RB4_SetDigitalInput()  (_TRISB4 = 1)
#define IO_RB4_SetDigitalOutput() (_TRISB4 = 0)

#define IO_RB8_SetPin(STATE)          (_LATB8 = STATE)
#define IO_RB8_Toggle()           (_LATB8 ^= 1)
#define IO_RB8_GetValue()         _RB8
#define IO_RB8_SetDigitalInput()  (_TRISB8 = 1)
#define IO_RB8_SetDigitalOutput() (_TRISB8 = 0)

#define IO_RB9_SetPin(STATE)          (_LATB9 = STATE)
#define IO_RB9_Toggle()           (_LATB9 ^= 1)
#define IO_RB9_GetValue()         _RB9
#define IO_RB9_SetDigitalInput()  (_TRISB9 = 1)
#define IO_RB9_SetDigitalOutput() (_TRISB9 = 0)

#define IO_RB12_SetPin(STATE)          (_LATB12 = STATE)
#define IO_RB12_Toggle()           (_LATB12 ^= 1)
#define IO_RB12_GetValue()         _RB12
#define IO_RB12_SetDigitalInput()  (_TRISB12 = 1)
#define IO_RB12_SetDigitalOutput() (_TRISB12 = 0)

#define IO_RB13_SetPin(STATE)          (_LATB13 = STATE)
#define IO_RB13_Toggle()           (_LATB13 ^= 1)
#define IO_RB13_GetValue()         _RB13
#define IO_RB13_SetDigitalInput()  (_TRISB13 = 1)
#define IO_RB13_SetDigitalOutput() (_TRISB13 = 0)

#define IO_RB15_SetPin(STATE)          (_LATB15 = STATE)
#define IO_RB15_Toggle()           (_LATB15 ^= 1)
#define IO_RB15_GetValue()         _RB15
#define IO_RB15_SetDigitalInput()  (_TRISB15 = 1)
#define IO_RB15_SetDigitalOutput() (_TRISB15 = 0)


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
