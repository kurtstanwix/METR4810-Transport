#ifndef COMMS_H
#define	COMMS_H

#include <xc.h>
#include <stdbool.h>
#include "buffer.h"
#include "mcc_generated_files/clock.h"


#define PACKET_LENGTH 34

void init_comms(void);
bool comms_get_packet(BUFFER_OBJ *dest);
void process_comms(void);
void set_speed(int16_t speed);
void process_packet(BUFFER_OBJ *packet);

#endif	// COMMS_H

