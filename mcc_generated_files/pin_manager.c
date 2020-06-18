#include <xc.h>
#include <stdio.h>
#include "pin_manager.h"
#include "../util.h"

void PIN_MANAGER_Initialize(void) {
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
    IO_RB4_SetDirection(PIN_DIRECTION_INPUT);
    IO_RB7_SetDirection(PIN_DIRECTION_OUTPUT); // UART1 Tx
    IO_RB8_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RB9_SetDirection(PIN_DIRECTION_INPUT);
    IO_RB12_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RB13_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RB14_SetDirection(PIN_DIRECTION_INPUT);
    IO_RB15_SetDirection(PIN_DIRECTION_INPUT);

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

