#include "mcc_generated_files/clock.h"
#define FCY _XTAL_FREQ
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/tmr3.h"
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
#define PACKET_LENGTH (16+2)
// 32 bytes for commands from pc then 2 for \r\n
#define COMMAND_LENGTH (64+2)


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
                position[18 - (i++)] = pos % 10 + 48;
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
                errorst[15 - (i++)] = err % 10 + 48;
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
// INTERRUPT ON CHANGE TEST
////////////////////////////////////////////////////

int main(void) {
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    CLOCK_Initialize();
    UART_Initialise();
    Hardware_PWM_Initialise();
    MS_TIMER_Initialize();
    Encoder_Initialise();
    init_debug();
    
    IO_RB1_SetDirection(PIN_DIRECTION_OUTPUT);
    
    
    
    
    
    //uint16_t setSpeedms = 6;
    //uint16_t PWMPeriod = __builtin_divsd(ENCODER_TO_LINEAR, setSpeedms);
    
    Hardware_PWM_Period_Set_us(HARDWARE_PWM1_OC_NUM, 1000);
    Hardware_PWM_Period_Set_us(HARDWARE_PWM2_OC_NUM, 0);
    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 300);
    Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM2_OC_NUM, 0);
    Hardware_PWM_Enable(); // Start the hardware PWM timer
    Hardware_PWM_Start(HARDWARE_PWM1_OC_NUM);
    Hardware_PWM_Start(HARDWARE_PWM2_OC_NUM);
    //Hardware_PWM_Period_Set_us(HARDWARE_PWM2_OC_NUM, 1500);
    //Hardware_PWM_Enable(); // Start the hardware PWM timer
    //Hardware_PWM_Start(HARDWARE_PWM1_OC_NUM);
    
    int32_t shaftVel;
    bool neg;
    char speed[15] = "Speed:       \r\n";
    uint8_t i;
    uint8_t count = 0;
    while (1) {
        speed[12] = 0;
        speed[11] = 0;
        speed[10] = 0;
        speed[9] = 0;
        speed[8] = 0;
        speed[7] = 0;
        speed[6] = 0;
        shaftVel = Encoder_1_shaft_revs();//Encoder_1_get_shaft_ang_speed();
        i = 0;
        neg = shaftVel < 0;
        i = 0;
        if (shaftVel) {
            while (shaftVel) {
                speed[12 - (i++)] = (shaftVel * (neg ? -1: 1)) % 10 + 48;
                shaftVel /= 10;
            }
            if (neg) {
                speed[12 - i] = '-';
            }
        } else {
            speed[12] = '0';
        }
        print_debug(speed, 15);
        MS_TIMER_Delay_ms(500);
        count++;
        if (count == 10) {
            Hardware_PWM_Duty_Cycle_Set(HARDWARE_PWM1_OC_NUM, 0);
            
        }
        //IO_RB4_Toggle();
        //setSpeedms++;
        //PWMPeriod = __builtin_divsd(ENCODER_TO_LINEAR, setSpeedms);
        //Hardware_PWM_Period_Set_us(HARDWARE_PWM1_OC_NUM, PWMPeriod);
        //MS_TIMER_Delay_ms(50);
    }
}


////////////////////////////////////////////////////
// BLUETOOTH PWM TEST
////////////////////////////////////////////////////
/*
int main(void) {
    // initialize the device
    SYSTEM_Initialize();
    
    bool atMode = false;
    AT_PIN_Clear();
    
    BUFFER_OBJ packetBuffer;
    init_buffer(&packetBuffer, COMMAND_LENGTH);
    
    BUFFER_OBJ commandBuffer;
    init_buffer(&commandBuffer, COMMAND_LENGTH);
    
    uint8_t data = 0;
    uint8_t rxStatus = UART_RX_STATUS_MORE;
        
    PC_CLEAR_RX_BUFFER();
    PC_CLEAR_TX_BUFFER();
    uint8_t sendCommandTo = PC_UART_NUM;
    
    init_bluetooth();
    
    print_debug("Waiting for connection...\r\n", 27);
    wait_for_connection();
    print_debug("Connected\r\n", 11);
    
    while (1) {        
       if (!BT_IS_CONNECTED()) {
            print_debug("Lost Connection\n\rReconnecting...\r\n", 34);
            wait_for_connection();
            print_debug("Reconnected\r\n", 13);
        }
        
        
        
#ifdef __USER_CONTROL
//commandBuffer.tail = commandBuffer.buffer;
        // Allows for the user to send 
        rxStatus = read_line_to_buffer(&commandBuffer, PC_UART_NUM, 1);
            if (rxStatus == UART_READ_LINE_RECEIVED) {
                if (atMode) {
                    if (compare_strings(commandBuffer.buffer, "ATMODE OFF\r\n", 12)) {
                        print_debug("ATMODE Turned off\r\n", 19);
                        atMode = false;
                        AT_PIN_Clear();
                    } else if (compare_strings(commandBuffer.buffer, "ATMODE OFF\r\n", 12)) {
                        print_debug("ATMODE Already on\r\n", 19);
                    } else if (compare_strings(commandBuffer.buffer, "RESET\r\n", 7)) {
                        reset_bluetooth();
                        atMode = false;
                        print_debug("BT Reset\r\nATMODE Turned Off\r\n", 29);
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
                        print_debug(commandBuffer.buffer, commandBuffer.tail - commandBuffer.buffer);
                    }
                } else {
                    if (compare_strings(commandBuffer.buffer, "ATMODE ON\r\n", 11)) {
                        print_debug("ATMODE Turned on\r\n", 18);
                        atMode = true;
                        AT_PIN_Set();
                    } else if (compare_strings(commandBuffer.buffer, "ATMODE OFF\r\n", 12)) {
                        print_debug("ATMODE Already off\r\n", 20);
                    } else {
                        send_buffer(&commandBuffer, BLUETOOTH_UART_NUM, true);
                    }
                }
                commandBuffer.tail = commandBuffer.buffer;
            }
#endif 
        rxStatus = read_line_to_buffer(&packetBuffer, BLUETOOTH_UART_NUM, 1);
        if (rxStatus == UART_READ_LINE_RECEIVED) {
            print_debug("Received:", 9);
            print_debug(packetBuffer.buffer, packetBuffer.tail - packetBuffer.buffer);
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

