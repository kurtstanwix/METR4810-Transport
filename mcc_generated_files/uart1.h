#ifndef _UART1_H
#define _UART1_H

#include <stdbool.h>
#include <stdint.h>

/**
  @Summary
    Initializes the UART instance : 1

  @Description
    This routine initializes the UART driver instance for : 1
    index.
    This routine must be called before any other UART routine is called.
    
  @Preconditions
    None.

  @Returns
    None.

  @Param
    None.

  @Comment
    None.    
 
  @Example
    None.

*/

void UART1_Initialize(void);

/**
  @Summary
    Read a byte of data from the UART1

  @Description
    This routine reads a byte of data from the UART1.

  @Preconditions
    UART1_Initialize function should have been called 
    before calling this function. The transfer status should be checked to see 
    if the receiver is not empty before calling this function.

  @Param
    None.

  @Returns
    A data byte received by the driver.

  @Example
    None.
*/

uint8_t UART1_Read(void);

void UART1_Clear_Rx_Buffer(void);

/**
  @Summary
    Writes a byte of data to the UART1

  @Description
    This routine writes a byte of data to the UART1.

  @Preconditions
    UART1_Initialize function should have been called 
    before calling this function. The transfer status should be checked to see if
    transmitter is not full before calling this function.

  @Param
    byte         - Data byte to write to the UART1

  @Returns
    None.

  @Example
    None.
*/

void UART1_Write(uint8_t byte);

void UART1_Clear_Tx_Buffer(void);

/**
  @Description
    Indicates of there is data available to read.

  @Returns
    true if byte can be read.
    false if byte can't be read right now.
*/
bool UART1_IsRxReady(void);

/**
  @Description
    Indicates if a byte can be written.
 
 @Returns
    true if byte can be written.
    false if byte can't be written right now.
*/
bool UART1_IsTxReady(void);

/**
  @Description
    Indicates if all bytes have been transferred.
 
 @Returns
    true if all bytes transfered.
    false if there is still data pending to transfer.
*/
bool UART1_IsTxDone(void);

#endif  // _UART1_H