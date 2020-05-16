/**
  PIN MANAGER Generated Driver File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.c

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


/**
    Section: Includes
*/

#include <xc.h>
#include <stdio.h>
#include "pin_manager.h"
#include "../util.h"

/**
 Section: Driver Interface Function Definitions
*/
void PIN_MANAGER_Initialize (void)
{
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0000; // Init PORTA output latches
    LATB = 0x0000; // Init PORTB output latches
#ifdef __USER_DEBUG
    IO_RB0_SetPin(PIN_STATE_ON); // UART2 Tx requires pin start as on
#endif    
    IO_RB7_SetPin(PIN_STATE_ON); // UART1 Tx requires pin start as on

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x0000; // Init PORTA direction register
    IO_RA0_SetDirection(PIN_DIRECTION_INPUT); // ICSP PGC
    IO_RA1_SetDirection(PIN_DIRECTION_INPUT); // ICSP PGD
    IO_RA2_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RA3_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RA4_SetDirection(PIN_DIRECTION_OUTPUT);
    
    TRISB = 0x0000; // Init PORTB direction register
#ifdef __USER_DEBUG
    IO_RB0_SetDirection(PIN_DIRECTION_OUTPUT); // UART2 Tx
    IO_RB1_SetDirection(PIN_DIRECTION_INPUT); // UART2 Rx
#else
    IO_RB0_SetDirection(PIN_DIRECTION_OUTPUT); // OC2 PWM
    IO_RB1_SetDirection(PIN_DIRECTION_OUTPUT); // OC3 PWM
#endif
    IO_RB2_SetDirection(PIN_DIRECTION_INPUT); // UART1 Rx
    IO_RB4_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RB7_SetDirection(PIN_DIRECTION_OUTPUT); // UART1 Tx
    IO_RB8_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RB9_SetDirection(PIN_DIRECTION_INPUT);
    IO_RB12_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RB13_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RB13_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RB15_SetDirection(PIN_DIRECTION_OUTPUT);

    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPD1 = 0x0000;
    CNPD2 = 0x0002;
    CNPD3 = 0x0000;
    CNPU1 = 0x0000;
    CNPU2 = 0x0000;
    CNPU3 = 0x0000;

    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000;
    ODCB = 0x0000;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSA = 0x0000;
    ANSB = 0x0000;
}

