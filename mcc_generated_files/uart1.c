#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <xc.h>
#include "uart1.h"
#include "../buffer.h"

static uint8_t * volatile rxTail;
static uint8_t *rxHead;
static uint8_t *txTail;
static uint8_t * volatile txHead;
static bool volatile rxOverflowed;

/* We add one extra byte than requested so that we don't have to have a separate
 * bit to determine the difference between buffer full and buffer empty, but
 * still be able to hold the amount of data requested by the user.  Empty is
 * when head == tail.  So full will result in head/tail being off by one due to
 * the extra byte.
 */
#define UART1_CONFIG_TX_BYTEQ_LENGTH (2*MAX_BUFFER_SIZE+1)
#define UART1_CONFIG_RX_BYTEQ_LENGTH (2*MAX_BUFFER_SIZE+1)

static uint8_t txQueue[UART1_CONFIG_TX_BYTEQ_LENGTH];
static uint8_t rxQueue[UART1_CONFIG_RX_BYTEQ_LENGTH];

void (*UART1_TxDefaultInterruptHandler)(void);
void (*UART1_RxDefaultInterruptHandler)(void);

void UART1_Initialize(void) {
    // Disable UART1 Rx and Tx interrupts
    IEC0bits.U1TXIE = 0;
    IEC0bits.U1RXIE = 0;

    // STSEL 1; IREN disabled; PDSEL 8N; UARTEN disabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; RXINV disabled; UEN TX_RX; 
    // Data Bits = 8; Parity = None; Stop Bits = 1;
    U1MODE = 0x0008;
    // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
    U1STA = 0x00;
    // BaudRate = 9600; Frequency = 16000000 Hz; BRG 416; 
    U1BRG = 0x1A0;

    txHead = txQueue;
    txTail = txQueue;
    rxHead = rxQueue;
    rxTail = rxQueue;

    rxOverflowed = false;

    // Enable UART1 Rx interrupt
    IEC0bits.U1RXIE = 1;

    // Make sure to set LAT bit corresponding to TxPin as high before UART initialization
    U1MODEbits.UARTEN = 1; // enabling UART ON bit
    U1STAbits.UTXEN = 1;
}

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    if (txHead == txTail) {
        IEC0bits.U1TXIE = 0;
    } else {
        IFS0bits.U1TXIF = 0;

        while (!(U1STAbits.UTXBF == 1)) {
            U1TXREG = *txHead++;

            if (txHead == (txQueue + UART1_CONFIG_TX_BYTEQ_LENGTH)) {
                txHead = txQueue;
            }
            // Are we empty?
            if (txHead == txTail) {
                break;
            }
        }
    }
}

void UART1_Clear_Tx_Buffer(void) {
    txHead = txTail;
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0;

    while ((U1STAbits.URXDA == 1)) {
        *rxTail = U1RXREG;

        // Will the increment not result in a wrap and not result in a pure collision?
        // This is most often condition so check first
        if ((rxTail != (rxQueue + UART1_CONFIG_RX_BYTEQ_LENGTH - 1)) &&
                ((rxTail + 1) != rxHead)) {
            rxTail++;
        } else if ((rxTail == (rxQueue + UART1_CONFIG_RX_BYTEQ_LENGTH - 1)) &&
                (rxHead != rxQueue)) {
            // Pure wrap no collision
            rxTail = rxQueue;
        } else { // must be collision
            rxOverflowed = true;
        }
    }
}

void UART1_Clear_Rx_Buffer(void) {
    while (UART1_IsRxReady()) {
        UART1_Read();
    }
}

void __attribute__((interrupt, no_auto_psv)) _U1ErrInterrupt(void) {
    if ((U1STAbits.OERR == 1)) {
        U1STAbits.OERR = 0;
    }
    IFS4bits.U1ERIF = 0;
}

uint8_t UART1_Read(void) {
    uint8_t data = 0;

    while (rxHead == rxTail) {
    }

    data = *rxHead;

    rxHead++;

    if (rxHead == (rxQueue + UART1_CONFIG_RX_BYTEQ_LENGTH)) {
        rxHead = rxQueue;
    }
    return data;
}

void UART1_Write(uint8_t byte) {
    while (UART1_IsTxReady() == false) {
    }

    *txTail = byte;
    txTail++;

    if (txTail == (txQueue + UART1_CONFIG_TX_BYTEQ_LENGTH)) {
        txTail = txQueue;
    }
    IEC0bits.U1TXIE = 1;
}

bool UART1_IsRxReady(void) {
    return !(rxHead == rxTail);
}

bool UART1_IsTxReady(void) {
    uint16_t size;
    uint8_t *snapshot_txHead = (uint8_t*) txHead;

    if (txTail < snapshot_txHead) {
        size = (snapshot_txHead - txTail - 1);
    } else {
        size = (UART1_CONFIG_TX_BYTEQ_LENGTH - (txTail - snapshot_txHead) - 1);
    }
    return (size != 0);
}

bool UART1_IsTxDone(void) {
    if (txTail == txHead) {
        return (bool) U1STAbits.TRMT;
    }
    return false;
}