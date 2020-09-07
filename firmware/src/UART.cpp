/* 
 * File:   UART.cpp
 * Author: Bob
 * 
 * Created on May 25, 2020, 8:36 PM
 */

extern "C" 
{
#include "definitions.h"
}
#include "UART.h"

CUART3 UART3;

extern "C"
{
    
void __ISR(_UART3_FAULT_VECTOR, ipl1AUTO) UART3_FAULT_Handler (void)
{
    UART3_ErrorGet();

    // Clear error interrupt flag
    IFS4CLR = _IFS4_U3EIF_MASK;

    // Clear up the receive interrupt flag so that RX interrupt is not
    // triggered for error bytes
    IFS4CLR = _IFS4_U3RXIF_MASK;
}

void __ISR(_UART3_RX_VECTOR, ipl1AUTO) UART3_RX_Handler (void)
{
    UART3.RXHandler();
    // Clear UART3 RX Interrupt flag after reading data buffer
    IFS4CLR = _IFS4_U3RXIF_MASK;
}

void __ISR(_UART3_TX_VECTOR, ipl1AUTO) UART3_TX_Handler (void)
{
    UART3.TXHandler();
    // Clear UART3TX Interrupt flag
    IFS4CLR = _IFS4_U3TXIF_MASK;
}

}
