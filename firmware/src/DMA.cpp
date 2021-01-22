
#include "DMA.h"

int DMA::_instanceCount;

DMA::DMA(int channel, int priority, const DMASource &dmaSource,
        const DMADestination &dmaDestination, int dmaTransferTrigger) :
        _regs(*((DMAxRegisters *) (&DCH0CON + (&DCH1CON - &DCH0CON) * channel))),
        _channel(channel)
{      
    if (++_instanceCount == 1)
        DMAPMD.Enable();

    // Make sure the definition of DMAxRegisters matches the PIC32 register layout
    if (sizeof(DMAxRegisters) != (&DCH1CON - &DCH0CON) * sizeof(DCH0CON))
    {
        __builtin_software_breakpoint();
    }
    // Enable the DMA controller
    DMACONbits.ON = 1;
    // Channel off; set priority; no chaining
    _regs.DCHCON = priority & 3;
    // No start or stop IRQs; no pattern match
    _regs.DCHECON = 0;
    // Source physical address
    _regs.DCHSSA = uint32_t(KVA_TO_PA(dmaSource.address));
    // Source size
    _regs.DCHSSIZ = dmaSource.blockSize;
    // Destination
    SetDestination(dmaDestination);
    // Cell size (bytes transferred per event)
    _regs.DCHCSIZ = dmaSource.cellSize;
    // Channel Transfer Event
    if (dmaTransferTrigger != -1)
    {
        _regs.DCHECON.bits.CHSIRQ = uint8_t(dmaTransferTrigger);
        _regs.DCHECON.bits.SIRQEN = 1;
    }
    // No interrupts
    _regs.DCHINT = 0;
    // But set the interrupt priorities. If you forget to set them, interrupts don't work
    SetInterruptPriorities();
}

DMA::~DMA() 
{
    Abort();
    Disable();
    if (--_instanceCount == 0)
        DMAPMD.Disable();
}

Implement_InterruptHandler(_DMA0_VECTOR, DMAInt[0].dma)
Implement_InterruptHandler(_DMA1_VECTOR, DMAInt[1].dma)
Implement_InterruptHandler(_DMA2_VECTOR, DMAInt[2].dma)
Implement_InterruptHandler(_DMA3_VECTOR, DMAInt[3].dma)
Implement_InterruptHandler(_DMA4_VECTOR, DMAInt[4].dma)
Implement_InterruptHandler(_DMA5_VECTOR, DMAInt[5].dma)
Implement_InterruptHandler(_DMA6_VECTOR, DMAInt[6].dma)
Implement_InterruptHandler(_DMA7_VECTOR, DMAInt[7].dma)
