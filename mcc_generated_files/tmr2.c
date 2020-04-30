
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
#include "tmr2.h"

/**
 Section: File specific functions
*/
void (*TMR2_InterruptHandler)(void) = NULL;
void TMR2_CallBack(void);

/**
  Section: Data Type Definitions
*/

/** TMR Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

  @Description
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

/**
  Section: Driver Interface
*/

void TMR2_Initialize (void)
{
    // Inititialise timer count to 0
    TMR2 = 0x00;
    //Period = 0.001 s; Frequency = 4000000 Hz; PR2 3999; 
    //PR2 = 0xF9F; // 1ms
    //PR2 = 0x18F; // 100us 4MHz
    PR2 = 0x63F; // 100us 16MHz
    //PR2 = 0x27; // 10us 4MHz
    //PR2 = 0x9F; // 10us 16MHz
    //TCKPS 1:1; T32 16 Bit; TON enabled; TSIDL disabled; TCS FOSC/2; TGATE disabled; 
    T2CON = 0x0000;
    
    //_TCKPS1 = 0;
    //_TCKPS0 = 0;

    if (TMR2_InterruptHandler == NULL) {
        TMR2_SetInterruptHandler(&TMR2_CallBack);
    }

    IFS0bits.T2IF = false;
    IEC0bits.T2IE = true;
	

}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _T2Interrupt (  )
{
    /* Check if the Timer Interrupt/Status is set */

    //***User Area Begin

    // ticker function call;
    // ticker is 1 -> Callback function gets called everytime this ISR executes
    if (TMR2_InterruptHandler) { 
           TMR2_InterruptHandler(); 
    }

    //***User Area End
    
    IFS0bits.T2IF = false;
}

void TMR2_Period16BitSet(uint16_t value) {
    /* Update the counter values */
    PR2 = value;
}

uint16_t TMR2_Period16BitGet(void) {
    return(PR2);
}

void TMR2_Counter16BitSet (uint16_t value) {
    /* Update the counter values */
    TMR2 = value;
}

uint16_t TMR2_Counter16BitGet(void) {
    return(TMR2);
}


void __attribute__ ((weak)) TMR2_CallBack(void) {
    // Add your custom callback code here
}

void  TMR2_SetInterruptHandler(void (*InterruptHandler)(void)) { 
    IEC0bits.T2IE = false;
    TMR2_InterruptHandler = InterruptHandler; 
    IEC0bits.T2IE = true;
}

void TMR2_Start(void) {
    /*Enable the interrupt*/
    IEC0bits.T2IE = true;

    /* Start the Timer */
    T2CONbits.TON = 1;
}

void TMR2_Stop( void ) {
    /* Stop the Timer */
    T2CONbits.TON = false;

    /*Disable the interrupt*/
    IEC0bits.T2IE = false;
}

/**
 End of File
*/
