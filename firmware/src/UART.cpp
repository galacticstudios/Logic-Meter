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
#include "Interrupts.h"

#define UART_INTS(n) \
Implement_InterruptHandler(_UART##n##_FAULT_VECTOR, UARTInt[n - 1].fault) \
Implement_InterruptHandler(_UART##n##_RX_VECTOR, UARTInt[n - 1].receiveDone) \
Implement_InterruptHandler(_UART##n##_TX_VECTOR, UARTInt[n - 1].transferDone) \

UART_INTS(1)
UART_INTS(2)
UART_INTS(3)
UART_INTS(4)
UART_INTS(5)
UART_INTS(6)
