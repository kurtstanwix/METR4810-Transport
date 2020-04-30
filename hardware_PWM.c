
/**
  OC2 Generated Driver API Source File

  @Company
    Microchip Technology Inc.

  @File Name
    oc2.c

  @Summary
    This is the generated source file for the OC2 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for OC2.
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

#include "hardware_PWM.h"
#include "mcc_generated_files/tmr3.h"

/**
  Section: Driver Interface
*/
static const uint16_t OCMode = 0b110; // Edge-Aligned PWM;

void Hardware_PWM_Initialise (void) {
    /* Output Compare 2 Control Register 1
     * bit 13: OCSIDL (Stop in Idle Mode): Don't stop -> 0
     * bits 12-10: OCTSEL (Timer): As specified in hardware_PWM.h
     * bit 9: ENFLT2 (Comparator Fault Input): Disabled -> 0
     * bit 8: ENFLT1 (OCFB Fault Input): Disabled -> 0
     * bit 7: ENFLT0 (OCFA Fault Input): Disabled -> 0
     * bit 6: OCFLT2 (Comparator Fault Status): Disabled -> 0
     * bit 5: OCFLT1 (OCFB Fault Status): Disabled -> 0
     * bit 4: OCFLT0 (OCFA Fault Status): Disabled -> 0
     * bit 3: TRIGMODE (Trigger Mode) Not triggering -> 0
     * bits 2-0: OCM (Mode) Edge-Aligned PWM -> 110 (000 to turn off for now)
     */
    OC2CON1 = 0x0000; // Init
    switch (HARDWARE_PWM_TMR) {
        case 1:
            OC2CON1bits.OCTSEL = 0b100;
            break;
        case 2:
            OC2CON1bits.OCTSEL = 0b000;
            break;
        case 3:
            OC2CON1bits.OCTSEL = 0b001;
            break;
        case 4:
            OC2CON1bits.OCTSEL = 0b010;
            break;
        case 5:
            OC2CON1bits.OCTSEL = 0b011;
    }
    
    /* Output Compare 2 Control Register 2
     * bit 15: FLTMD (Fault Mode): Cycle -> 0
     * bit 14: FLTOUT (Fault Output): Low -> 0
     * bit 13: FLTTRIEN (Fault Pin State): Unaffected -> 0
     * bit 12: OCINV (Invert Output): Disabled -> 0
     * bit 10-9: DCB (Falling edge location): Start of cycle -> 00
     * bit 8: OC32 (32 bit mode): disabled -> 0
     * bit 7: OCTRIG (Sync or Trigger): Sync with self -> 0
     * bit 6: TRIGSTAT (Trigger Status) Not triggered -> 0
     * bit 5: OCTRIS (Pin direction) peripheral on pin -> 0
     * bits 4-0: SYNCSEL (Trigger/Synchronisation source) Self -> 11111
     */
    OC2CON2 = 0x0000; // Init
    //OC2CON2bits.OCTRIG = 0b0;
    OC2CON2bits.SYNCSEL = 0b11111;
    
    // OC2RS 0
    OC2RS = 0x0;
    // OC2R 0; 
    OC2R = 0x0;
    // OC2TMR 0; 
    OC2TMR = 0x00;
	
    Hardware_PWM_TMR_Function(Initialize());
}

void Hardware_PWM_Period_Set_us(uint32_t us) {
    /* Datasheet has period formula of:
     * period = (OC2RS + 1) / TMRFreq */
    
    /* Maximum period (in us) is given by:
       (65536 / TMRFreq) * 1000000
       Modify the timer's prescaler accordingly */
    // Minimum is prescaler / 2 in uS
    // Maximum is prescaler * 2^14 in uS
    // Resolution is prescaler / 4 in uS
    OC2RS = (uint16_t) (us * (TMR3_FrequencyGet() / 1000000) - 1);
}

void Hardware_PWM_Pulse_Width_Set_us(uint32_t us) {
    // Minimum is prescaler / 2 in uS
    // Maximum is prescaler * 2^14 in uS
    // Resolution is prescaler / 4 in uS
    OC2R = (uint16_t) (us * (TMR3_FrequencyGet() / 1000000) - 1);
}

// Duty cycle in 1/10th of 1%: 1000 = 100%
void Hardware_PWM_Duty_Cycle_Set(uint16_t percent) {
    OC2R = (uint16_t)((uint32_t)OC2RS * percent / 1000);
}

void Hardware_PWM_Start(void) {
    OC2CON1bits.OCM = OCMode;
    Hardware_PWM_TMR_Function(Start());
}

void Hardware_PWM_Stop(void) {
    OC2CON1bits.OCM = 0;
    Hardware_PWM_TMR_Function(Stop());
}