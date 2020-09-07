/* 
 * File:   UART.h
 * Author: Bob
 *
 * Created on May 25, 2020, 8:36 PM
 */

#ifndef UART_H
#define	UART_H

#include "PPS.h"

// Use #defines to make "UART3" symbols for the registers that contain UART3 info
// (e.g. IEC1SET has some UART3-related bits). This allows us to use preprocessor
// tokenizing (##) to assemble the register names based on the UART number

#if defined(__32MK1024MCF064__)

#define U3RXPPS PPSGroup1Inputs

#define UART3RXIECSET IEC1SET
#define UART3RXIECCLR IEC1CLR
#define UART3IEC_RXIE_MASK _IEC1_U3RXIE_MASK
#define UART3TXIECSET IEC2SET
#define UART3TXIECCLR IEC2CLR
#define UART3IEC_TXIE_MASK _IEC2_U3TXIE_MASK
#define UART3RXIPCbits IPC15bits
#define UART3EIPCbits IPC15bits
#define UART3TXIPCbits IPC16bits

#elif defined(__32MZ2048EFH064__)

#define U3RXPPS PPSGroup2Inputs

#define UART3RXIECSET IEC4SET
#define UART3RXIECCLR IEC4CLR
#define UART3IEC_RXIE_MASK _IEC4_U3RXIE_MASK
#define UART3TXIECSET IEC4SET
#define UART3TXIECCLR IEC4CLR
#define UART3IEC_TXIE_MASK _IEC4_U3TXIE_MASK
#define UART3RXIPCbits IPC39bits
#define UART3EIPCbits IPC39bits
#define UART3TXIPCbits IPC39bits

#else

#error UART registers not defined for this microcontroller

#endif

#define UARTCLASS(N) \
class CUART##N \
{ \
public: \
    CUART##N() : \
        _readCallback(NULL), _readContext(NULL), _writeCallback(NULL), _writeContext(NULL) \
        {} \
    ~CUART##N() {} \
    \
    void Initialize() {UART##N##_Initialize();} \
    bool SerialSetup(UART_SERIAL_SETUP *setup, uint32_t srcClkFreq) \
    { \
       UART##N##_SerialSetup(setup, srcClkFreq); \
    } \
    \
    void Disable() {U##N##MODECLR = _U3MODE_ON_MASK;} \
    \
    void EnableRXInterrupt() {UART##N##RXIECSET = UART##N##IEC_RXIE_MASK;} \
    void DisableRXInterrupt() {UART##N##RXIECCLR = UART##N##IEC_RXIE_MASK;} \
    \
    void EnableTXInterrupt() {UART##N##TXIECSET = UART##N##IEC_TXIE_MASK;} \
    void DisableTXInterrupt() {UART##N##TXIECCLR = UART##N##IEC_TXIE_MASK;} \
    \
    void SetInterruptPriorities() \
    { \
        UART##N##RXIPCbits.U##N##RXIP = 1; \
        UART##N##RXIPCbits.U##N##RXIS = 0; \
        UART##N##EIPCbits.U##N##EIP = 1; \
        UART##N##EIPCbits.U##N##EIS = 0; \
        UART##N##TXIPCbits.U##N##TXIP = 1; \
        UART##N##TXIPCbits.U##N##TXIS = 0; \
    } \
    \
    void SetRXPPS(U##N##RXPPS pin) {U##N##RXR = pin;} \
    \
    bool RXReady() {return U##N##STAbits.URXDA;} \
    uint32_t RXData() {return U##N##RXREG;} \
    \
    void RegisterReadCallback(void (*callback)(void *), void *context) {_readCallback = callback; _readContext = context;} \
    void UnregisterReadCallback() {_readCallback = NULL;} \
    \
    void RXHandler() {if (_readCallback) (*_readCallback)(_readContext);} \
    \
    \
    bool TXReady() {return !U##N##STAbits.UTXBF;} \
    void TXData(uint8_t data) {U##N##TXREG = data;} \
    \
    void RegisterWriteCallback(void (*callback)(void *), void *context) {_writeCallback = callback; _writeContext = context;} \
    void UnregisterWriteCallback() {_writeCallback = NULL;} \
    \
    void TXHandler() {if (_writeCallback) (*_writeCallback)(_writeContext);} \
    \
private: \
    void (*_readCallback)(void *context); \
    void *_readContext; \
    void (*_writeCallback)(void *context); \
    void *_writeContext; \
    \
    CUART##N(const CUART##N& orig); \
};

UARTCLASS(3)

extern CUART3 UART3;

#endif	/* UART_H */

