/* 
 * File:   UART.h
 * Author: Bob
 *
 * Created on May 25, 2020, 8:36 PM
 */

#ifndef UART_H
#define	UART_H

#include "Peripherals.h"
#include "Interrupts.h"
#include "Oscillator.h"
#include "PPS.h"
#include "PMD.h"

struct UARTxRegisters
{
    RegisterTCSI<__U1MODEbits_t> UMODE;   
    RegisterTCSI<__U1STAbits_t> USTA;   
    RegisterCSI UTXREG;   
    RegisterCSI URXREG;   
    RegisterCSI UBRG;   
};

typedef struct
{
    uint32_t baudRate;
    enum {UART8BitParityNone, UART8BitParityEven, UART8BitParityOdd, UART9BitParityNone} widthParity;
    // 1 or 2 stop bits
    int stopBits;
} UARTSerialSetup;



template <int index>
class UART
{
public:
    UART()
    {
        UARTPMD[index - 1].Enable();
    }
    ~UART()
    {
        DisableFaultInterrupt();
        DisableRXInterrupt();
        DisableTXInterrupt();
        Disable();
        UARTPMD[index - 1].Disable();
    }
    
    void Initialize() 
    {
        _regs.UMODE = 0x0;

        /* Enable UART Receiver and Transmitter */
        _regs.USTA = (_U1STA_UTXEN_MASK | _U1STA_URXEN_MASK);

        /* BAUD Rate register Setup */
        _regs.UBRG = 0;

        /* Turn ON UART3 */
        _regs.UMODE = _U1MODE_ON_MASK;
    }
    
    bool SerialSetup(UARTSerialSetup *setup, uint32_t srcClkFreq)
    {
        if (setup != NULL)
        {
            uint32_t baud = setup->baudRate;
            uint32_t brgValHigh = 0;
            uint32_t brgValLow = 0;
            uint32_t brgVal = 0;

            if(srcClkFreq == 0)
            {
                srcClkFreq = oscillator.PBCLK(2);
            }

            /* Calculate BRG value */
            brgValLow = ((srcClkFreq / baud) >> 4) - 1;
            brgValHigh = ((srcClkFreq / baud) >> 2) - 1;

            /* Check if the baud value can be set with low baud settings */
            if((brgValHigh >= 0) && (brgValHigh <= UINT16_MAX))
            {
                brgVal =  (((srcClkFreq >> 2) + (baud >> 1)) / baud ) - 1;
                _regs.UMODE.bits.BRGH = 1;
            }
            else if ((brgValLow >= 0) && (brgValLow <= UINT16_MAX))
            {
                brgVal = ( ((srcClkFreq >> 4) + (baud >> 1)) / baud ) - 1;
                _regs.UMODE.bits.BRGH = 0;
            }
            else
            {
                return false;
            }

            _regs.UMODE.bits.PDSEL = setup->widthParity;

            /* Configure UART mode */
            _regs.UMODE.bits.STSEL = setup->stopBits - 1;

            /* Configure UART3 Baud Rate */
            _regs.UBRG = brgVal;

            return true;
        }

        return false;
    }
    
    void Disable() {_regs.UMODE.clr = _U1MODE_ON_MASK;}
    
    void EnableFaultInterrupt() {UARTInt[index - 1].fault.enable = 1;}
    void DisableFaultInterrupt() {UARTInt[index - 1].fault.enable = 0;}

    void EnableRXInterrupt() 
    {
        UARTInt[index - 1].receiveDone.enable = 1;
    }
    void DisableRXInterrupt() {UARTInt[index - 1].receiveDone.enable = 0;}
    
    void EnableTXInterrupt() 
    {
        UARTInt[index - 1].transferDone.enable = 1;
    } 
    void DisableTXInterrupt() {UARTInt[index - 1].transferDone.enable = 0;} 
    
    // WARNING: Interrupts won't fire if you don't set priorities!
    void SetInterruptPriorities(int priority = 1, int subpriority = 0) 
    { 
        UARTInt[index - 1].receiveDone.priority = priority;
        UARTInt[index - 1].receiveDone.subpriority = subpriority;
        UARTInt[index - 1].transferDone.priority = priority;
        UARTInt[index - 1].transferDone.subpriority = subpriority;
        UARTInt[index - 1].fault.priority = priority;
        UARTInt[index - 1].fault.subpriority = subpriority;
    } 
    
    bool RXReady() const {return _regs.USTA.bits.URXDA;} 
    uint32_t RXData() {return _regs.URXREG;} 
    
    void RegisterReadCallback(void (*callback)(void *), void *context) 
    {
        SetInterruptHandler(UARTInt[index - 1].receiveDone.irqNumber, callback, context);
    } 
    void UnregisterReadCallback() 
    {
        ClearInterruptHandler(UARTInt[index - 1].receiveDone.irqNumber);
    } 
    
    bool TXReady() const {return !_regs.USTA.bits.UTXBF;} 
    void TXData(uint32_t data) {_regs.UTXREG = data;} 
    
    void RegisterWriteCallback(void (*callback)(void *), void *context) 
    {
        SetInterruptHandler(UARTInt[index - 1].transferDone.irqNumber, callback, context);
    } 
    void UnregisterWriteCallback() 
    {
        ClearInterruptHandler(UARTInt[index - 1].transferDone.irqNumber);
    } 
      
private:
    UARTxRegisters &_regs = *(UARTxRegisters *) (&U1MODE + (&U2MODE - &U1MODE) * (index - 1));
};

#endif	/* UART_H */

