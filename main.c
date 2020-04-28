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

#define __USER_DEBUG
#define __USER_CONTROL
/**
  Section: Included Files
*/
#include "mcc_generated_files/clock.h"
#include "mcc_generated_files/oc2.h"
#define FCY _XTAL_FREQ
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/tmr2.h"
#include "libpic30.h"
#include <stdbool.h>
#include "at_control.h"
#include "uart_manager.h"
#include "util.h"

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
















int main(void)
{
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
    
    Software_PWM_Period_Set(0, 10000);
    Software_PWM_Period_Set(1, 20000);
    Software_PWM_Period_Set(2, 10000);
    Software_PWM_Period_Set(3, 10000);
    // Duty Cycles to 100%
    Software_PWM_Duty_Cycle_Set(0, 1000);
    Software_PWM_Duty_Cycle_Set(1, 1000);
    Software_PWM_Duty_Cycle_Set(2, 1000);
    Software_PWM_Duty_Cycle_Set(3, 1000);
    
    Software_PWM_Disable(0);
    Software_PWM_Disable(1);
    Software_PWM_Disable(2);
    Software_PWM_Disable(3);
    
    /*
    Software_PWM_Period_Set(0,200);
    Software_PWM_Duty_Cycle_Set(0,500);
    Software_PWM_Enable(0);
    uint16_t dutyCycle = 1;
    */
    
    
    
    PC_CLEAR_RX_BUFFER();
    PC_CLEAR_TX_BUFFER();
    uint8_t sendCommandTo = PC_UART_NUM;
    
    init_bluetooth();
    uint8_t state = BT_STATE_INVALID;
    print_debug("Waiting for connection...\r\n", 27);
    wait_for_connection();
    print_debug("Connected\r\n", 11);
    while (1)
    {
        /*
        if (dutyCycle > 1000) {
            dutyCycle = 1;
        }
        Software_PWM_Duty_Cycle_Set(0,dutyCycle);
        IO_RA3_SetPin(1);
        TMR1_Delay_ms(500);
        IO_RA3_SetPin(0);
        dutyCycle+=10;
        */
        
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
                        */ 
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
        
        /*
        TMR1_Delay_ms(1000);
        if (state) {
            Software_PWM_Disable(0);
            state = 0;
        } else {
            Software_PWM_Enable(0);
            state = 1;
        }
        /*
        if (brightness > 100) {
            brightness = 0;
        }
        //OC2_PWMPulseWidthSet(1000000);
        
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
        }
        //OC2_PWMDutyCycleSet(brightness);
        __delay_ms(1000);
        brightness += 10;
        count++;
        //IO_RA2_Toggle();
        */
    }

    return 1;
}
/**
 End of File
*/

