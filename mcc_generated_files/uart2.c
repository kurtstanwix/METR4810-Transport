#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "xc.h"
#include "uart2.h"

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
#define UART2_CONFIG_TX_BYTEQ_LENGTH (32+1)
#define UART2_CONFIG_RX_BYTEQ_LENGTH (32+1)

static uint8_t txQueue[UART2_CONFIG_TX_BYTEQ_LENGTH];
static uint8_t rxQueue[UART2_CONFIG_RX_BYTEQ_LENGTH];

void (*UART2_TxDefaultInterruptHandler)(void);
void (*UART2_RxDefaultInterruptHandler)(void);

void UART2_Initialize(void) {
    IEC1bits.U2TXIE = 0;
    IEC1bits.U2RXIE = 0;

    // STSEL 1; IREN disabled; PDSEL 8N; UARTEN disabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; RXINV disabled; UEN TX_RX; 
    // Data Bits = 8; Parity = None; Stop Bits = 1;
    U2MODE = 0x0008;
    // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
    U2STA = 0x00;
    // BaudRate = 9600; Frequency = 16000000 Hz; BRG 416; 
    U2BRG = 0x1A0;

    txHead = txQueue;
    txTail = txQueue;
    rxHead = rxQueue;
    rxTail = rxQueue;

    rxOverflowed = false;

    IEC1bits.U2RXIE = 1;

    //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
    U2MODEbits.UARTEN = 1; // enabling UART ON bit
    U2STAbits.UTXEN = 1;
}

void __attribute__((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
    if (txHead == txTail) {
        IEC1bits.U2TXIE = 0;
    } else {
        IFS1bits.U2TXIF = 0;

        while (!(U2STAbits.UTXBF == 1)) {
            U2TXREG = *txHead++;

            if (txHead == (txQueue + UART2_CONFIG_TX_BYTEQ_LENGTH)) {
                txHead = txQueue;
            }
            // Are we empty?
            if (txHead == txTail) {
                break;
            }
        }
    }
}

void UART2_Clear_Tx_Buffer(void) {
    txHead = txTail;
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt(void) {
    IFS1bits.U2RXIF = 0;

    while ((U2STAbits.URXDA == 1)) {
        *rxTail = U2RXREG;

        // Will the increment not result in a wrap and not result in a pure collision?
        // This is most often condition so check first
        if ((rxTail != (rxQueue + UART2_CONFIG_RX_BYTEQ_LENGTH - 1)) &&
                ((rxTail + 1) != rxHead)) {
            rxTail++;
        }
        else if ((rxTail == (rxQueue + UART2_CONFIG_RX_BYTEQ_LENGTH - 1)) &&
                (rxHead != rxQueue)) {
            // Pure wrap no collision
            rxTail = rxQueue;
        }
        else { // must be collision
            rxOverflowed = true;
        }
    }
}

void UART2_Clear_Rx_Buffer(void) {
    while (UART2_IsRxReady()) {
        UART2_Read();
    }
}

void __attribute__((interrupt, no_auto_psv)) _U2ErrInterrupt(void) {
    if ((U2STAbits.OERR == 1)) {
        U2STAbits.OERR = 0;
    }

    IFS4bits.U2ERIF = 0;
}

uint8_t UART2_Read(void) {
    uint8_t data = 0;

    while (rxHead == rxTail) {
    }

    data = *rxHead;

    rxHead++;

    if (rxHead == (rxQueue + UART2_CONFIG_RX_BYTEQ_LENGTH)) {
        rxHead = rxQueue;
    }
    return data;
}

void UART2_Write(uint8_t byte) {
    while (UART2_IsTxReady() == 0) {
    }

    *txTail = byte;

    txTail++;

    if (txTail == (txQueue + UART2_CONFIG_TX_BYTEQ_LENGTH)) {
        txTail = txQueue;
    }

    IEC1bits.U2TXIE = 1;
}

bool UART2_IsRxReady(void) {
    return !(rxHead == rxTail);
}

bool UART2_IsTxReady(void) {
    uint16_t size;
    uint8_t *snapshot_txHead = (uint8_t*) txHead;

    if (txTail < snapshot_txHead) {
        size = (snapshot_txHead - txTail - 1);
    } else {
        size = (UART2_CONFIG_TX_BYTEQ_LENGTH - (txTail - snapshot_txHead) - 1);
    }

    return (size != 0);
}

bool UART2_IsTxDone(void) {
    if (txTail == txHead) {
        return (bool) U2STAbits.TRMT;
    }

    return false;
}