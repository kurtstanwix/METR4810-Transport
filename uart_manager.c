#include "ms_timer.h"
#include "uart_manager.h"
#include "mcc_generated_files/pin_manager.h"

/**
 * Initialise the UART Modules based on if Bluetooth and PC have been defined
 */
void UART_Initialise(void) {
#ifdef BLUETOOTH_UART_NUM
    BLUETOOTH_INITIALISE();
#endif
#ifdef PC_UART_NUM
    PC_INITIALISE();
#endif
}

/**
 * Read a byte from the UART into the buffer
 * @param buffer buffer to read the byte into
 * @param uartNum UART to read the byte from
 * @return the status based on whether data was read or an EOL was read
 */
uint8_t read_to_buffer(BUFFER_OBJ *buffer, uint8_t uartNum) {
    *(buffer->tail) = uartNum == BLUETOOTH_UART_NUM ? BLUETOOTH_READ() : PC_READ();
    buffer->tail++;
    if (*(buffer->tail - 1) == '\r') {
        // Got an \r check for an \n
        *(buffer->tail) = uartNum == BLUETOOTH_UART_NUM ? BLUETOOTH_READ() : PC_READ();
        buffer->tail++;
        if (*(buffer->tail - 1) == '\n') {
            return UART_RX_STATUS_EOL;
        } else { // Only \r received, remove it
            *(buffer->tail - 2) = *(buffer->tail - 1);
            buffer->tail--;
        }
    }
    if (buffer->tail == buffer->buffer + buffer->size - 2) {
        // Maximum size of buffer reached without receiving \r\n
        *(buffer->tail++) = '\r';
        *(buffer->tail++) = '\n';
        return UART_RX_STATUS_BF;
    }
    return UART_RX_STATUS_MORE;
}

/**
 * Reads a \r\n terminated line from the specified UART. Upon return, any read
 * data will be stored in buffer with the return indicating the state of the
 * data. If the buffer fills before a new line is read, \r\n will be appended
 * the status will indicate a full line received.
 * @param buffer The buffer object to read the UART data into, continuing from
 *               tail of it.
 * @param uartNum The UART number to read from, must have been initialised.
 * @param timeout Time in ms before reading returns if a full line isn't read. 0
 *                for no timeout, will only return once a line has been read.
 * @return Status of the read operation:
 *          - UART_READ_LINE_NOTHING timed out before any data was received
 *          - UART_READ_LINE_PARTIAL timed out before a full line was received
 *          - UART_READ_LINE_RECEIVED a full line was received or buffer full
 */
uint8_t read_line_to_buffer(BUFFER_OBJ *buffer, uint8_t uartNum, uint16_t timeout) {
    uint8_t rxStatus;
    uint8_t status = UART_READ_LINE_NOTHING;
    uint16_t elapsed = 0;
    while (true) {
        if (!(uartNum == BLUETOOTH_UART_NUM ? BLUETOOTH_RX_READY() : PC_RX_READY())) {
            MS_TIMER_Delay_ms(1);
            elapsed++;
            if (timeout && elapsed >= timeout) {
                break; // Timed out without reading line
            }
            continue;
        }
        // Byte ready to read
        rxStatus = read_to_buffer(buffer, uartNum);
        status = UART_READ_LINE_PARTIAL;
        
        if (rxStatus == UART_RX_STATUS_EOL || rxStatus == UART_RX_STATUS_BF) {
            status = UART_READ_LINE_RECEIVED; // Got a line
            break;
        }
    }
    return status;
}

/**
 * Send the buffer contents to the specified uartNum
 * @param buffer buffer with the contents in it to send
 * @param uartNum UART to send contents to
 * @param clearBuffer if true, will clear the buffer after sending
 */
void send_buffer(BUFFER_OBJ *buffer, uint8_t uartNum, bool clearBuffer) {
    uint8_t *sendPos = buffer->buffer;
    while (sendPos != buffer->tail) {
        // Write bytes until end of buffer
        uartNum == BLUETOOTH_UART_NUM ? BLUETOOTH_WRITE(*sendPos) : PC_WRITE(*sendPos);
        sendPos++;
    }
    if (clearBuffer) {
        // Clear buffer is simply resetting the tail
        buffer->tail = buffer->buffer;
    }
}