/* 
 * File:   DMA.h
 * Author: Bob
 *
 * Created on September 13, 2020, 3:04 PM
 */

#ifndef DMA_H
#define	DMA_H

#include <stddef.h>
extern "C"
{
#include "definitions.h"
}
#include "Peripherals.h"
#include "PMD.h"
#include "Interrupts.h"

struct DMASource
{
    void *address;
    size_t cellSize;
    size_t blockSize;
};

struct DMADestination
{
    void *address;
    size_t blockSize;
};

struct DMAxRegisters
{
    RegisterTCSI<__DCH0CONbits_t> DCHCON;   
    RegisterTCSI<__DCH0ECONbits_t> DCHECON;   
    RegisterTCSI<__DCH0INTbits_t> DCHINT;   
    RegisterTCSI<__DCH0SSAbits_t> DCHSSA;   
    RegisterTCSI<__DCH0DSAbits_t> DCHDSA;   
    RegisterTCSI<__DCH0SSIZbits_t> DCHSSIZ;   
    RegisterTCSI<__DCH0DSIZbits_t> DCHDSIZ;   
    RegisterTCSI<__DCH0SPTRbits_t> DCHSPTR;   
    RegisterTCSI<__DCH0DPTRbits_t> DCHDPTR;   
    RegisterTCSI<__DCH0CSIZbits_t> DCHCSIZ;   
    RegisterTCSI<__DCH0CPTRbits_t> DCHCPTR;   
    RegisterTCSI<__DCH0DATbits_t> DCHDAT;   
};

class DMA
{
public:
    DMA(int channel, int priority, const DMASource &dmaSource, 
        const DMADestination &dmaDestination, int dmaTransferTrigger = -1);
    ~DMA();
    
    void Enable() {_regs.DCHCON.set = 1 << 7;}
    void Disable() {_regs.DCHCON.clr = 1 << 7;}

    void Force() {_regs.DCHECON.bits.CFORCE = 1;}
    void Abort() {_regs.DCHECON.bits.CABORT = 1;}
    bool IsBusy() const {return _regs.DCHCON.bits.CHBUSY;}
    
    void SetAbortInterrupt(uint8_t irq)
    {
        _regs.DCHECON.bits.CHAIRQ = irq;
        _regs.DCHECON.bits.AIRQEN = 0;
    }
    void ClearAbortInterrupt() {_regs.DCHECON.bits.AIRQEN = 0;}

    enum DMAInterruptTrigger 
        {AddressError = 1, TransferAbort = 2, CellTransferComplete = 4, BlockTransferComplete = 8,
         DestinationHalfFull = 16, DestinationDone = 32, SourceHalfEmpty = 64, SourceDone = 128};
    void SetDMAInterruptTrigger(int trig) {_regs.DCHINT = trig << 16;}
    int GetDMAInterruptCause() const {return _regs.DCHINT & 0xff;}
    
    void EnableInterrupt() {DMAInt[_channel].dma.enable = 1;}
    void DisableInterrupt() {DMAInt[_channel].dma.enable = 0;}
    
    void ClearDMAInterruptFlags() {_regs.DCHINT.clr = 0xff;}

    // WARNING: Interrupts won't fire if you don't set priorities!
    void SetInterruptPriorities(int priority = 1, int subpriority = 0)
    {
        DMAInt[_channel].dma.priority = priority;
        DMAInt[_channel].dma.subpriority = subpriority;
    }
   
    void RegisterCallback(void (*callback)(void *), void *context) 
    {
        SetInterruptHandler(DMAInt[_channel].dma.irqNumber, callback, context);
    }
    void UnregisterCallback() 
    {
        ClearInterruptHandler(DMAInt[_channel].dma.irqNumber);
    }
    
    void SetDestination(const DMADestination &dmaDestination)
    {
        // Destination physical address
        _regs.DCHDSA = uint32_t(KVA_TO_PA(dmaDestination.address));
        // Destination size
        _regs.DCHDSIZ = dmaDestination.blockSize;
    }
    
    // Returns the Physical Address (!!!) of the DMA destination! Use PA_TO_KVA0
    void *GetDestinationAddress() const {return (void *) _regs.DCHDSA.reg;}
    // Returns a count from 0..65535
    uint16_t GetDestinationPointer() const {return uint16_t(_regs.DCHDPTR);}
    
    enum ChainMode {NoChaining, FromHigherPriorityChannel, FromLowerPriorityChannel};
    
    void SetChaining(ChainMode chainMode)
    {
        switch (chainMode)
        {
            case NoChaining :
                _regs.DCHCON.clr = 1 << 5;
                break;
                
            case FromHigherPriorityChannel :
                _regs.DCHCON.clr = 1 << 8;
                _regs.DCHCON.set = 1 << 5;
                break;
                
            case FromLowerPriorityChannel :
                _regs.DCHCON.set = (1 << 8) | (1 << 5);
                break;
        }
    }
    
private:
    DMA(const DMA& orig);
    
    int _channel;
    DMAxRegisters &_regs; 
    static int _instanceCount;
};

#endif	/* DMA_H */

