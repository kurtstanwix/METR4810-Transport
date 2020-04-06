/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
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
  Section: Included Files
*/
#include "mcc_generated_files/clock.h"
#include "mcc_generated_files/oc2.h"
#define FCY _XTAL_FREQ
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "libpic30.h"

/*
void delayUs(uint32_t delay_in_ms) {
     uint32_t tWait = ( GetSystemClock() / 2000 ) * delay_in_ms;
     uint32_t tStart = ReadCoreTimer();
     while( ( ReadCoreTimer() - tStart ) < tWait );
 }
*/
/*
                         Main application
 */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    uint16_t brightness = 0;
    uint16_t count = 0;
    uint8_t state = 0;
    uint32_t period = 1000;
    
    //OC2_PWMPeriodSet(20000);
    OC2_PWMPeriodSetUS(20000);
    //Software_PWM_Period_Set(0, period);
    //Software_PWM_Duty_Cycle_Set(0, 50);
    //OC2_PWMPulseWidthSetUS(500000);
    //OC2RS = 65535;
    //OC2_PWMPulseWidthSet(16384);
    while (1)
    {
        if (brightness > 100) {
            brightness = 0;
        }
        //OC2_PWMPulseWidthSet(1000000);
        /*
        if (state == 0 && count >= 3) {
            Software_PWM_Period_Set(1, 500UL);
            state = 1;
        } else if (state == 1 && count >= 6) {
            Software_PWM_Period_Set(1, 1000UL);
            state = 2;
        } else if (state == 2 && count >= 9) {
            Software_PWM_Period_Set(1, 1500UL);
            state = 3;
        } else if (state == 3 && count >= 12) {
            Software_PWM_Period_Set(1, 2000UL);
            state = 0;
            count = 0;
        }*/
        OC2_PWMDutyCycleSet(brightness);
        __delay_ms(1000);
        brightness += 10;
        count++;
        //IO_RA2_Toggle();
    }

    return 1;
}
/**
 End of File
*/

