/*
 * File:   uart_manager.c
 * Author: Kurt
 *
 * Created on April 12, 2020, 9:22 AM
 */

#include "mcc_generated_files/tmr2.h"
#include "mcc_generated_files/tmr1.h"
#include "uart_manager.h"


void UART_Initialise(void) {
#ifdef BLUETOOTH_UART_NUM
    BLUETOOTH_INITIALISE();
#endif
#ifdef PC_UART_NUM
    PC_INITIALISE();
#endif
}

// Read a given uart byte into the buffer
uint8_t read_to_buffer(BUFFER_OBJ *buffer, uint8_t uartNum) {
    Software_PWM_Enable((uartNum - 1) * 2);
    *(buffer->tail) = uartNum == BLUETOOTH_UART_NUM ? BLUETOOTH_READ() : PC_READ();
    buffer->tail++;
    if (*(buffer->tail - 1) == '\r') {
        *(buffer->tail) = uartNum == BLUETOOTH_UART_NUM ? BLUETOOTH_READ() : PC_READ();
        buffer->tail++;
        if (*(buffer->tail - 1) == '\n') {
            Software_PWM_Disable((uartNum - 1) * 2);
            return UART_RX_STATUS_EOL;
            //send_buffer(&packetBuffer, PC_UART_NUM, true);
        } else { // Only \r received, remove it
            *(buffer->tail - 2) = *(buffer->tail - 1);
            buffer->tail--;
        }
    }
    if (buffer->tail == buffer->buffer + buffer->size - 2) {
        *(buffer->tail++) = '\r';
        *(buffer->tail++) = '\n';
        Software_PWM_Disable((uartNum - 1) * 2);
        return UART_RX_STATUS_BF;
        //send_buffer(&packetBuffer, PC_UART_NUM, true);
    }
    Software_PWM_Disable((uartNum - 1) * 2);
    return UART_RX_STATUS_MORE;
}

// timeout = 0 for no time out
uint8_t read_line_to_buffer(BUFFER_OBJ *buffer, uint8_t uartNum, uint16_t timeout) {
    uint8_t rxStatus;
    uint8_t status = UART_READ_LINE_NOTHING;
    uint16_t elapsed = 0;
    while (true) {
        if (!(uartNum == BLUETOOTH_UART_NUM ? BLUETOOTH_RX_READY() : PC_RX_READY())) {
            TMR1_Delay_ms(1);
            elapsed++;
            if (timeout && elapsed >= timeout) {
                break;
            }
            continue;
        }
        rxStatus = read_to_buffer(buffer, uartNum);
        status = UART_READ_LINE_PARTIAL;
        
        if (rxStatus == UART_RX_STATUS_EOL || rxStatus == UART_RX_STATUS_BF) {
            //flag = true;
            status = UART_READ_LINE_RECEIVED;
            break;
        }
    }
    return status;
}

// Send the buffer contents to the specified 
void send_buffer(BUFFER_OBJ *buffer, uint8_t uartNum, uint8_t clearBuffer) {
    uint8_t *sendPos = buffer->buffer;
    Software_PWM_Enable((uartNum - 1) * 2 + 1);
    while (sendPos != buffer->tail) {
        uartNum == BLUETOOTH_UART_NUM ? BLUETOOTH_WRITE(*sendPos) : PC_WRITE(*sendPos);
        sendPos++;
    }
    Software_PWM_Disable((uartNum - 1) * 2 + 1);
    if (clearBuffer) {
        buffer->tail = buffer->buffer;
    }
}