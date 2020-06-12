#include "xc.h"
#include <stdbool.h>
#include "mcc_generated_files/pin_manager.h"
#include "motor_encoder.h"


/* 
 * Encoder_Shaft:Wheel ratio
 * Motor Gear Ratio: 78:1
 * Bevel Gear Ratio: 16:25
 * Total: 78 * 16 / 25
 * Scaled by 100
 */
#define GEAR_RATIO 4992

/* 
 * Motor_Shaft_Rev to Encoder_Shaft_Rev
 * Motor Gear Ratio: 78:1
 * Total: 78
 * Scaled by 100
 */
#define M_GEAR_RATIO 7800

/* 
 * Encoder_Shaft_Rev to Counts
 * Number of counts in a revolution of encoder, using X1 encoding
 * (low to high transition only on channel 1)
 */
#define COUNTS_PER_REV 5

/* 
 * Motor_Shaft_Rev to Encoder_Counts
 * M_GEAR_RATIO * COUNTS_PER_REV
 * TRANSPORT
 */
#define M_REV_TO_ENC 390


/* 
 * Motor_Shaft_Rev to Encoder_Counts
 * M_GEAR_RATIO * COUNTS_PER_REV
 * Joint 1
 */
//#define M_REV_TO_ENC 630

/* 
 * Motor_Shaft_Rev to Encoder_Counts
 * M_GEAR_RATIO * COUNTS_PER_REV
 * Joint 2
 */
//#define M_REV_TO_ENC 894

/* 
 * Radius: 0.015m
 * Revolutions Per Second to Linear Speed conversion factor: 2 * pi * Radius
 * Scaled by 100000
 */
#define REVS_TO_LINEAR 9425


/* 
 * Linear Speed (mm/s) to Encoder Counts Per Second conversion factor:
 * ((GEAR_RATIO * COUNTS_PER_REV) / REVS_TO_LINEAR) * 1000
 */
#define LINEAR_TO_ENCODER 2648


//////////////////////////////////////////////////
// Encoder 1 parameters
//////////////////////////////////////////////////

#define ENCODER_1_PINA_GetValue() IO_RB15_GetValue()
#define ENCODER_1_PINB_GetValue() IO_RB14_GetValue()

volatile uint8_t enc1LastState = PIN_STATE_OFF; // Last recorded state change
volatile uint8_t enc1Count = 0; // Number of counts since last processing
volatile bool enc1Forward = true; // Is motor travelling forward
volatile int32_t enc1Position = 0; // Distance in number of counts from initial position
volatile int16_t enc1CountsPerSecond = 0; // Encoder shaft's speed (counts/s)
volatile us_time_t enc1LastEncTimeUS = 0; // Last processing time (us)
volatile us_time_t enc1CurrentEncTimeUS = 0; // Current processing time (us)

//////////////////////////////////////////////////

//////////////////////////////////////////////////
// Encoder 2 parameters
//////////////////////////////////////////////////

#define ENCODER_2_PINA_GetValue() IO_RA3_GetValue()
#define ENCODER_2_PINB_GetValue() IO_RB4_GetValue()

volatile uint8_t enc2LastState = PIN_STATE_OFF; // Last recorded state change
volatile uint8_t enc2Count = 0; // Number of counts since last processing
volatile bool enc2Forward = true; // Is motor travelling forward
volatile int32_t enc2Position = 0; // Distance in number of counts from initial position
volatile int16_t enc2CountsPerSecond = 0; // Encoder shaft's speed (counts/s)
volatile us_time_t enc2LastEncTime = 0; // Last processing time (us)
volatile us_time_t enc2CurrentEncTime = 0; // Current processing time (us)

//////////////////////////////////////////////////


#define MAX_SPEED 10000
//#define MIN_SPEED -MAX_SPEED
#define COUNT_PERIOD 6
// If speed is zero, no interrupt will be generated to update this so
// calling process will check the time since a speed update and zero
// it if greater than this
#define ZERO_SPEED_TIMEOUT 300


//////////////////////////////////////////////////
// Encoder 1 Function Definitions
//////////////////////////////////////////////////

// Only Position processing
/*
void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt(void) {
    // Check if Encoder 1 event and process
    if (enc1LastState == PIN_STATE_OFF) {
        if (ENCODER_1_PINA_GetValue()) {
            enc1LastState = PIN_STATE_ON;
            //if (ENCODER_A_PIN2_GetValue()) {
            if (ENCODER_1_PINA_GetValue()) {
                //forward = false;
                //enc1Position--;
                enc1Position++;
                enc1Forward = true;
            } else {
                //forward = true;
                //enc1Position++;
                enc1Position--;
                enc1Forward = false;
            }
            
        }
    } else if (!ENCODER_1_PINA_GetValue()) {
        enc1LastState = PIN_STATE_OFF;
    }
    
    // Check if Encoder 2 event and process
    if (enc2LastState == PIN_STATE_OFF) {
        if (ENCODER_2_PINA_GetValue()) {
            enc2LastState = PIN_STATE_ON;
            //if (ENCODER_A_PIN2_GetValue()) {
            if (ENCODER_2_PINA_GetValue()) {
                //forward = false;
                //enc2Position--;
                enc2Position++;
                enc2Forward = true;
            } else {
                //forward = true;
                //enc2Position++;
                enc2Position--;
                enc2Forward = false;
            }
        }
    } else if (!ENCODER_2_PINA_GetValue()) {
        enc2LastState = PIN_STATE_OFF;
    }
    _CNIF = 0;
}
*/


// Only Speed processing
void __attribute__ ((interrupt, no_auto_psv)) _CNInterrupt(void) {
    // Check if Encoder 1 event and process
    if (enc1LastState == PIN_STATE_OFF) {
        if (ENCODER_1_PINA_GetValue()) {
            enc1LastState = PIN_STATE_ON;
            if (ENCODER_1_PINB_GetValue()) {
            //if (ENCODER_1_PINA_GetValue()) {
                enc1Forward = false;
                enc1Position--;
                //enc1Forward = true;
            } else {
                enc1Forward = true;
                enc1Position++;
                //enc1Forward = false;
            }
            enc1Count++;
            // process speed
            if (enc1Count == COUNT_PERIOD) {
                // Force a DIVSD instruction, rather than slower software ___udivsi3
                enc1CurrentEncTimeUS = MS_TIMER_get_time_us();
                if (enc1CurrentEncTimeUS == enc1LastEncTimeUS) {
                    enc1CountsPerSecond = MAX_SPEED;
                } else {
                    // Force a DIVSD instruction, rather than software ___udivsi3
                    //countsPerSecond = __builtin_divsd((COUNT_PERIOD * 1000000), currentEncTime - lastEncTime);
                    enc1CountsPerSecond = (COUNT_PERIOD * 1000000) / (enc1CurrentEncTimeUS - enc1LastEncTimeUS);
                }
                
                if (!enc1Forward) {
                    enc1CountsPerSecond *= -1; // Going backwards
                }
                enc1Count = 0;
                enc1LastEncTimeUS = enc1CurrentEncTimeUS;
            }
        }
    } else if (!ENCODER_1_PINA_GetValue()) {
        enc1LastState = PIN_STATE_OFF;
    }
    
    // Check if Encoder 2 event and process
    /*
    if (enc2LastState == PIN_STATE_OFF) {
        if (ENCODER_2_PINA_GetValue()) {
            enc2LastState = PIN_STATE_ON;
            //if (ENCODER_A_PIN2_GetValue()) {
            if (ENCODER_2_PINA_GetValue()) {
                //forward = false;
                enc2Forward = true;
            } else {
                //forward = true;
                enc2Forward = false;
            }
            enc2Count++;
            if (enc2Count == COUNT_PERIOD) {
                // Force a DIVSD instruction, rather than slower software ___udivsi3
                enc2CurrentEncTime = MS_TIMER_get_time_us();
                if (enc2CurrentEncTime == enc2LastEncTime) {
                    enc2CountsPerSecond = MAX_SPEED;
                } else {
                    // Force a DIVSD instruction, rather than software ___udivsi3
                    //countsPerSecond = __builtin_divsd((COUNT_PERIOD * 1000000), currentEncTime - lastEncTime);
                    enc2CountsPerSecond = (COUNT_PERIOD * 1000000) / (enc2CurrentEncTime - enc2LastEncTime);
                }
                if (!enc2Forward) {
                    enc2CountsPerSecond *= -1; // Going backwards
                }
                IO_RB1_Toggle();
                enc2Count = 0;
                enc2LastEncTime = enc2CurrentEncTime;
            }
        }
    } else if (!ENCODER_2_PINA_GetValue()) {
        enc2LastState = PIN_STATE_OFF;
    }*/
    _CNIF = 0;
}


void Encoder_Initialise(void) {
    _CN11IE = 1;
    _CNIF = 0; // Clear Change Notification interrupt flag
    _CNIE = 1; // Enable Change Notification interrupt
}

//////////////////////////////////////////////////
// Encoder 1 Function Definitions
//////////////////////////////////////////////////

//int16_t Encoder_1_get_shaft_ang_speed(void) {
//    return ((uint32_t)enc1CountsPerSecond * 1000) / LINEAR_TO_ENCODER;
//}


int16_t Encoder_1_get_shaft_ang_speed(void) {
    if (enc1CountsPerSecond < 0) {
        int32_t temp1 = (int32_t)enc1CountsPerSecond;
        int32_t temp2 = temp1 | 0xFFFF0000;
        int32_t temp3 = temp2 * 1000;
        int32_t temp4 = temp3  / M_REV_TO_ENC;
        int16_t res = (int16_t) temp4;
        return res;
    } else {
        return ((int32_t)enc1CountsPerSecond * 1000) / M_REV_TO_ENC;
    }
}

int32_t Encoder_1_get_position(void) {
    return enc1Position;
}

int32_t Encoder_1_shaft_revs(void) {
    return (enc1Position * 1000) / M_REV_TO_ENC;
}

void Encoder_1_process(void) {
    //uint16_t test1 = MS_TIMER_get_time_ms();
    //uint16_t test2 = enc1LastEncTimeUS / 1000;
    //uint16_t test3 = MS_TIMER_get_time_ms() - enc1LastEncTimeUS / 1000;
    if ((MS_TIMER_get_time_ms() - enc1LastEncTimeUS / 1000) > ZERO_SPEED_TIMEOUT) {
        enc1CountsPerSecond = 0;
    }
}

ms_time_t Encoder_1_get_last_enc_time(void) {
    return enc1LastEncTimeUS;
}

//////////////////////////////////////////////////


//////////////////////////////////////////////////
// Encoder 2 Function Definitions
//////////////////////////////////////////////////

int16_t Encoder_2_get_shaft_ang_speed(void) {
    return ((uint32_t)enc2CountsPerSecond * 1000) / LINEAR_TO_ENCODER;
}

int32_t Encoder_2_get_position(void) {
    return enc2Position;
}

ms_time_t Encoder_2_get_last_end_time(void) {
    return enc2LastEncTime;
}

//////////////////////////////////////////////////