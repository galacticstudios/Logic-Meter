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

#define SPIIMPL(N) \
\
CSPI##N *CSPI##N::_spiObj; \
\
extern "C" \
{ \
\
void __ISR(_SPI##N##_FAULT_VECTOR, ipl1AUTO) SPI##N##_FAULT_Handler (void) \
{ \
    SPI##N##STATCLR = 1 << 12; \
\
    if (CSPI##N::_spiObj) \
    { \
        /* Clear error interrupt flag */ \
        CSPI##N::_spiObj->FaultIFSCLR = CSPI##N::IFS_FaultIF_MASK; \
\
        /* Clear up the receive interrupt flag so that RX interrupt is not \
           triggered for error bytes */ \
        CSPI##N::_spiObj->RXIFSCLR = CSPI##N::IFS_RXIF_MASK; \
    } \
} \
\
void __ISR(_SPI##N##_RX_VECTOR, ipl1AUTO) SPI##N##_RX_Handler (void) \
{ \
    if (CSPI##N::_spiObj) \
    { \
        CSPI##N::_spiObj->RXHandler(); \
        /* Clear SPI RX Interrupt flag after reading data buffer */ \
        CSPI##N::_spiObj->RXIFSCLR = CSPI##N::IFS_RXIF_MASK; \
    } \
} \
\
void __ISR(_SPI##N##_TX_VECTOR, ipl1AUTO) SPI##N##_TX_Handler (void) \
{ \
    if (CSPI##N::_spiObj) \
    { \
        CSPI##N::_spiObj->TXHandler(); \
        /* Clear SPI TX Interrupt flag */ \
        CSPI##N::_spiObj->CSPI##N::IFS_TXIF_MASK; \
    } \
} \
\
}

SPIIMPL(2)
SPIIMPL(4)
