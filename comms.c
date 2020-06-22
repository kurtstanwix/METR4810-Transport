#include "xc.h"
#include "comms.h"
#include "at_control.h"
#include "uart_manager.h"
#include "ms_timer.h"
#include "hardware_PWM.h"

BUFFER_OBJ rcvBuffer;
BUFFER_OBJ sndBuffer;
BUFFER_OBJ ackBuffer;
BUFFER_OBJ packetBuffer;
BUFFER_OBJ replyBuffer;
bool sent = false;

#define COMMAND_LENGTH 3
#define COMMAND_ACK_INDEX 0
#define COMMAND_SPD_INDEX 1

const char *commands[] = {
    "ACK",
    "SPD"
};


#define STATE_RCV_CONNECTING 0
#define STATE_RCV_SEQ_U 1
#define STATE_RCV_SEQ_V 2
// Transport's U Receiving (Controller's X Sending) Sequence Number as per FSM
#define RCV_SEQNUM_U 0
// Transport's V Receiving (Controller's Y Sending) Sequence Number as per FSM
#define RCV_SEQNUM_V 1

#define STATE_SND_CONNECTING 0
#define STATE_SND_SEQ_X 1
#define STATE_SND_SEQ_X_ACK 2
#define STATE_SND_SEQ_Y 3
#define STATE_SND_SEQ_Y_ACK 4
// Transport's X Sending (Controller's U Receiving) Sequence Number as per FSM
#define SND_SEQNUM_X 2
// Transport's Y Sending (Controller's V Receiving) Sequence Number as per FSM
#define SND_SEQNUM_Y 3

// Time out of no ACK before resending
#define COMMS_TIMEOUT 2000

#define ASCII_OFFSET 48

uint8_t rcvState = STATE_RCV_CONNECTING; // Starting state
uint8_t sndState = STATE_SND_CONNECTING; // Starting state
bool rcvACKX = false; // Flag for received an X ACK
bool rcvACKY = false; // Flag for received an Y ACK
ms_time_t sentTime; // Last sent time
uint8_t rcvStatus;
uint8_t receivedSeqNum;

bool packetReady = false; // If some input is ready to be read

/**
 * Initialise the communications FSM and connect to the Controller
 */
void init_comms(void) {
    init_buffer(&rcvBuffer, PACKET_LENGTH);
    init_buffer(&sndBuffer, PACKET_LENGTH);
    init_buffer(&packetBuffer, PACKET_LENGTH - 1);
    init_buffer(&ackBuffer, 7);
    init_buffer(&replyBuffer, PACKET_LENGTH);
    
    print_debug("Waiting for connection...", 25, true);
    wait_for_connection();
    print_debug("Connected", 9, true);

    rcvState = STATE_RCV_SEQ_U;
    sndState = STATE_SND_SEQ_X;

    sent = true;
    
}

/**
 * Get the packet if one has been received
 * @param dest Where to put the contents of the packet
 * @return true if a packet was read, false otherwise
 */
bool comms_get_packet(BUFFER_OBJ *dest) {
    if (packetReady) {
        copy_to_buffer(dest, packetBuffer.buffer, packetBuffer.tail - packetBuffer.buffer, true);
        packetReady = false;
        return true;
    }
    return false;
}

/**
 * Add a packet to be sent by the FSM
 * @param toSend Buffer with packet to send
 * @return  true if the packet was sent, false otherwise
 */
bool comms_send(BUFFER_OBJ *toSend) {
    if (!sent) {
        // Already sending something, can't send again until processed
        return false;
    }
    sndBuffer.tail = sndBuffer.buffer + 1; // Shift right by one to make room for seq num
    copy_to_buffer(&sndBuffer, toSend->buffer, toSend->tail - toSend->buffer, false);
    sent = false;
    return true;
}

/**
 * Send an ACK packet with the specified acknowledgement number
 * @param ackNum the sequence number that's being acknowledged
 */
void send_ACK(uint8_t ackNum) {
    ackNum += ASCII_OFFSET;
    copy_to_buffer(&ackBuffer, "0ACK", 4, true);
    copy_to_buffer(&ackBuffer, &ackNum, 1, false);
    copy_to_buffer(&ackBuffer, "\r\n", 2, false);
    send_buffer(&ackBuffer, BLUETOOTH_UART_NUM, true);
}

/**
 * Implements the FSM, should be called periodically
 */
void process_comms(void) {
    if (!BT_IS_CONNECTED()) {
        print_debug("Lost Connection\n\rReconnecting...", 32, true);
        wait_for_connection();
        print_debug("Reconnected", 11, true);
        rcvState = STATE_RCV_SEQ_U;
        sndState = STATE_SND_SEQ_X;
        sent = true;
    }

    rcvStatus = read_line_to_buffer(&rcvBuffer, BLUETOOTH_UART_NUM, 1);
    if (rcvStatus == UART_READ_LINE_RECEIVED) {
        // Process ACK's seperately from rcv FSM
        if (compare_strings(rcvBuffer.buffer + 1, commands[COMMAND_ACK_INDEX], COMMAND_LENGTH)) {
            if (rcvBuffer.buffer[4] - ASCII_OFFSET == SND_SEQNUM_X) {
                rcvACKX = true;
            } else if (rcvBuffer.buffer[4] - ASCII_OFFSET == SND_SEQNUM_Y) {
                rcvACKY = true;
            }
        } else {
            receivedSeqNum = rcvBuffer.buffer[0] - ASCII_OFFSET;
            switch (rcvState) {
                case STATE_RCV_SEQ_U:
                    if (receivedSeqNum == RCV_SEQNUM_U) {
                        // Correct sequence number received, extract packet,
                        // set ready flag and send ACK
                        print_debug("Received: ", 10, false);
                        print_debug(rcvBuffer.buffer, rcvBuffer.tail - rcvBuffer.buffer, false);
                        copy_to_buffer(&packetBuffer, rcvBuffer.buffer + 1, rcvBuffer.tail - rcvBuffer.buffer - 1, true);
                        packetReady = true;
                        send_ACK(RCV_SEQNUM_U);
                        rcvState = STATE_RCV_SEQ_V;
                    }// Otherwise incorrect sequence number, discard packet
                    else {
                        print_debug("Incorrect Seq Num. Expected: ", 29, false);
                        char temp = RCV_SEQNUM_U + ASCII_OFFSET;
                        print_debug(&temp, 1, false);
                        print_debug(", Got: ", 7, false);
                        print_debug(rcvBuffer.buffer, 1, true);
                    }
                    break;
                case STATE_RCV_SEQ_V:
                    if (receivedSeqNum == RCV_SEQNUM_V) {
                        // Correct sequence number received, extract packet,
                        // set ready flag and send ACK
                        print_debug("Received: ", 10, false);
                        print_debug(rcvBuffer.buffer, rcvBuffer.tail - rcvBuffer.buffer, false);
                        copy_to_buffer(&packetBuffer, rcvBuffer.buffer + 1, rcvBuffer.tail - rcvBuffer.buffer - 1, true);
                        packetReady = true;
                        send_ACK(RCV_SEQNUM_V);
                        rcvState = STATE_RCV_SEQ_U;
                    }// Otherwise incorrect sequence number, discard packet
                    else {
                        // Incorrect sequence number, discard packet
                        print_debug("Incorrect Seq Num. Expected: ", 29, false);
                        char temp = RCV_SEQNUM_V + ASCII_OFFSET;
                        print_debug(&temp, 1, false);
                        print_debug(", Got: ", 7, false);
                        print_debug(rcvBuffer.buffer, 1, true);
                    }
                    break;
            }
        }
        rcvBuffer.tail = rcvBuffer.buffer;
    }

    switch (sndState) {
        case STATE_SND_SEQ_X:
            if (!sent) {
                // Prepend the sequence number and send, starting the timeout timer
                sndBuffer.buffer[0] = SND_SEQNUM_X + ASCII_OFFSET;
                send_buffer(&sndBuffer, BLUETOOTH_UART_NUM, false);
                sentTime = MS_TIMER_get_time_ms();
                sndState = STATE_SND_SEQ_X_ACK;
            }
            break;
        case STATE_SND_SEQ_X_ACK:
            if (rcvACKX) {
                // Have received the correct ACK, clear send buffer and move to next seq num
                sent = true;
                sndState = STATE_SND_SEQ_Y;
                sndBuffer.tail = sndBuffer.buffer;
            } else if (MS_TIMER_get_time_ms() - sentTime > COMMS_TIMEOUT) {
                // No ACK received in time, resend message
                sndState = STATE_SND_SEQ_X;
            }
            break;
        case STATE_SND_SEQ_Y:
            if (!sent) {
                // Prepend the sequence number and send, starting the timeout timer
                sndBuffer.buffer[0] = SND_SEQNUM_Y + ASCII_OFFSET;
                send_buffer(&sndBuffer, BLUETOOTH_UART_NUM, false);
                sentTime = MS_TIMER_get_time_ms();
                sndState = STATE_SND_SEQ_Y_ACK;
            }
            break;
        case STATE_SND_SEQ_Y_ACK:
            if (rcvACKY) {
                // Have received the correct ACK, clear send buffer and move to next seq num
                sent = true;
                sndState = STATE_SND_SEQ_X;
                sndBuffer.tail = sndBuffer.buffer;
            } else if (MS_TIMER_get_time_ms() - sentTime > COMMS_TIMEOUT) {
                // No ACK received in time, resend message
                sndState = STATE_SND_SEQ_Y;
            }
            break;
    }
    rcvACKX = false;
    rcvACKY = false;
}

// Max signed int16 value, won't be sent if valid packet
#define INVALID_PARAM 32767

// 
/**
 * Searches backwards from the buffer's tail to find the prevChar and returns
 * the integer parameter between the two.
 * @param packet packet to get the parameter from
 * @param prevChar the character before the first digit of the parameter
 * @return  the read parameter, or INVALID_PARAM if invalid
 */
int16_t get_param(BUFFER_OBJ *packet, uint8_t prevChar) {
    int16_t result = 0;
    uint8_t j;
    uint8_t *startIndex = packet->tail;
    while (*(packet->tail) != prevChar) {
        uint16_t digit = *(packet->tail);
        if (digit == '-') {
            // Negative sign, invert and that's the last of the parameter
            result *= -1;
            packet->tail--;
            break;
        }
        if (digit < 0 + ASCII_OFFSET || digit > 9 + ASCII_OFFSET) {
            // Parameter contains non numeric value
            //print_debug("Invalid Param for MOV", 21, true);
            return INVALID_PARAM; // An invalid number
        }
        digit -= ASCII_OFFSET;
        // Convert digit into it value based on its position
        for (j = 0; j < (startIndex - packet->tail); j++) {
            digit *= 10;
        }
        // Add digit to overall value
        result += digit;
        packet->tail--;
    }
    return result;
}

/**
 * Sets the speed of the Motors based on a received command
 * @param speed speed to set the motor
 */
void set_speed(int16_t speed) {
    if (speed > 0) {
        Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 1000);
        Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM2_OC_NUM, 1000 - (uint16_t) speed);
    } else if (speed < 0) {
        Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 1000 - (uint16_t) (speed * -1));
        Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM2_OC_NUM, 1000);
    } else {
        Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 1000);
        Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM2_OC_NUM, 1000);
    }
}

/**
 * Processes a received packet, if valid, or discards
 * @param packet packet to process
 */
void process_packet(BUFFER_OBJ *packet) {
    if (compare_strings(packet->buffer, commands[COMMAND_SPD_INDEX], COMMAND_LENGTH)) {
        packet->tail -= 3;
        uint16_t speed = get_param(packet, commands[COMMAND_SPD_INDEX][COMMAND_LENGTH - 1]);
        set_speed(speed);
        // Send the response once command is completed
        copy_to_buffer(&replyBuffer, "SPD\r\n", 5, true);
        comms_send(&replyBuffer);
    } else {
        print_debug("Invalid Command", 15, true);
        print_debug(packet->buffer, packet->tail - packet->buffer, false);
    }
}