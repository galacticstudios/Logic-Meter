/* 
 * File:   SPI.cpp
 * Author: Bob
 * 
 * Created on July 11, 2020, 1:56 PM
 */

extern "C" 
{
#include "definitions.h"
}
#include "SPI.h"

#define SPI_INTS(n) \
Implement_InterruptHandler(_SPI##n##_FAULT_VECTOR, SPIInt[n - 1].fault) \
Implement_InterruptHandler(_SPI##n##_RX_VECTOR, SPIInt[n - 1].receiveDone) \
Implement_InterruptHandler(_SPI##n##_TX_VECTOR, SPIInt[n - 1].transferDone) \

SPI_INTS(1)
SPI_INTS(2)
SPI_INTS(3)
SPI_INTS(4)
#ifdef _SPI5_FAULT_VECTOR
SPI_INTS(5)
#endif
#ifdef _SPI6_FAULT_VECTOR
SPI_INTS(6)
#endif
