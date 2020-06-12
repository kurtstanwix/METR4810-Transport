#include "mcc_generated_files/clock.h"
#define FCY _XTAL_FREQ
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "libpic30.h"
#include <stdbool.h>
#include "at_control.h"
#include "uart_manager.h"
#include "util.h"
//#include "two_timers_PWM.h"
//#include "one_timer_PWM.h"
//#include "software_PWM.h"
#include "hardware_PWM.h"
#include "motor_encoder.h"


#include "mcc_generated_files/interrupt_manager.h"
#include "ms_timer.h"

#ifdef __USER_DEBUG
#define __USER_CONTROL
#endif
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





// 16 bytes for packet, then 2 for \r\n
//#define PACKET_LENGTH (16+2)
// 32 bytes for commands from pc then 2 for \r\n
//#define COMMAND_LENGTH (64+2)


// Bluetooth AT command toggle pin








// Possible Bluetooth States
//#define BT_STATE_INQ ("INQUIRING", 9)




//#define BT_ROLE_MASTER 1








/* 
 * Encoder Counts Per Second to Linear Speed (nm/s) conversion factor:
 * (REVS_TO_LINEAR * 1000000) / (GEAR_RATIO * COUNTS_PER_REV)
 */
#define ENCODER_TO_LINEAR 377595

typedef struct PI {
    int16_t kp;
    int16_t ki;
    int32_t sumError;
    int32_t output;
    int16_t target;
    us_time_t lastTime;
} PI_OBJ;

PI_OBJ motor1;

// kp and ki must be multiplied by 1000

void PI_init(PI_OBJ *pi, uint16_t kp, uint16_t ki) {
    pi->kp = kp;
    pi->ki = ki;
    pi->sumError = 0;
    pi->lastTime = 0;
    pi->target = 0;
}

int32_t error;

bool PI_compute(PI_OBJ *pi, int16_t input) {
    us_time_t currentTime = MS_TIMER_get_time_us();
    int32_t elapsed = currentTime - pi->lastTime;

    error = pi->target - input;
    pi->sumError += pi->ki * ((error * elapsed) / 1000);

    pi->output = pi->kp * error + pi->sumError;
    if (pi->output > 1000000) {
        pi->sumError -= pi->output - 1000000;
        pi->output = 1000000;
    } else if (pi->output < -1000000) {
        pi->sumError += -1000000 - pi->output;
        pi->output = -1000000;
    }

    pi->lastTime = currentTime;
}
#define MAX_PERIOD 32768

////////////////////////////////////////////////////
// PI CONTROL TEST
////////////////////////////////////////////////////
/*
int main(void) {
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    CLOCK_Initialize();
    UART_Initialise();
    Hardware_PWM_Initialise();
    Hardware_PWM_Period_Set_us(HARDWARE_PWM1_OC_NUM, 100000);
    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 500);
    Hardware_PWM_Enable(); // Start the hardware PWM timer
    Hardware_PWM_Start(HARDWARE_PWM1_OC_NUM);
    MS_TIMER_Initialize();
    Encoder_Initialise();
    init_debug();
    
    PI_init(&motor1, 50, 50);
    
    int32_t pos;
    int32_t err;
    char position[21] = "Position:          \r\n";
    char errorst[18] = "Error:          \r\n";
    uint8_t i;
    motor1.target = 2648;
    uint16_t temp;
    while (1) {
        pos = Encoder_1_get_position();
        PI_compute(&motor1, pos);
        temp = (((1000000 - motor1.output) / 1000) * MAX_PERIOD)  / 1000;
        Hardware_PWM_Period_Set_us(HARDWARE_PWM1_OC_NUM, temp);
        if (motor1.output == 0)
        position[18] = 0;
        position[17] = 0;
        position[16] = 0;
        position[15] = 0;
        position[14] = 0;
        position[13] = 0;
        position[12] = 0;
        position[11] = 0;
        position[10] = 0;
        position[9] = 0;
        
        errorst[15] = 0;
        errorst[14] = 0;
        errorst[13] = 0;
        errorst[12] = 0;
        errorst[11] = 0;
        errorst[10] = 0;
        errorst[9] = 0;
        errorst[8] = 0;
        errorst[7] = 0;
        errorst[6] = 0;
        
        bool neg = pos < 0;
        i = 0;
        if (pos) {
            while (pos) {
                position[18 - (i++)] = pos % 10 + ASCII_OFFSET;
                pos /= 10;
            }
            if (neg) {
                position[18 - i] = '-';
            }
        } else {
            position[18] = '0';
        }
        print_debug(position, 21);
        
        err = error;
        neg = err < 0;
        i = 0;
        if (err) {
            while (err) {
                errorst[15 - (i++)] = err % 10 + ASCII_OFFSET;
                err /= 10;
            }
            if (neg) {
                errorst[15 - i] = '-';
            }
        } else {
            errorst[15] = '0';
        }
        print_debug(errorst, 18);
        
        MS_TIMER_Delay_ms(50);
    }
    
}
 */

////////////////////////////////////////////////////
// MOTOR PROFILING
////////////////////////////////////////////////////
/*
int main(void) {
    PIN_MANAGER_Initialize();
    IO_RB4_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RB4_SetPin(PIN_STATE_ON);
    INTERRUPT_Initialize();
    CLOCK_Initialize();
    UART_Initialise();
    Hardware_PWM_Initialise();
    MS_TIMER_Initialize();
    Encoder_Initialise();
    init_debug();

    IO_RB0_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RB1_SetDirection(PIN_DIRECTION_OUTPUT);



    int16_t dutyCycle;
    int16_t dutyCyclePrint;

    //uint16_t setSpeedms = 6;
    //uint16_t PWMPeriod = __builtin_divsd(ENCODER_TO_LINEAR, setSpeedms);

    Hardware_PWM_Period_Set_us(HARDWARE_PWM1_OC_NUM, 1000);
    Hardware_PWM_Period_Set_us(HARDWARE_PWM2_OC_NUM, 1000);
    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 1000);
    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM2_OC_NUM, 1000);
    Hardware_PWM_Enable(); // Start the hardware PWM timer
    Hardware_PWM_Start(HARDWARE_PWM1_OC_NUM);
    Hardware_PWM_Start(HARDWARE_PWM2_OC_NUM);
    //IO_RB0_SetPin(PIN_STATE_ON);
    //Hardware_PWM_Period_Set_us(HARDWARE_PWM2_OC_NUM, 1500);
    //Hardware_PWM_Enable(); // Start the hardware PWM timer
    //Hardware_PWM_Start(HARDWARE_PWM1_OC_NUM);

    int32_t shaftVel;
    bool neg;
    char out[11] = "  DC ,speed";
    uint8_t i;
    uint8_t j;
    uint8_t count = 0;
    bool forward;
    bool finished;
    uint8_t trial;
    print_debug("Starting", 8, true);
    print_debug(out, 11, true);
    for (trial = 0 + ASCII_OFFSET; trial < 10 + ASCII_OFFSET; trial++) {
        print_debug("Trial ", 6, false);
        
        print_debug(&trial, 1, true);
        forward = true;
        finished = false;
        dutyCycle = 0;
        IO_RB0_SetPin(PIN_STATE_ON);
        Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 1000);
        while (!finished) {

            MS_TIMER_Delay_ms(300);
            Encoder_1_process(); // Checks if speed is zero
            // Clear old output data
            out[10] = ' ';
            out[9] = ' ';
            out[8] = ' ';
            out[7] = ' ';
            out[6] = ' ';

            out[4] = ' ';
            out[3] = ' ';
            out[2] = ' ';
            out[1] = ' ';
            out[0] = ' ';

            // Average of speed over 150ms
            shaftVel = 0;
            for (j = 0; j < 10; j++) {
                shaftVel += Encoder_1_get_shaft_ang_speed(); //Encoder_1_shaft_revs();//
                MS_TIMER_Delay_ms(15);
            }
            shaftVel /= 10;

            // Turn speed into char digits
            i = 0;
            neg = shaftVel < 0;
            if (shaftVel) {
                while (shaftVel) {
                    out[10 - (i++)] = (shaftVel * (neg ? -1 : 1)) % 10 + ASCII_OFFSET;
                    shaftVel /= 10;
                }
                if (neg) {
                    out[10 - i] = '-';
                }
            } else {
                out[10] = '0';
            }

            // Turn duty cycle into char digits
            dutyCyclePrint = dutyCycle;
            i = 0;
            neg = dutyCyclePrint < 0;
            if (dutyCyclePrint) {
                while (dutyCyclePrint) {
                    out[4 - (i++)] = (dutyCyclePrint * (neg ? -1 : 1)) % 10 + ASCII_OFFSET;
                    dutyCyclePrint /= 10;
                }
                if (neg) {
                    out[4 - i] = '-';
                }
            } else {
                out[4] = '0';
            }

            print_debug(out, 11, true);

            if (forward) {
                dutyCycle += 10;
                if (dutyCycle > 1000) {
                    forward = false;
                    dutyCycle = 0;
                    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 1000);
                    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM2_OC_NUM, 1000);
                    MS_TIMER_Delay_ms(600);
                } else {
                    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 1000);
                    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM2_OC_NUM, 1000 - (uint16_t) dutyCycle);
                }
            } else {
                dutyCycle -= 10;
                if (dutyCycle < -1000) {
                    forward = false;
                    finished = true;
                    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 1000);
                    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM2_OC_NUM, 1000);
                    MS_TIMER_Delay_ms(600);
                } else {
                    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 1000 - (uint16_t) (dutyCycle * -1));
                    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM2_OC_NUM, 1000);
                }
            }
            //IO_RB4_Toggle();
            //setSpeedms++;
            //PWMPeriod = __builtin_divsd(ENCODER_TO_LINEAR, setSpeedms);
            //Hardware_PWM_Period_Set_us(HARDWARE_PWM1_OC_NUM, PWMPeriod);
            //MS_TIMER_Delay_ms(50);
        }
    }
}
 */

////////////////////////////////////////////////////
// BLUETOOTH TEST
////////////////////////////////////////////////////


#define PACKET_LENGTH 34
BUFFER_OBJ rcvBuffer;
BUFFER_OBJ sndBuffer;
BUFFER_OBJ ackBuffer;
BUFFER_OBJ packetBuffer;
bool sent = false;


#define STATE_RCV_CONNECTING 0
#define STATE_RCV_SEQ_U 1
#define STATE_RCV_SEQ_V 2
#define RCV_SEQNUM_U 0
#define RCV_SEQNUM_V 1

#define STATE_SND_CONNECTING 0
#define STATE_SND_SEQ_X 1
#define STATE_SND_SEQ_X_ACK 2
#define STATE_SND_SEQ_Y 3
#define STATE_SND_SEQ_Y_ACK 4
#define SND_SEQNUM_X 2
#define SND_SEQNUM_Y 3

#define COMMS_TIMEOUT 2000

#define ASCII_OFFSET 48

uint8_t rcvState = STATE_RCV_CONNECTING;
uint8_t sndState = STATE_SND_CONNECTING;
bool rcvACKX = false;
bool rcvACKY = false;
ms_time_t sentTime;
uint8_t rcvStatus;
uint8_t receivedSeqNum;

bool packetReady = false;

void init_comms(void) {
    init_buffer(&rcvBuffer, PACKET_LENGTH);
    init_buffer(&sndBuffer, PACKET_LENGTH);
    init_buffer(&packetBuffer, PACKET_LENGTH - 1);
    init_buffer(&ackBuffer, 7);
    
    print_debug("Waiting for connection...", 25, true);
    wait_for_connection();
    print_debug("Connected", 9, true);
    
    rcvState = STATE_RCV_SEQ_U;
    sndState = STATE_SND_SEQ_X;
    
    sent = true;
}

bool comms_get_packet(BUFFER_OBJ *dest) {
    if (packetReady) {
        copy_to_buffer(dest, packetBuffer.buffer, packetBuffer.tail - packetBuffer.buffer, true);
        packetReady = false;
        return true;
    }
    return false;
}

bool comms_send(BUFFER_OBJ *toSend) {
    if (!sent) {
        return false;
    }
    sndBuffer.tail = sndBuffer.buffer + 1;
    copy_to_buffer(&sndBuffer, toSend->buffer, toSend->tail - toSend->buffer, false);
    sent = false;
}

void send_ACK(uint8_t seqNum) {
    seqNum += ASCII_OFFSET;
    copy_to_buffer(&ackBuffer, "0ACK", 4, true);
    copy_to_buffer(&ackBuffer, &seqNum, 1, false);
    copy_to_buffer(&ackBuffer, "\r\n", 2, false);
    send_buffer(&ackBuffer, BLUETOOTH_UART_NUM, true);
}

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
        if (compare_strings(rcvBuffer.buffer + 1, "ACK", 3)) {
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
                        print_debug("Received: ", 10, false);
                        print_debug(rcvBuffer.buffer, rcvBuffer.tail - rcvBuffer.buffer, false);
                        copy_to_buffer(&packetBuffer, rcvBuffer.buffer + 1, rcvBuffer.tail - rcvBuffer.buffer - 1, true);
                        packetReady = true;
                        send_ACK(RCV_SEQNUM_U);
                        rcvState = STATE_RCV_SEQ_V;
                    } else {
                        print_debug("Incorrect Seq Num. Expected: ", 29, false);
                        char temp = RCV_SEQNUM_U + ASCII_OFFSET;
                        print_debug(&temp, 1, false);
                        print_debug(", Got: ", 7, false);
                        print_debug(rcvBuffer.buffer, 1, true);
                    }
                    break;
                case STATE_RCV_SEQ_V:
                    if (receivedSeqNum == RCV_SEQNUM_V) {
                        print_debug("Received: ", 10, false);
                        print_debug(rcvBuffer.buffer, rcvBuffer.tail - rcvBuffer.buffer, false);
                        copy_to_buffer(&packetBuffer, rcvBuffer.buffer + 1, rcvBuffer.tail - rcvBuffer.buffer - 1, true);
                        packetReady = true;
                        send_ACK(RCV_SEQNUM_V);
                        rcvState = STATE_RCV_SEQ_U;
                    } else {
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
                sndBuffer.buffer[0] = SND_SEQNUM_X + ASCII_OFFSET;
                send_buffer(&sndBuffer, BLUETOOTH_UART_NUM, false);
                sentTime = MS_TIMER_get_time_ms();
                sndState = STATE_SND_SEQ_X_ACK;
            }
            break;
        case STATE_SND_SEQ_X_ACK:
            if (rcvACKX) {
                sent = true;
                sndState = STATE_SND_SEQ_Y;
                sndBuffer.tail = sndBuffer.buffer;
            } else if (MS_TIMER_get_time_ms() - sentTime > COMMS_TIMEOUT) {
                sndState = STATE_SND_SEQ_X;
            }
            break;
        case STATE_SND_SEQ_Y:
            if (!sent) {
                sndBuffer.buffer[0] = SND_SEQNUM_Y + ASCII_OFFSET;
                send_buffer(&sndBuffer, BLUETOOTH_UART_NUM, false);
                sentTime = MS_TIMER_get_time_ms();
                sndState = STATE_SND_SEQ_Y_ACK;
            }
            break;
        case STATE_SND_SEQ_Y_ACK:
            if (rcvACKY) {
                sent = true;
                sndState = STATE_SND_SEQ_X;
                sndBuffer.tail = sndBuffer.buffer;
            } else if (MS_TIMER_get_time_ms() - sentTime > COMMS_TIMEOUT) {
                sndState = STATE_SND_SEQ_Y;
            }
            break;
    }
    rcvACKX = false;
    rcvACKY = false;
    /*
    uint8_t seqNum = packetBuffer.buffer[0] - ASCII_OFFSET;
            if (seqNum != receiveSeq) {
                print_debug("Incorrect Seq Num. Expected: ", 29, false);
                receiveSeq += ASCII_OFFSET;
                print_debug(&receiveSeq, 1, false);
                receiveSeq -= ASCII_OFFSET;
                print_debug(", Got: ", 7, false);
                print_debug(packetBuffer.buffer, 1, true);
            } else if (compare_strings((packetBuffer.buffer + 1), "MOV", 3)) {
                print_debug("Move Command Received:", 22, true);
                print_debug("Distance = ", 11, false);
                print_debug((packetBuffer.buffer + 4), 4, false);
                print_debug("mm", 2, true);
                
                sendSeq += ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, &sendSeq, 1, true);
                sendSeq -= ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, "ACK", 3, false);
                
                seqNum += ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, &seqNum, 1, false);
                seqNum -= ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, "\r\n", 2, false);
                
                if (receiveSeq == 1) {
                    receiveSeq = 0;
                } else {
                    receiveSeq = 1;
                }
                if (sendSeq == 3) {
                    sendSeq = 2;
                } else {
                    sendSeq = 3;
                }
                
                    send_buffer(&commandBuffer, BLUETOOTH_UART_NUM, true);
            } else if (compare_strings((packetBuffer.buffer + 1), "SPD", 3)) {
                print_debug("Speed Command Received:", 23, true);
                print_debug("Speed = ", 8, false);
                print_debug((packetBuffer.buffer + 4), 3, false);
                print_debug("mm/s", 4, true);
                
                sendSeq += ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, &sendSeq, 1, true);
                sendSeq -= ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, "ACK", 3, false);
                
                seqNum += ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, &seqNum, 1, false);
                seqNum -= ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, "\r\n", 2, false);
                
                if (receiveSeq == 1) {
                    receiveSeq = 0;
                } else {
                    receiveSeq = 1;
                }
                if (sendSeq == 3) {
                    sendSeq = 2;
                } else {
                    sendSeq = 3;
                }
                
                    send_buffer(&commandBuffer, BLUETOOTH_UART_NUM, true);
            } else {
                print_debug("Invalid Command: ", 17, false);
                print_debug(packetBuffer.buffer + 1, packetBuffer.tail - packetBuffer.buffer -1, false);
            }
            packetBuffer.tail = packetBuffer.buffer;
     * */
}

int main(void) {
    // initialize the device
    //SYSTEM_Initialize();
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    CLOCK_Initialize();
    UART_Initialise();
    MS_TIMER_Initialize();
    init_debug();
    
    AT_PIN_Clear();

    PC_CLEAR_RX_BUFFER();
    PC_CLEAR_TX_BUFFER();
    
    init_bluetooth();
    init_comms();
    
    BUFFER_OBJ testRcvBuffer;
    init_buffer(&testRcvBuffer, 32 + 2 - 1);
    BUFFER_OBJ testingBuffer;
    init_buffer(&testingBuffer, 32 + 2 - 1);
    
    uint8_t pcrxStatus;
    bool takingInput = true;

    while (1) {
        process_comms();
        if (comms_get_packet(&testRcvBuffer)) {
            print_debug("Got: ", 5, false);
            print_debug(testRcvBuffer.buffer, testRcvBuffer.tail - testRcvBuffer.buffer, false);
        }
        if (takingInput) {
            pcrxStatus = read_line_to_buffer(&testingBuffer, PC_UART_NUM, 1);
            if (pcrxStatus == UART_READ_LINE_RECEIVED) {
                if (takingInput = comms_send(&testingBuffer)) {
                    testingBuffer.tail = testingBuffer.buffer;
                }
            }
        } else {
            if (takingInput = comms_send(&testingBuffer)) {
                testingBuffer.tail = testingBuffer.buffer;
            }
        }
    }
}

////////////////////////////////////////////////////
// BLUETOOTH TEST
////////////////////////////////////////////////////
/*
int main(void) {
    // initialize the device
    //SYSTEM_Initialize();
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    CLOCK_Initialize();
    UART_Initialise();
    MS_TIMER_Initialize();
    init_debug();
    
    init_comms();

    bool atMode = false;
    AT_PIN_Clear();

    init_buffer(&packetBuffer, COMMAND_LENGTH);
    init_buffer(&commandBuffer, COMMAND_LENGTH);

    uint8_t data = 0;
    uint8_t rxStatus = UART_RX_STATUS_MORE;

    PC_CLEAR_RX_BUFFER();
    PC_CLEAR_TX_BUFFER();
    uint8_t sendCommandTo = PC_UART_NUM;

    uint8_t receiveSeq = 0;
    uint8_t sendSeq = 2;
    
    

    init_bluetooth();

    print_debug("Waiting for connection...", 25, true);
    wait_for_connection();
    print_debug("Connected", 9, true);

    while (1) {
        if (!BT_IS_CONNECTED()) {
            print_debug("Lost Connection\n\rReconnecting...", 32, true);
            wait_for_connection();
            print_debug("Reconnected", 11, true);
        }



#ifdef __USER_CONTROL
        //commandBuffer.tail = commandBuffer.buffer;
        // Allows for the user to send 
        rxStatus = read_line_to_buffer(&commandBuffer, PC_UART_NUM, 1);
        if (rxStatus == UART_READ_LINE_RECEIVED) {
            if (atMode) {
                if (compare_strings(commandBuffer.buffer, "ATMODE OFF\r\n", 12)) {
                    print_debug("ATMODE Turned off", 17, true);
                    atMode = false;
                    AT_PIN_Clear();
                } else if (compare_strings(commandBuffer.buffer, "ATMODE OFF\r\n", 12)) {
                    print_debug("ATMODE Already on", 17, true);
                } else if (compare_strings(commandBuffer.buffer, "RESET\r\n", 7)) {
                    reset_bluetooth();
                    atMode = false;
                    print_debug("BT Reset\r\nATMODE Turned Off", 27, true);
                } else if (compare_strings(commandBuffer.buffer, "AT+", 3) ||
                        compare_strings(commandBuffer.buffer, "AT\r\n", 4)) {
                    // An AT command in ATMode, send to bluetooth for processing
                    send_buffer(&commandBuffer, BLUETOOTH_UART_NUM, true);
                    /*
                } else if (compare_strings(commandBuffer.buffer, "ADDR", 4)) {
                    // Debugging for testing address extracting and creation
                    ADDRESS addr;
                    extract_address(&addr, commandBuffer.buffer+4);
                    copy_to_buffer(&commandBuffer, "Upper: ", 7, true);
                    copy_to_buffer(&commandBuffer, addr.nap, 4, false);
                    copy_to_buffer(&commandBuffer, "\nMiddle: ", 9, false);
                    copy_to_buffer(&commandBuffer, addr.uap, 2, false);
                    copy_to_buffer(&commandBuffer, "\nLower: ", 8, false);
                    copy_to_buffer(&commandBuffer, addr.lap, 2, false);
                    copy_to_buffer(&commandBuffer, "\nAddress: ", 10, false);
                    format_address(&addr);
                    copy_to_buffer(&commandBuffer, addr.formatted, 14, false);
                    copy_to_buffer(&commandBuffer, "\r\n", 2, false);
                        
                        
                    sendCommandTo = PC_UART_NUM;
                     *
                } else {
                    print_debug(commandBuffer.buffer, commandBuffer.tail - commandBuffer.buffer, false);
                }
            } else {
                if (compare_strings(commandBuffer.buffer, "ATMODE ON\r\n", 11)) {
                    print_debug("ATMODE Turned on", 16, true);
                    atMode = true;
                    AT_PIN_Set();
                } else if (compare_strings(commandBuffer.buffer, "ATMODE OFF\r\n", 12)) {
                    print_debug("ATMODE Already off", 18, true);
                } else {
                    send_buffer(&commandBuffer, BLUETOOTH_UART_NUM, true);
                }
            }
            commandBuffer.tail = commandBuffer.buffer;
        }
#endif 
        rxStatus = read_line_to_buffer(&packetBuffer, BLUETOOTH_UART_NUM, 1);
        if (rxStatus == UART_READ_LINE_RECEIVED) {
            //print_debug("Received:", 9, false);
            //print_debug(packetBuffer.buffer, packetBuffer.tail - packetBuffer.buffer, false);
            //packetBuffer.tail = packetBuffer.buffer;
            uint8_t seqNum = packetBuffer.buffer[0] - ASCII_OFFSET;
            if (seqNum != receiveSeq) {
                print_debug("Incorrect Seq Num. Expected: ", 29, false);
                receiveSeq += ASCII_OFFSET;
                print_debug(&receiveSeq, 1, false);
                receiveSeq -= ASCII_OFFSET;
                print_debug(", Got: ", 7, false);
                print_debug(packetBuffer.buffer, 1, true);
            } else if (compare_strings((packetBuffer.buffer + 1), "MOV", 3)) {
                print_debug("Move Command Received:", 22, true);
                print_debug("Distance = ", 11, false);
                print_debug((packetBuffer.buffer + 4), 4, false);
                print_debug("mm", 2, true);
                
                sendSeq += ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, &sendSeq, 1, true);
                sendSeq -= ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, "ACK", 3, false);
                
                seqNum += ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, &seqNum, 1, false);
                seqNum -= ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, "\r\n", 2, false);
                
                if (receiveSeq == 1) {
                    receiveSeq = 0;
                } else {
                    receiveSeq = 1;
                }
                if (sendSeq == 3) {
                    sendSeq = 2;
                } else {
                    sendSeq = 3;
                }
                
                    send_buffer(&commandBuffer, BLUETOOTH_UART_NUM, true);
            } else if (compare_strings((packetBuffer.buffer + 1), "SPD", 3)) {
                print_debug("Speed Command Received:", 23, true);
                print_debug("Speed = ", 8, false);
                print_debug((packetBuffer.buffer + 4), 3, false);
                print_debug("mm/s", 4, true);
                
                sendSeq += ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, &sendSeq, 1, true);
                sendSeq -= ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, "ACK", 3, false);
                
                seqNum += ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, &seqNum, 1, false);
                seqNum -= ASCII_OFFSET;
                copy_to_buffer(&commandBuffer, "\r\n", 2, false);
                
                if (receiveSeq == 1) {
                    receiveSeq = 0;
                } else {
                    receiveSeq = 1;
                }
                if (sendSeq == 3) {
                    sendSeq = 2;
                } else {
                    sendSeq = 3;
                }
                
                    send_buffer(&commandBuffer, BLUETOOTH_UART_NUM, true);
            } else {
                print_debug("Invalid Command: ", 17, false);
                print_debug(packetBuffer.buffer + 1, packetBuffer.tail - packetBuffer.buffer -1, false);
            }
            packetBuffer.tail = packetBuffer.buffer;
        }
    }

    return 1;
}
*/
////////////////////////////////////////////////////
// HARDWARE PWM TEST
////////////////////////////////////////////////////
/*
int main(void) {
    // initialize the device
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    CLOCK_Initialize();
    Hardware_PWM_Initialise();
    
    Hardware_PWM_Period_Set_us(HARDWARE_PWM1_OC_NUM, 20000);
    Hardware_PWM_Period_Set_us(HARDWARE_PWM2_OC_NUM, 20000);
    Hardware_PWM_Enable(); // Start the hardware PWM timer
    Hardware_PWM_Start(HARDWARE_PWM1_OC_NUM);
    Hardware_PWM_Start(HARDWARE_PWM2_OC_NUM);
    
    uint16_t dutyCycle = 1;
    
    while (1) {
        if (dutyCycle > 1000) {
            dutyCycle = 1;
        }
        Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, dutyCycle);
        Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM2_OC_NUM, 1001 - dutyCycle);
        IO_RA3_SetPin(1);
        TMR1_Delay_ms(100);
        IO_RA3_SetPin(0);
        dutyCycle+=1;
}
 */

////////////////////////////////////////////////////
// SOFTWARE PWM TEST
////////////////////////////////////////////////////
/*
int main(void) {
    // initialize the device
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    CLOCK_Initialize();
    IO_RB14_SetDirection(PIN_DIRECTION_OUTPUT);
    IO_RB15_SetDirection(PIN_DIRECTION_OUTPUT);
    Software_PWM_Initialize();
    MS_TIMER_Initialize();
    
    //Software_PWM_Period_Set(4,20000);
    //Software_PWM_Duty_Cycle_Set(4,100);
    //Software_PWM_Disable(4);
    
    Software_PWM_Period_Set(0,10U);
    Software_PWM_Period_Set(1,10U);
    Software_PWM_Period_Set(2,10U);
    Software_PWM_Period_Set(3,10U);
    Software_PWM_Period_Set(4,10U);
    Software_PWM_Period_Set(5,10U);
    //Software_PWM_Period_Set(1,25U);
    Software_PWM_Duty_Cycle_Set(0,500);
    Software_PWM_Duty_Cycle_Set(1,500);
    Software_PWM_Duty_Cycle_Set(2,500);
    Software_PWM_Duty_Cycle_Set(3,500);
    Software_PWM_Duty_Cycle_Set(4,500);
    Software_PWM_Duty_Cycle_Set(5,500);
    //Software_PWM_Duty_Cycle_Set(0,500);
    Software_PWM_Enable(0);
    Software_PWM_Enable(1);
    Software_PWM_Enable(2);
    Software_PWM_Enable(3);
    Software_PWM_Enable(4);
    Software_PWM_Enable(5);
    //Software_PWM_Enable(1);
    Software_PWM_Start();
    
    uint16_t dutyCycle = 1;
    
    while (1) {
        //Software_PWM_Duty_Cycle_Set(4,dutyCycle);
        //IO_RA3_SetPin(1);
        //MS_TIMER_Delay_ms(500);
        //IO_RA3_SetPin(0);
        //dutyCycle+=10;
        //if (dutyCycle > 100) {
        //    dutyCycle = 1;
        //}
    }
}
 */

////////////////////////////////////////////////////
// TWO TIMER PWM TEST
////////////////////////////////////////////////////
/*
int main(void) {
    // initialize the device
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    CLOCK_Initialize();
    Two_Timers_PWM_Initialise();
    
    Two_Timers_PWM_Period_Set(100);
    Two_Timers_PWM_Duty_Cycle_Set(1);
    Two_Timers_PWM_Enable();
    
    uint16_t dutyCycle = 1;
    
    while (1) {
        Two_Timers_PWM_Duty_Cycle_Set(dutyCycle);
        IO_RA3_SetPin(1);
        TMR1_Delay_ms(100);
        IO_RA3_SetPin(0);
        dutyCycle+=1;
        
        if (dutyCycle > 100) {
            dutyCycle = 1;
        }
    }
}
 */
/**
 End of File
 */

