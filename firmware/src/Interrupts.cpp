/* 
 * File:   Interrupts.cpp
 * Author: Bob
 * 
 * Created on September 14, 2020, 9:30 AM
 */

extern "C"
{
#include "definitions.h"
}
#include "Interrupts.h"

const SPIIntMeta SPIInt[] = 
{
#if defined(__32MZ2048EFH064__)
    // SPI1
    {
        // Fault
        IntMeta(_SPI1_FAULT_VECTOR),
        // Receive Done
        IntMeta(_SPI1_RX_VECTOR),
        // Transfer Done
        IntMeta(_SPI1_TX_VECTOR),
    },
    // SPI2
    {
        // Fault
        IntMeta(_SPI2_FAULT_VECTOR),
        // Receive Done
        IntMeta(_SPI2_RX_VECTOR),
        // Transfer Done
        IntMeta(_SPI2_TX_VECTOR),
    },
    // SPI3
    {
        // Fault
        IntMeta(_SPI3_FAULT_VECTOR),
        // Receive Done
        IntMeta(_SPI3_RX_VECTOR),
        // Transfer Done
        IntMeta(_SPI3_TX_VECTOR),
    },
    // SPI4
    {
        // Fault
        IntMeta(_SPI4_FAULT_VECTOR),
        // Receive Done
        IntMeta(_SPI4_RX_VECTOR),
        // Transfer Done
        IntMeta(_SPI4_TX_VECTOR),
    },
    
#else
#error No SPI Interrupt metadata defined for this microcontroller
#endif
};

const UARTIntMeta UARTInt[] = 
{
#if defined(__32MZ2048EFH064__)
    // UART1
    {
        // Fault
        IntMeta(_UART1_FAULT_VECTOR),
        // Receive Done
        IntMeta(_UART1_RX_VECTOR),
        // Transfer Done
        IntMeta(_UART1_TX_VECTOR),
    },
    // UART2
    {
        // Fault
        IntMeta(_UART2_FAULT_VECTOR),
        // Receive Done
        IntMeta(_UART2_RX_VECTOR),
        // Transfer Done
        IntMeta(_UART2_TX_VECTOR),
    },
    // UART3
    {
        // Fault
        IntMeta(_UART3_FAULT_VECTOR),
        // Receive Done
        IntMeta(_UART3_RX_VECTOR),
        // Transfer Done
        IntMeta(_UART3_TX_VECTOR),
    },
    // UART4
    {
        // Fault
        IntMeta(_UART4_FAULT_VECTOR),
        // Receive Done
        IntMeta(_UART4_RX_VECTOR),
        // Transfer Done
        IntMeta(_UART4_TX_VECTOR),
    },
     // UART5
    {
        // Fault
        IntMeta(_UART5_FAULT_VECTOR),
        // Receive Done
        IntMeta(_UART5_RX_VECTOR),
        // Transfer Done
        IntMeta(_UART5_TX_VECTOR),
    },
    // UART6
    {
        // Fault
        IntMeta(_UART6_FAULT_VECTOR),
        // Receive Done
        IntMeta(_UART6_RX_VECTOR),
        // Transfer Done
        IntMeta(_UART6_TX_VECTOR),
    },
   
#else
#error No UART Interrupt metadata defined for this microcontroller
#endif
};


const TMRIntMeta TMRInt[] = 
{
#if defined(__32MZ2048EFH064__)
    {
        IntMeta(_TIMER_1_VECTOR),
    },
    {
        IntMeta(_TIMER_2_VECTOR),
    },
    {
        IntMeta(_TIMER_3_VECTOR),
    },
    {
        IntMeta(_TIMER_4_VECTOR),
    },
    {
        IntMeta(_TIMER_5_VECTOR),
    },
    {
        IntMeta(_TIMER_6_VECTOR),
    },
    {
        IntMeta(_TIMER_7_VECTOR),
    },
    {
        IntMeta(_TIMER_8_VECTOR),
    },
    {
        IntMeta(_TIMER_9_VECTOR),
    },
    
#else
#error No Timer Interrupt metadata defined for this microcontroller
#endif
};

const InputCaptureIntMeta InputCaptureInt[] = 
{
#if defined(__32MZ2048EFH064__)
    {
        IntMeta(_INPUT_CAPTURE_1_VECTOR),
        IntMeta(_INPUT_CAPTURE_1_ERROR_VECTOR),
    },
    {
        IntMeta(_INPUT_CAPTURE_2_VECTOR),
        IntMeta(_INPUT_CAPTURE_2_ERROR_VECTOR),
    },
    {
        IntMeta(_INPUT_CAPTURE_3_VECTOR),
        IntMeta(_INPUT_CAPTURE_3_ERROR_VECTOR),
    },
    {
        IntMeta(_INPUT_CAPTURE_4_VECTOR),
        IntMeta(_INPUT_CAPTURE_4_ERROR_VECTOR),
    },
    {
        IntMeta(_INPUT_CAPTURE_5_VECTOR),
        IntMeta(_INPUT_CAPTURE_5_ERROR_VECTOR),
    },
    {
        IntMeta(_INPUT_CAPTURE_6_VECTOR),
        IntMeta(_INPUT_CAPTURE_6_ERROR_VECTOR),
    },
    
#else
#error No Input Capture Interrupt metadata defined for this microcontroller
#endif
};

const DMAIntMeta DMAInt[] = 
{
#if defined(__32MZ2048EFH064__)
    // DMA0
    {
        IntMeta(_DMA0_VECTOR),
    },
    // DMA1
    {
        IntMeta(_DMA1_VECTOR),
    },
    // DMA2
    {
        IntMeta(_DMA2_VECTOR),
    },
    // DMA3
    {
        IntMeta(_DMA3_VECTOR),
    },
    // DMA4
    {
        IntMeta(_DMA4_VECTOR),
    },
    // DMA5
    {
        IntMeta(_DMA5_VECTOR),
    },
    // DMA6
    {
        IntMeta(_DMA6_VECTOR),
    },
    // DMA7
    {
        IntMeta(_DMA7_VECTOR),
    },
    
#else
#error No DMA Interrupt metadata defined for this microcontroller
#endif
};

UserInterruptHandler userInterruptHandlers[213];
