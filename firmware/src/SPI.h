/* 
 * File:   SPI.h
 * Author: Bob
 *
 * Created on July 11, 2020, 1:56 PM
 */

#ifndef SPI_H
#define	SPI_H

#include "PPS.h"
#include "Peripherals.h"
#include "Interrupts.h"
#include "PMD.h"
#include "DMA.h"

struct SPIxRegisters
{
    RegisterTCSI<__SPI1CONbits_t> SPICON;   
    RegisterTCSI<__SPI1STATbits_t> SPISTAT;   
    RegisterCSI SPIBUF;   
    RegisterCSI SPIBRG;   
    RegisterTCSI<__SPI1CON2bits_t> SPICON2;   
};

template <int index>
class SPI
{
public:
    SPI()
    {
        SPIPMD[index - 1].Enable();
    }
    ~SPI()
    {
        Disable();
        SPIPMD[index - 1].Disable();
    }

    void Initialize(bool master, uint32_t masterBaud, bool enhbuf, int dataSize = 8)
    {
        Disable();
        DisableFaultInterrupt();
        DisableRXInterrupt();
        DisableTXInterrupt();
        _regs.SPICON = 0;
        uint32_t dummy = _regs.SPIBUF;
        _regs.SPICON.bits.ENHBUF = enhbuf;
        _regs.SPICON.bits.SRXISEL = 1;
        SPIInt[index - 1].fault.flag = 0;
        SPIInt[index - 1].receiveDone.flag = 0;
        SPIInt[index - 1].transferDone.flag = 0;
        _regs.SPIBRG = masterBaud;
        _regs.SPISTAT.bits.SPIROV = 0;
        _regs.SPICON.bits.MSTEN = master;
        SetWidth(dataSize);
    }

    void SetMode(int polarity, int phase)
    {
        _regs.SPICON.bits.CKP = polarity;
        /* Everyone on the web who says that CKE is just the inverse */
        /* of CPHA is talking about SPI Masters. When you're an SPI */
        /* slave, CKE should be equal to CPHA!! */
        _regs.SPICON.bits.CKE = _regs.SPICON.bits.MSTEN ? !phase : phase;
    }
    
    void SetWidth(int width)
    {
        _regs.SPICON.bits.MODE16 = (width == 16);
        _regs.SPICON.bits.MODE32 = (width == 32);
    }
    
    enum class ReceiveInterruptTrigger {BufferEmpty, BufferNotEmpty, BufferHalfFull, BufferFull};
    void SetReceiveInterruptTrigger(ReceiveInterruptTrigger rt) {_regs.SPICON.bits.SRXISEL = int(rt);}
    enum class TransmitInterruptTrigger {ShiftRegisterEmpty, BufferEmpty, BufferHalfEmpty, BufferNotFull};
    void SetTransmitInterruptTrigger(TransmitInterruptTrigger tt) {_regs.SPICON.bits.STXISEL = int(tt);}
   
    void UseSPISelect(bool useSPISelect)
    {
        _regs.SPICON.bits.SSEN = _regs.SPICON.bits.MSSEN = useSPISelect ? 1 : 0;
    }

    void Enable() {_regs.SPICON.set = _SPI1CON_ON_MASK;}
    void Disable() {_regs.SPICON.clr = _SPI1CON_ON_MASK;}
    
    void EnableFaultInterrupt() {SPIInt[index - 1].fault.enable = 1;}
    void DisableFaultInterrupt() {SPIInt[index - 1].fault.enable = 0;}
    
    void EnableRXInterrupt() {SPIInt[index - 1].receiveDone.enable = 1;} 
    void DisableRXInterrupt() {SPIInt[index - 1].receiveDone.enable = 0;}
    
    void EnableTXInterrupt() {SPIInt[index - 1].transferDone.enable = 1;} 
    void DisableTXInterrupt() {SPIInt[index - 1].transferDone.enable = 0;} 

    // WARNING: Interrupts won't fire if you don't set priorities!
    void SetInterruptPriorities(int priority = 1, int subpriority = 0)
    {
        SPIInt[index - 1].receiveDone.priority = priority;
        SPIInt[index - 1].receiveDone.subpriority = subpriority;
        SPIInt[index - 1].transferDone.priority = priority;
        SPIInt[index - 1].transferDone.subpriority = subpriority;
        SPIInt[index - 1].fault.priority = priority;
        SPIInt[index - 1].fault.subpriority = subpriority;
    }
   
    bool RXReady() {return !_regs.SPISTAT.bits.SPIRBE;}
    uint32_t RXData() {return _regs.SPIBUF;}
   
    void RegisterReadCallback(void (*callback)(void *), void *context) 
    {
        SetInterruptHandler(SPIInt[index - 1].receiveDone.irqNumber, callback, context);
    }
    void UnregisterReadCallback() 
    {
        ClearInterruptHandler(SPIInt[index - 1].receiveDone.irqNumber);
    }
   
    bool TXReady() {return !_regs.SPISTAT.bits.SPITBF;}
    bool TXEmpty() {return _regs.SPISTAT.bits.SPITBE;}
    void TXData(uint32_t data) {_regs.SPIBUF = data;}
   
    void RegisterWriteCallback(void (*callback)(void *), void *context) 
    {
        SetInterruptHandler(SPIInt[index - 1].transferDone.irqNumber, callback, context);
    }
    void UnregisterWriteCallback() 
    {
        ClearInterruptHandler(SPIInt[index - 1].transferDone.irqNumber);
    }
   
    void ClearReceiveOverrun() {_regs.SPISTAT.clr = 1 << 6;}
    void RegisterFaultCallback(void (*callback)(void *), void *context) 
    {
        SetInterruptHandler(SPIInt[index - 1].fault.irqNumber, callback, context);
    }
    void UnregisterFaultCallback() 
    {
        ClearInterruptHandler(SPIInt[index - 1].fault.irqNumber);
    }
    
    ::DMASource DMASource() 
    {
        size_t width = _regs.SPICON.bits.MODE32 ? 4 : (_regs.SPICON.bits.MODE16 ? 2 : 1);
        return ::DMASource {(void *) &_regs.SPIBUF.reg, width, width};
    }
    ::DMADestination DMADestination() 
    {
        size_t width = _regs.SPICON.bits.MODE32 ? 4 : (_regs.SPICON.bits.MODE16 ? 2 : 1);
        return ::DMADestination {(void *) &_regs.SPIBUF.reg, width};
    }
    uint8_t ReceiveDoneIRQ() const {return SPIInt[index - 1].receiveDone.irqNumber;}
    uint8_t TransferDoneIRQ() const {return SPIInt[index - 1].transferDone.irqNumber;}
    
private:
    SPI(const SPI &orig);
    
    SPIxRegisters &_regs = *(SPIxRegisters *) (&SPI1CON + (&SPI2CON - &SPI1CON) * (index - 1));
};

#endif	/* SPI_H */

