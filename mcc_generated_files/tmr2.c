
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
#include "pin_manager.h"

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

typedef struct _TMR_OBJ_STRUCT
{
    /* Timer Elapsed */
    volatile bool           timerElapsed;
    /*Software Counter value*/
    volatile uint8_t        count;

} TMR_OBJ;

static TMR_OBJ tmr2_obj;


typedef struct _SOFTWARE_PWM_STRUCT
{
    volatile uint32_t count;
    // PWM period
    volatile uint32_t period;
    // 
    volatile uint32_t highValue;
    // Current output state (0 or 1)
    //void (*Set_Pin)(void);
    //void (*Clear_Pin)(void);
    void (*set_pin)(uint8_t);
    volatile uint8_t enabled;
    volatile uint8_t state;
    volatile uint16_t dutyCycle;

} PWM_OBJ;

/**
 Section: File specific functions
*/
void (*TMR2_InterruptHandler)(void) = NULL;
void TMR2_CallBack(void);

void Software_PWM_Reset_Obj(volatile PWM_OBJ *pwm);
void Software_PWM_Duty_Cycle_Set_Obj(volatile PWM_OBJ *pwm, uint16_t dutyCycle);
void Software_PWM_Period_Set_Obj(volatile PWM_OBJ *pwm, uint32_t period);



#define NUM_PWM 4
volatile PWM_OBJ pwms[NUM_PWM];

// Wrappers for the Pin toggle defs
static void PWM0_SetPin(uint8_t state) {
    IO_RA2_SetPin(state);
}
static void PWM1_SetPin(uint8_t state) {
    IO_RA3_SetPin(state);
}
static void PWM2_SetPin(uint8_t state) {
    IO_RB4_SetPin(state);
}
static void PWM3_SetPin(uint8_t state) {
    IO_RA4_SetPin(state);
}

/*
static void PWM0_Clear(void) {
    IO_RA2_SetLow();
}

static void PWM1_Set(void) {
    IO_RA3_SetHigh();
}

static void PWM1_Clear(void) {
    IO_RA3_SetLow();
}

static void PWM2_Set(void) {
    IO_RB4_SetHigh();
}

static void PWM2_Clear(void) {
    IO_RB4_SetLow();
}

static void PWM3_Set(void) {
    IO_RA4_SetHigh();
}

static void PWM3_Clear(void) {
    IO_RA4_SetLow();
}
/*
static void PWM4_Set(void) {
    IO_RA4_SetHigh();
}

static void PWM4_Clear(void) {
    IO_RA4_SetLow();
}
*/

/*void Software_PWM_Initialise(PWM_OBJ *pwm, uint16_t period, uint8_t dutyCycle, void (*set), void (*clear)) {
    pwm->count = 0;
    pwm->period = period;
    Software_PWM_Duty_Cycle_Set(pwm, dutyCycle);
    pwm->Set_Pin = set;
    pwm->Clear_Pin = clear;
    pwm->Set_Pin();
    pwm->state = 1;
    pwm->enabled = 1;
}*/

/**
 * Creates a software PWM object 
 * @param period
 * @param dutyCycle
 * @param set
 * @param clear
 * @return 
 */
PWM_OBJ Software_PWM_Create(uint16_t period, uint16_t dutyCycle, void (*setPin)(uint8_t)) {
    PWM_OBJ result;
    result.count = 0;
    result.period = period;
    //result.Set_Pin = set;
    //result.Clear_Pin = clear;
    result.set_pin = setPin;
    Software_PWM_Duty_Cycle_Set_Obj(&result, dutyCycle);
    //result.Clear_Pin();
    result.state = 0;
    result.set_pin(PIN_STATE_OFF);
    result.enabled = 0;
    return result;
}

void Software_PWM_Enable(uint8_t pwmIndex) {
    if (pwmIndex < NUM_PWM && !pwms[pwmIndex].enabled) {
        pwms[pwmIndex].state = PIN_STATE_ON;
        pwms[pwmIndex].set_pin(PIN_STATE_ON);
        pwms[pwmIndex].enabled = 1;
        pwms[pwmIndex].count = 0;
    }
}

void Software_PWM_Disable(uint8_t pwmIndex) {
    if (pwmIndex < NUM_PWM && pwms[pwmIndex].enabled) {
        pwms[pwmIndex].state = PIN_STATE_OFF;
        pwms[pwmIndex].enabled = 0;
        pwms[pwmIndex].set_pin(PIN_STATE_OFF);
        pwms[pwmIndex].count = 0;
    }
}

void Software_PWM_Reset_Obj(volatile PWM_OBJ *pwm) {
    pwm->count = 0;
    pwm->state = PIN_STATE_ON;
    pwm->set_pin(PIN_STATE_ON);
}

// Duty cycle in 1/10th of 1%: 1000 = 100%
void Software_PWM_Duty_Cycle_Set_Obj(volatile PWM_OBJ *pwm, uint16_t dutyCycle) {
    pwm->highValue = (dutyCycle / 1000.0) * pwm->period;
    pwm->dutyCycle = dutyCycle;
    if (pwm->enabled) {
        Software_PWM_Reset_Obj(pwm);
    }
}
void Software_PWM_Period_Set_Obj(volatile PWM_OBJ *pwm, uint32_t period) {
    pwm->period = period;
    // Recalculate timer value for same duty cycle with new period
    Software_PWM_Duty_Cycle_Set_Obj(pwm, pwm->dutyCycle);
    if (pwm->enabled) {
        Software_PWM_Reset_Obj(pwm);
    }
}




/**
  Section: Driver Interface
*/

void TMR2_Initialize (void) {
    // Inititialise timer count to 0
    TMR2 = 0x00;
    //Period = 0.001 s; Frequency = 4000000 Hz; PR2 3999; 
    //PR2 = 0xF9F; // 1ms
    PR2 = 0x18F; // 100us
    //PR2 = 0x27; // 10us
    //TCKPS 1:1; T32 16 Bit; TON enabled; TSIDL disabled; TCS FOSC/2; TGATE disabled; 
    T2CON = 0x8000;

    if (TMR2_InterruptHandler == NULL) {
        TMR2_SetInterruptHandler(&TMR2_CallBack);
    }
    
    pwms[0] = Software_PWM_Create(10000, 500, &PWM0_SetPin);
    pwms[1] = Software_PWM_Create(20000, 500, &PWM1_SetPin);
    pwms[2] = Software_PWM_Create(10000, 500, &PWM2_SetPin);
    pwms[3] = Software_PWM_Create(20000, 500, &PWM3_SetPin);
    //pwms[4] = Software_PWM_Create(1000, 500, &PWM4_Set, &PWM4_Clear);
    
    // Reset timer interrupt occurred flag
    IFS0bits.T2IF = false;
    // Enable timer interrupt
    IEC0bits.T2IE = true;
	
    tmr2_obj.timerElapsed = false;
}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _T2Interrupt (  ) {
    /* Check if the Timer Interrupt/Status is set */

    //***User Area Begin

    // ticker function call;
    // ticker is 1 -> Callback function gets called everytime this ISR executes
    if(TMR2_InterruptHandler) {
        TMR2_InterruptHandler(); 
    }

    //***User Area End

    tmr2_obj.count++;
    tmr2_obj.timerElapsed = true;
    IFS0bits.T2IF = false;
}

void Software_PWM_Reset(uint8_t pwmIndex) {
    if (pwmIndex < NUM_PWM) {
        Software_PWM_Reset_Obj(pwms + pwmIndex);
    }
}

void Software_PWM_Period_Set(uint8_t pwmIndex, uint32_t period) {
    if (pwmIndex < NUM_PWM) {
        Software_PWM_Period_Set_Obj(pwms + pwmIndex, period);
    }
}

void Software_PWM_Duty_Cycle_Set(uint8_t pwmIndex, uint16_t dutyCycle) {
    if (pwmIndex < NUM_PWM) {
        Software_PWM_Duty_Cycle_Set_Obj(pwms + pwmIndex, dutyCycle);
    }
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

void __attribute__ ((weak)) TMR2_CallBack(void)
{
    uint8_t i;
    for (i = 0; i < NUM_PWM; i++) {
        if (pwms[i].enabled) {
            pwms[i].count++;
            if (pwms[i].state) {
                if (pwms[i].count > pwms[i].highValue) {
                    pwms[i].set_pin(PIN_STATE_OFF);
                    pwms[i].state = PIN_STATE_OFF;
                }
            } else {
                if (pwms[i].count >= pwms[i].period) {
                    pwms[i].set_pin(PIN_STATE_ON);
                    pwms[i].state = PIN_STATE_ON;
                    pwms[i].count = 0;
                }
            }
        }
    }
}

void  TMR2_SetInterruptHandler(void (* InterruptHandler)(void))
{ 
    // Disable timer interrupt
    IEC0bits.T2IE = false;
    // Set interrupt callback function
    TMR2_InterruptHandler = InterruptHandler;
    // Renable timer interrupt
    IEC0bits.T2IE = true;
}

void TMR2_Start( void )
{
    // Reset the software status information
    tmr2_obj.timerElapsed = false;
    //Enable timer interrupt
    IEC0bits.T2IE = true;
    // Start the Timer
    T2CONbits.TON = 1;
}

void TMR2_Stop( void )
{
    // Stop the Timer */
    T2CONbits.TON = false;
    // Disable timer interrupt
    IEC0bits.T2IE = false;
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
