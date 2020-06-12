#ifndef MOTOR_ENCODER_H
#define	MOTOR_ENCODER_H

#include <xc.h>
#include "ms_timer.h"

void Encoder_Initialise(void);

//////////////////////////////////////////////////
// Encoder 1 Function Declarations
//////////////////////////////////////////////////

int16_t Encoder_1_get_shaft_ang_speed(void);
int32_t Encoder_1_get_position(void);
int32_t Encoder_1_shaft_revs(void);
ms_time_t Encoder_1_get_last_enc_time(void);
void Encoder_1_process(void);

//////////////////////////////////////////////////


//////////////////////////////////////////////////
// Encoder 2 Function Declarations
//////////////////////////////////////////////////

int16_t Encoder_2_get_shaft_ang_speed(void);
int32_t Encoder_2_get_position(void);
ms_time_t Encoder_2_get_last_enc_time(void);

//////////////////////////////////////////////////

#endif	// MOTOR_ENCODER_H

