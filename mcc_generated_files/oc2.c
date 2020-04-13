
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

#include "oc2.h"
#include "tmr1.h"

/** OC Mode.

  @Summary
    Defines the OC Mode.

  @Description
    This data type defines the OC Mode of operation.

*/

static uint16_t         gOC2Mode;

/**
  Section: Driver Interface
*/

void OC2_Initialize (void)
{
    /* Output Compare 2 Control Register 1
     * bit 13: OCSIDL (Stop in Idle Mode): Don't stop -> 0
     * bits 12-10: OCTSEL (Timer): Timer 1 -> 100
     * bit 9: ENFLT2 (Comparator Fault Input): Disabled -> 0
     * bit 8: ENFLT1 (OCFB Fault Input): Disabled -> 0
     * bit 7: ENFLT0 (OCFA Fault Input): Disabled -> 0
     * bit 6: OCFLT2 (Comparator Fault Status): Disabled -> 0
     * bit 5: OCFLT1 (OCFB Fault Status): Disabled -> 0
     * bit 4: OCFLT0 (OCFA Fault Status): Disabled -> 0
     * bit 3: TRIGMODE (Trigger Mode) Not triggering -> 0
     * bits 2-0: OCM (Mode) Edge-Aligned PWM -> 110
     */
    OC2CON1 = 0x1006;
    
    OC2CON1bits.OCM = 0; // Stop for now
    //OC2CON1 = 0x0; // Clear bits
    //OC2CON1bits.OCM = (1 << 2) || (1 << 1);
    //OC2CON1bits.OCTSEL = (1 << 2);
    
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
    //OC2CON2 = 0x1F;
    //OC2CON2 = 0x0; // Clear bits
    //OC2CON2bits.SYNCSEL = (1 << 4) || (1 << 3) || (1 << 2) || (1 << 1) || (1 << 0);
    OC2CON2bits.OCTRIG = 0;
    
    OC2CON2bits.SYNCSEL4 = 1;
    OC2CON2bits.SYNCSEL3 = 1;
    OC2CON2bits.SYNCSEL2 = 1;
    OC2CON2bits.SYNCSEL1 = 1;
    OC2CON2bits.SYNCSEL0 = 1;
    
    // OC2RS 0; 
    OC2RS = 0x00;
    // OC2R 0; 
    OC2R = 0x7D0;
    // OC2TMR 0; 
    OC2TMR = 0x00;
	
    gOC2Mode = OC2CON1bits.OCM;
}

void OC2_PWMPeriodSetUS(uint32_t uS) {
    // Minimum is prescaler / 2 in uS
    // Maximum is prescaler * 2^14 in uS
    // Resolution is prescaler / 4 in uS
    OC2RS = (uint16_t) (uS * (TMR1_TickFrequencyGet() / 1000000) - 1);
}

void OC2_PWMPulseWidthSetUS(uint32_t uS) {
    // Minimum is prescaler / 2 in uS
    // Maximum is prescaler * 2^14 in uS
    // Resolution is prescaler / 4 in uS
    OC2R = (uint16_t) (uS * (TMR1_TickFrequencyGet() / 1000000) - 1);
}

void OC2_PWMDutyCycleSet(uint16_t percent) {
    OC2R = (uint16_t)((uint32_t)OC2RS * percent / 100);
}

void __attribute__ ((weak)) OC2_CallBack(void)
{
    // Add your custom callback code here
}

void OC2_Tasks( void )
{	
    if(IFS0bits.OC2IF)
    {
		// OC2 callback function 
		OC2_CallBack();
        IFS0bits.OC2IF = 0;
    }
}



void OC2_Start( void )
{
    OC2CON1bits.OCM = gOC2Mode;
}


void OC2_Stop( void )
{
    OC2CON1bits.OCM = 0;
}


void OC2_SingleCompareValueSet( uint16_t value )
{
    OC2R = value;
}


void OC2_DualCompareValueSet( uint16_t priVal, uint16_t secVal )
{
    OC2R = priVal;
	
    OC2RS = secVal;
}


void OC2_CentreAlignedPWMConfig( uint16_t priVal, uint16_t secVal )
{
    OC2R = priVal;
	
    OC2RS = secVal;
}

void OC2_EdgeAlignedPWMConfig( uint16_t priVal, uint16_t secVal )
{
    OC2R = priVal;
	
    OC2RS = secVal;
}

void OC2_SecondaryValueSet( uint16_t secVal )
{
   
    OC2RS = secVal;
}


void OC2_PrimaryValueSet( uint16_t priVal )
{
   
    OC2R = priVal;
}

bool OC2_IsCompareCycleComplete( void )
{
    return(IFS0bits.OC2IF);
}


bool OC2_FaultStatusGet( OC2_FAULTS faultNum )
{
    bool status;
    /* Return the status of the fault condition */
   
    switch(faultNum)
    { 
        case OC2_FAULT0:
			status = OC2CON1bits.OCFLT0;
            break;
        case OC2_FAULT1:
			status = OC2CON1bits.OCFLT1;
            break;
        case OC2_FAULT2:
			status = OC2CON1bits.OCFLT2;
            break;
        default :
            break;

    }
    return(status);
}


void OC2_FaultStatusClear( OC2_FAULTS faultNum )
{
    
    switch(faultNum)
    { 
        case OC2_FAULT0:
			OC2CON1bits.OCFLT0 = 0;
                break;
        case OC2_FAULT1:
			OC2CON1bits.OCFLT1 = 0;
                break;
        case OC2_FAULT2:
			OC2CON1bits.OCFLT2 = 0;
                break;
        default :
                break;
    }    
}


void OC2_ManualTriggerSet( void )
{
    OC2CON2bits.TRIGSTAT= true; 
}

bool OC2_TriggerStatusGet( void )
{
    return( OC2CON2bits.TRIGSTAT );
}


void OC2_TriggerStatusClear( void )
{
    /* Clears the trigger status */
    OC2CON2bits.TRIGSTAT = 0;
}

/**
 End of File
*/
