#include "hardware_PWM.h"
#include <stdbool.h>
#include "mcc_generated_files/clock.h"

// Current mode of OC
static const uint16_t OCMode = 0b110; // Edge-Aligned PWM;

// Duty cycles of each PWM
uint16_t dutyCycle1 = 500;
uint16_t dutyCycle2 = 500;
uint16_t dutyCycle3 = 500;

/*
 * Valid values are:
 * 256, 64, 8, 1
 * Do not set otherwise
 */
#define HARDWARE_PWM_TMR_PRESCALER 8
#define HARDWARE_PWM_TMR_FREQUENCY (float)(CLOCK_PeripheralFrequencyGet() / HARDWARE_PWM_TMR_PRESCALER)

#define HARDWARE_PWM_OC_REGISTER(OC_NUM, REGISTER) PREPROCESSOR_STITCH(OC, PREPROCESSOR_STITCH(OC_NUM, REGISTER))

/**
 * Intialises the Hardware PWM's to be ready for use
 */
void Hardware_PWM_Initialise(void) {
#ifdef HARDWARE_PWM1_OC_NUM
    /* Output Compare for PWM1 Control Register 1
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
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, CON1) = 0x0000; // Init
    switch (HARDWARE_PWM_TMR) {
        case 1: // TMR1
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, CON1bits.OCTSEL) = 0b100;
            break;
        case 2: // TMR2
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, CON1bits.OCTSEL) = 0b000;
            break;
        case 3: // TMR3
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, CON1bits.OCTSEL) = 0b001;
            break;
        case 4: // TMR4
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, CON1bits.OCTSEL) = 0b010;
            break;
        case 5: // TMR5
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, CON1bits.OCTSEL) = 0b011;
    }

    /* Output Compare for PWM1 Control Register 2
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
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, CON2) = 0x0000; // Init
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, CON2bits.SYNCSEL) = 0b11111;

    // OCxRS 0
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, RS) = 0x0;
    // OCxR 0; 
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, R) = 0x0;
    // OCxTMR 0; 
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, TMR) = 0x00;
#endif
#ifdef HARDWARE_PWM2_OC_NUM
    /* Output Compare for PWM2 Control Register 1
     * Same as PWM1 */
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, CON1) = 0x0000; // Init
    switch (HARDWARE_PWM_TMR) {
        case 1:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, CON1bits.OCTSEL) = 0b100;
            break;
        case 2:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, CON1bits.OCTSEL) = 0b000;
            break;
        case 3:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, CON1bits.OCTSEL) = 0b001;
            break;
        case 4:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, CON1bits.OCTSEL) = 0b010;
            break;
        case 5:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, CON1bits.OCTSEL) = 0b011;
    }

    /* Output Compare for PWM2 Control Register 2
     * Same as PWM1 */
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, CON2) = 0x0000; // Init
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, CON2bits.SYNCSEL) = 0b11111;

    // OCxRS 0
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, RS) = 0x0;
    // OCxR 0; 
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, R) = 0x0;
    // OCxTMR 0; 
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, TMR) = 0x00;
#endif
#ifdef HARDWARE_PWM3_OC_NUM
    /* Output Compare for PWM3 Control Register 1
     * Same as PWM1 */
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, CON1) = 0x0000; // Init
    switch (HARDWARE_PWM_TMR) {
        case 1:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, CON1bits.OCTSEL) = 0b100;
            break;
        case 2:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, CON1bits.OCTSEL) = 0b000;
            break;
        case 3:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, CON1bits.OCTSEL) = 0b001;
            break;
        case 4:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, CON1bits.OCTSEL) = 0b010;
            break;
        case 5:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, CON1bits.OCTSEL) = 0b011;
    }

    /* Output Compare for PWM3 Control Register 2
     * Same as PWM1 */
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, CON2) = 0x0000; // Init
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, CON2bits.SYNCSEL) = 0b11111;

    // OCxRS 0
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, RS) = 0x0;
    // OCxR 0; 
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, R) = 0x0;
    // OCxTMR 0; 
    HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, TMR) = 0x00;
#endif

    TMR_TMR_REGISTER(HARDWARE_PWM_TMR) = 0x0000;
    TMR_PR_REGISTER(HARDWARE_PWM_TMR) = 0x0000;
    TMR_TxCON_REGISTER(HARDWARE_PWM_TMR) = 0x0000;
    switch (HARDWARE_PWM_TMR_PRESCALER) {
        case 256:
            TMR_TxCONbits(HARDWARE_PWM_TMR).TCKPS = 0b11;
            break;
        case 64:
            TMR_TxCONbits(HARDWARE_PWM_TMR).TCKPS = 0b10;
            break;
        case 8:
            TMR_TxCONbits(HARDWARE_PWM_TMR).TCKPS = 0b01;
            break;
        case 1:
            TMR_TxCONbits(HARDWARE_PWM_TMR).TCKPS = 0b00;
            break;
    }
    TMR_TxCONbits(HARDWARE_PWM_TMR).TON = 1;
    //TMR_FUNCTION(HARDWARE_PWM_TMR, Initialize());
}

/**
 * Sets the period of the PWM in microseconds
 * @param pwmNum PWM to set period of
 * @param us period of PWM in microseconds
 */
void Hardware_PWM_Period_Set_us(uint8_t pwmNum, uint32_t us) {
    /* Datasheet has period formula of:
     * period = (OC2RS + 1) / TMRFreq */

    /* Maximum period (in us) is given by:
       (65536 / TMRFreq) * 1000000
       Modify the timer's prescaler accordingly */
    // Minimum is prescaler / 2 in uS
    // Maximum is prescaler * 2^14 in uS
    // Resolution is prescaler / 4 in uS
    switch (pwmNum) {
#ifdef HARDWARE_PWM1_OC_NUM
        case HARDWARE_PWM1_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, RS) = (uint16_t) (us * (HARDWARE_PWM_TMR_FREQUENCY / 1000000) - 1);
            Hardware_PWM_Duty_Cycle_Set(pwmNum, dutyCycle1);
            break;
#endif
#ifdef HARDWARE_PWM2_OC_NUM
        case HARDWARE_PWM2_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, RS) = (uint16_t) (us * (HARDWARE_PWM_TMR_FREQUENCY / 1000000) - 1);
            Hardware_PWM_Duty_Cycle_Set(pwmNum, dutyCycle2);
            break;
#endif
#ifdef HARDWARE_PWM3_OC_NUM
        case HARDWARE_PWM3_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, RS) = (uint16_t) (us * (HARDWARE_PWM_TMR_FREQUENCY / 1000000) - 1);
            Hardware_PWM_Duty_Cycle_Set(pwmNum, dutyCycle3);
            break;
#endif
    }
    
}

/**
 * Set the pulse width of the PWM in microseconds
 * @param pwmNum PWM to set the pulse width of
 * @param us pulse width of PWM in microseconds
 */
void Hardware_PWM_Pulse_Width_Set_us(uint8_t pwmNum, uint32_t us) {
    // Minimum is prescaler / 2 in uS
    // Maximum is prescaler * 2^14 in uS
    // Resolution is prescaler / 4 in uS

    switch (pwmNum) {
#ifdef HARDWARE_PWM1_OC_NUM
        case HARDWARE_PWM1_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, R) = (uint16_t) (us * (HARDWARE_PWM_TMR_FREQUENCY / 1000000) - 1);
            break;
#endif
#ifdef HARDWARE_PWM2_OC_NUM
        case HARDWARE_PWM2_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, R) = (uint16_t) (us * (HARDWARE_PWM_TMR_FREQUENCY / 1000000) - 1);
            break;
#endif
#ifdef HARDWARE_PWM3_OC_NUM
        case HARDWARE_PWM3_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, R) = (uint16_t) (us * (HARDWARE_PWM_TMR_FREQUENCY / 1000000) - 1);
            break;
#endif
    }
}

/**
 * Set the duty cycle of the PWM given in 1/10th of 1%: 1000 = 100%
 * @param pwmNum PWM to set the duty cycle of
 * @param percent 1/10th of a percentage to set the PWM's duty cycle
 */
void Hardware_PWM_Duty_Cycle_Set(uint8_t pwmNum, uint16_t percent) {
    switch (pwmNum) {
#ifdef HARDWARE_PWM1_OC_NUM
        case HARDWARE_PWM1_OC_NUM:
            dutyCycle1 = percent;
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, R) = \
                    (uint16_t) ((uint32_t) HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, RS) * percent / 1000);
            break;
#endif
#ifdef HARDWARE_PWM2_OC_NUM
        case HARDWARE_PWM2_OC_NUM:
            dutyCycle2 = percent;
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, R) = \
                    (uint16_t) ((uint32_t) HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, RS) * percent / 1000);
            break;
#endif
#ifdef HARDWARE_PWM3_OC_NUM
        case HARDWARE_PWM3_OC_NUM:
            dutyCycle3 = percent;
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, R) = \
                    (uint16_t) ((uint32_t) HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, RS) * percent / 1000);
            break;
#endif
    }
}

/**
 * Starts the individual PWM OC module
 * @param pwmNum PWM to start
 */
void Hardware_PWM_Start(uint8_t pwmNum) {
    switch (pwmNum) {
#ifdef HARDWARE_PWM1_OC_NUM
        case HARDWARE_PWM1_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, CON1bits.OCM) = OCMode;
            break;
#endif
#ifdef HARDWARE_PWM2_OC_NUM
        case HARDWARE_PWM2_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, CON1bits.OCM) = OCMode;
            break;
#endif
#ifdef HARDWARE_PWM3_OC_NUM
        case HARDWARE_PWM3_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, CON1bits.OCM) = OCMode;
            break;
#endif
    }
}

/**
 * Stops the individual PWM OC module
 * @param pwmNum PWM to stop
 */
void Hardware_PWM_Stop(uint8_t pwmNum) {
    switch (pwmNum) {
#ifdef HARDWARE_PWM1_OC_NUM
        case HARDWARE_PWM1_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM1_OC_NUM, CON1bits.OCM) = 0;
            break;
#endif
#ifdef HARDWARE_PWM2_OC_NUM
        case HARDWARE_PWM2_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM2_OC_NUM, CON1bits.OCM) = 0;
            break;
#endif
#ifdef HARDWARE_PWM3_OC_NUM
        case HARDWARE_PWM3_OC_NUM:
            HARDWARE_PWM_OC_REGISTER(HARDWARE_PWM3_OC_NUM, CON1bits.OCM) = 0;
            break;
#endif
    }
}

/**
 * Enables the timer the PWM is using. Must be called before use
 */
void Hardware_PWM_Enable(void) {
    TMR_TxCONbits(HARDWARE_PWM_TMR).TON = 1;
}

/**
 * Enables the timer the PWM is using.
 */
void Hardware_PWM_Disable(void) {
#ifdef HARDWARE_PWM1_OC_NUM
    Hardware_PWM_Stop(HARDWARE_PWM1_OC_NUM);
#endif
#ifdef HARDWARE_PWM2_OC_NUM
    Hardware_PWM_Stop(HARDWARE_PWM2_OC_NUM);
#endif
#ifdef HARDWARE_PWM3_OC_NUM
    Hardware_PWM_Stop(HARDWARE_PWM3_OC_NUM);
#endif
    TMR_TxCONbits(HARDWARE_PWM_TMR).TON = 0;
}