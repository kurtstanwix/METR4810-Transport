
/**
  TMR1 Generated Driver API Source File 

  @Company
    Microchip Technology Inc.

  @File Name
    tmr1.c

  @Summary
    This is the generated source file for the TMR1 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for TMR1. 
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
*

/**
  Section: Included Files
*

#include <stdio.h>
#include "tmr1.h"
#include "clock.h"

/**
 Section: File specific functions
*
void (*TMR1_InterruptHandler)(void) = NULL;
void TMR1_CallBack(void);

#define MS_DELAY_COUNT TMR1
#define MS_DELAY_PERIOD PR1
#define MS_DELAY_CONFIG 

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
/*
typedef struct _TMR_OBJ_STRUCT
{
    /* Timer Elapsed *
    volatile bool           timerElapsed;
    /*Software Counter value*
    volatile uint8_t        count;

} TMR_OBJ;

static TMR_OBJ tmr1_obj;

//typedef struct _TMR_DELAY_OBJ_STRUCT {
//    volatile bool enabled;
//    volatile uint16_t elapsed;
//} MS_DELAY_OBJ;

//MS_DELAY_OBJ ms_delay_obj;
*

typedef uint16_t ms_delay_t;

volatile ms_delay_t delayMS;

/**
  Section: Driver Interface
*

uint16_t prescaler = 8;

void TMR1_Initialize (void)
{
    //TMR1 0; 
    TMR1 = 0x00;
    //Period = 0.001 s; Frequency = 4000000 Hz; PR1 499; 
    PR1 = 0x1F3;
    //TCKPS 1:8; TON enabled; TSIDL disabled; TCS FOSC/2; TECS SOSC; TSYNC disabled; TGATE disabled; 
    T1CON = 0x8000;
    _TCKPS1 = 0;
    _TCKPS0 = 1;

    if(TMR1_InterruptHandler == NULL) {
        TMR1_SetInterruptHandler(&TMR1_CallBack);
    }
    delayMS = 0;
    //ms_delay_obj.enabled = false;
    
	T1CONbits.TON = false;
    IFS0bits.T1IF = false;
    IEC0bits.T1IE = true;
}

float TMR1_PeriodGetNS(void) {
    return (PR1 + 1) * prescaler * (1000000000 / CLOCK_PeripheralFrequencyGet());
}

float TMR1_TickFrequencyGet(void) {
    return CLOCK_PeripheralFrequencyGet() / prescaler;
}

void TMR1_Delay_ms(uint16_t ms) {
    T1CONbits.TON = true; // Enable timer
    while (delayMS < ms); // Wait desired milliseconds
    T1CONbits.TON = false; // Disable timer
    delayMS = 0; // Reset delay;
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _T1Interrupt (  )
{
    if(TMR1_InterruptHandler) 
    { 
           TMR1_InterruptHandler(); 
    }
    
    IFS0bits.T1IF = false; // Clear interrupt flag
}
/*
void TMR1_Period16BitSet( uint16_t value )
{
    /* Update the counter values *
    PR1 = value;
    /* Reset the status information *
    tmr1_obj.timerElapsed = false;
}

uint16_t TMR1_Period16BitGet( void )
{
    return( PR1 );
}

void TMR1_Counter16BitSet ( uint16_t value )
{
    /* Update the counter values *
    TMR1 = value;
    /* Reset the status information *
    tmr1_obj.timerElapsed = false;
}

uint16_t TMR1_Counter16BitGet( void )
{
    return( TMR1 );
}
*

void __attribute__ ((weak)) TMR1_CallBack(void) {
    delayMS++;
}

void  TMR1_SetInterruptHandler(void (* InterruptHandler)(void))
{ 
    IEC0bits.T1IE = false;
    TMR1_InterruptHandler = InterruptHandler; 
    IEC0bits.T1IE = true;
}
/*
void TMR1_Start( void )
{
    /* Reset the status information *
    tmr1_obj.timerElapsed = false;

    /*Enable the interrupt*
    IEC0bits.T1IE = true;

    /* Start the Timer *
    T1CONbits.TON = 1;
}

void TMR1_Stop( void )
{
    /* Stop the Timer *
    T1CONbits.TON = false;

    /*Disable the interrupt*
    IEC0bits.T1IE = false;
}

bool TMR1_GetElapsedThenClear(void)
{
    bool status;
    
    status = tmr1_obj.timerElapsed;

    if(status == true)
    {
        tmr1_obj.timerElapsed = false;
    }
    return status;
}

int TMR1_SoftwareCounterGet(void)
{
    return tmr1_obj.count;
}

void TMR1_SoftwareCounterClear(void)
{
    tmr1_obj.count = 0; 
}
*/
/**
 End of File
*/
