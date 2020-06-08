#ifndef _UART2_H
#define _UART2_H

#include <stdbool.h>
#include <stdint.h>

/**
  @Summary
    Initializes the UART instance : 2

  @Description
    This routine initializes the UART driver instance for : 2
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

void UART2_Initialize(void);

/**
  @Summary
    Read a byte of data from the UART2

  @Description
    This routine reads a byte of data from the UART2.

  @Preconditions
    UART2_Initialize function should have been called 
    before calling this function. The transfer status should be checked to see 
    if the receiver is not empty before calling this function.

  @Param
    None.

  @Returns
    A data byte received by the driver.

  @Example
    None.
*/

uint8_t UART2_Read( void);

void UART2_Clear_Rx_Buffer(void);

/**
  @Summary
    Writes a byte of data to the UART2

  @Description
    This routine writes a byte of data to the UART2.

  @Preconditions
    UART2_Initialize function should have been called 
    before calling this function. The transfer status should be checked to see if
    transmitter is not full before calling this function.

  @Param
    byte         - Data byte to write to the UART2

  @Returns
    None.

  @Example
    None.
*/

void UART2_Write( uint8_t byte);

void UART2_Clear_Tx_Buffer(void);


/**
  @Description
    Indicates of there is data available to read.

  @Returns
    true if byte can be read.
    false if byte can't be read right now.
*/
bool UART2_IsRxReady(void);

/**
  @Description
    Indicates if a byte can be written.
 
 @Returns
    true if byte can be written.
    false if byte can't be written right now.
*/
bool UART2_IsTxReady(void);

/**
  @Description
    Indicates if all bytes have been transferred.
 
 @Returns
    true if all bytes transfered.
    false if there is still data pending to transfer.
*/
bool UART2_IsTxDone(void);

/**
  @Summary
    Assigns a function pointer with a transmit callback address.

  @Description
    This routine assigns a function pointer with a transmit callback address.

  @Param
    Address of the callback routine.

  @Returns
    None
 
  @Example 
    <code>
        UART2_SetTxInterruptHandler(&UART2_Transmit_CallBack);
    </code>
*/
void UART2_SetTxInterruptHandler(void* handler);

/**
  @Summary
    Transmit callback routine.

  @Description
    This routine is a transmit callback function.

  @Param
    None.

  @Returns
    None
 
  @Example 
    <code>
        UART2_SetTxInterruptHandler(&UART2_Transmit_CallBack);
    </code>
*/
void UART2_Transmit_CallBack(void);

/**
  @Summary
    Assigns a function pointer with a receive callback address.

  @Description
    This routine assigns a function pointer with a receive callback address.

  @Param
    Address of the callback routine.

  @Returns
    None
 
  @Example 
    <code>
        UART2_SetRxInterruptHandler(&UART2_Receive_CallBack);
    </code>
*/
void UART2_SetRxInterruptHandler(void* handler);

/**
  @Summary
    Receive callback routine.

  @Description
    This routine is a receive callback function.

  @Param
    None.

  @Returns
    None
 
  @Example 
    <code>
        UART2_SetTxInterruptHandler(&UART2_Receive_CallBack);
    </code>
*/
void UART2_Receive_CallBack(void);
 
#endif  // _UART2_H