#ifndef UART_MANAGER_H
#define	UART_MANAGER_H

#include "buffer.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"

#define BLUETOOTH_UART_NUM 1
#define PC_UART_NUM 2

#define UART_FUNC_WRAPPER(UART_NUM, FUNC) UART ## UART_NUM ## _ ## FUNC

#define UART_WRITE(UART_NUM, DATA) UART_FUNC_WRAPPER(UART_NUM, Write(DATA))
#define UART_READ(UART_NUM) UART_FUNC_WRAPPER(UART_NUM, Read())
#define UART_RX_READY(UART_NUM) UART_FUNC_WRAPPER(UART_NUM, IsRxReady())
#define UART_CLEAR_RX_BUFFER(UART_NUM) UART_FUNC_WRAPPER(UART_NUM, Clear_Rx_Buffer())
#define UART_CLEAR_TX_BUFFER(UART_NUM) UART_FUNC_WRAPPER(UART_NUM, Clear_Tx_Buffer())
#define UART_INITIALISE(UART_NUM) UART_FUNC_WRAPPER(UART_NUM, Initialize())

#ifdef BLUETOOTH_UART_NUM
#define BLUETOOTH_WRITE(DATA) UART_WRITE(BLUETOOTH_UART_NUM, DATA)
#define BLUETOOTH_READ() UART_READ(BLUETOOTH_UART_NUM)
#define BLUETOOTH_RX_READY() UART_RX_READY(BLUETOOTH_UART_NUM)
#define BLUETOOTH_CLEAR_RX_BUFFER() UART_CLEAR_RX_BUFFER(BLUETOOTH_UART_NUM)
#define BLUETOOTH_CLEAR_TX_BUFFER() UART_CLEAR_TX_BUFFER(BLUETOOTH_UART_NUM)
#define BLUETOOTH_INITIALISE() UART_INITIALISE(BLUETOOTH_UART_NUM)
#endif

#ifdef PC_UART_NUM
#define PC_WRITE(DATA) UART_WRITE(PC_UART_NUM, DATA)
#define PC_READ() UART_READ(PC_UART_NUM)
#define PC_RX_READY() UART_RX_READY(PC_UART_NUM)
#define PC_CLEAR_RX_BUFFER() UART_CLEAR_RX_BUFFER(PC_UART_NUM)
#define PC_CLEAR_TX_BUFFER() UART_CLEAR_TX_BUFFER(PC_UART_NUM)
#define PC_INITIALISE() UART_INITIALISE(PC_UART_NUM)
#endif

// Buffer not full and no EOL received, keep waiting
#define UART_RX_STATUS_MORE 0
// End of line characters (CR+LF) received
#define UART_RX_STATUS_EOL 1
// Buffer was full before EOL received
#define UART_RX_STATUS_BF 2

#define UART_READ_LINE_NOTHING 0
#define UART_READ_LINE_PARTIAL 1
#define UART_READ_LINE_RECEIVED 2

void UART_Initialise(void);

uint8_t read_to_buffer(BUFFER_OBJ *buffer, uint8_t uartNum);

uint8_t read_line_to_buffer(BUFFER_OBJ *buffer, uint8_t uartNum, uint16_t timeout);

void send_buffer(BUFFER_OBJ *buffer, uint8_t uartNum, bool clearBuffer);

#endif

