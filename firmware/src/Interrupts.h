/* 
 * File:   Interrupts.h
 * Author: Bob
 *
 * Created on September 14, 2020, 9:30 AM
 */

#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

#include "Peripherals.h"

struct IntMeta
{
    IntMeta(uint32_t intNo) :
        irqNumber(intNo), flag{*(&IFS0 + (&IFS1 - &IFS0) * (intNo / 32)), uint8_t(intNo % 32), uint8_t(intNo %32)},
        enable{*(&IEC0 + (&IEC1 - &IEC0) * (intNo / 32)), uint8_t(intNo % 32), uint8_t(intNo %32)},
        priority{*(&IPC0 + (&IPC1 - &IPC0) * (intNo / 4)), uint8_t(intNo * 8 % 32 + 4), uint8_t(intNo * 8 % 32 + 2)},
        subpriority{*(&IPC0 + (&IPC1 - &IPC0) * (intNo / 4)), uint8_t(intNo * 8 % 32 + 1), uint8_t(intNo * 8 % 32 + 0)}
        {}
    uint32_t irqNumber;
    RegisterBF flag;
    RegisterBF enable;
    RegisterBF priority;
    RegisterBF subpriority;
};

struct SPIIntMeta
{
    IntMeta fault;
    IntMeta receiveDone;
    IntMeta transferDone;
};

extern const SPIIntMeta SPIInt[];

struct UARTIntMeta
{
    IntMeta fault;
    IntMeta receiveDone;
    IntMeta transferDone;
};

extern const UARTIntMeta UARTInt[];

struct TMRIntMeta
{
    IntMeta timer;
};

extern const TMRIntMeta TMRInt[];

struct InputCaptureIntMeta
{
    IntMeta capture;
    IntMeta error;
};

extern const InputCaptureIntMeta InputCaptureInt[];

struct DMAIntMeta
{
    IntMeta dma;
};

extern const DMAIntMeta DMAInt[];

// Use this macro to define an interrupt handling routine. It takes two arguments,
// a vector name (which Microchip declares in their microcontroller .h files),
// and the interrupt's IntMeta struct
// 
// E.g.:
//    Implement_InterruptHandler(_SPI1_FAULT_VECTOR, SPIInt[0].fault)
//
// It creates the PIC32 interrupt handler, which calls the function pointer's
// target (if it's not null) and clears the interrupt flag.
//
#define Implement_InterruptHandler(v, intMeta) \
extern "C" void __ISR(v, ipl1AUTO) PIC32_##v##Handler() \
{ \
    if (userInterruptHandlers[v].fn) \
        (*userInterruptHandlers[v].fn)(userInterruptHandlers[v].context); \
    intMeta.flag = 0; \
}

typedef struct
{
    void (*fn)(void *context);
    void *context;
} UserInterruptHandler;
extern UserInterruptHandler userInterruptHandlers[];

inline void SetInterruptHandler(int vectorNumber, void (*fn)(void *context), void *context)
{
    userInterruptHandlers[vectorNumber].context = context; 
    userInterruptHandlers[vectorNumber].fn = fn;
}

inline void ClearInterruptHandler(int vectorNumber)
{
    userInterruptHandlers[vectorNumber].fn = nullptr;
}

#endif	/* INTERRUPTS_H */

