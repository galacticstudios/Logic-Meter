/* 
 * File:   InputCapture.h
 * Author: Bob
 *
 * Created on June 22, 2020, 11:07 AM
 */

#ifndef INPUTCAPTURE_H
#define	INPUTCAPTURE_H

extern "C" 
{
#include "definitions.h"
}
#include "Peripherals.h"
#include "Interrupts.h"
#include "PMD.h"
#include "PPS.h"


struct ICxRegisters
{
    RegisterTCSI<__IC1CONbits_t> ICCON;
    Register ICBUF;
};

template <int index>
class InputCapture
{
public:
    InputCapture()
    {
        ICPMD[index - 1].Enable();
    }
    ~InputCapture()
    {
        Disable();
        ICPMD[index - 1].Disable();
    }
    
    enum Edge {Disabled, EveryEdge, EveryFalling, EveryRising, 
        EveryFourthRising, EverySixteenthRising, EveryEdgeAfterSpecified, InterruptOnly};
    enum TimerSelect {TimerY, TimerX, Timer32Bit};
    enum FirstEdge {Falling, Rising};
    
    void Initialize(Edge edge, TimerSelect timerSelect, FirstEdge firstEdge = Falling)
    {
        Disable();
        _regs.ICCON = int(edge) | (int(timerSelect) << 7) | (int(firstEdge) << 9);
    }
    
    void Enable() {_regs.ICCON.set = _IC1CON_ON_MASK;}
    void Disable() {_regs.ICCON.clr = _IC1CON_ON_MASK;}
    
    bool Error() const {return _regs.ICCON.bits.ICOV;}
    bool DataReady() const {return _regs.ICCON.bits.ICBNE;}
    uint32_t ReadData() const {return _regs.ICBUF;}

    void EnableInterrupt() {InputCaptureInt[index - 1].capture.enable = 1;}
    void DisableInterrupt() {InputCaptureInt[index - 1].capture.enable = 0;}
    void ClearInterrupt() {InputCaptureInt[index - 1].capture.flag = 0;}
    
    // WARNING: Interrupts won't fire if you don't set priorities!
    void SetInterruptPriorities(int priority = 1, int subpriority = 0) 
    { 
        InputCaptureInt[index - 1].capture.priority = priority;
        InputCaptureInt[index - 1].capture.subpriority = subpriority;
    }
    
    void RegisterCallback(void (*callback)(void *), void *context) 
    {
        SetInterruptHandler(InputCaptureInt[index - 1].capture.irqNumber, callback, context);
    } 
    void UnregisterCallback() 
    {
        ClearInterruptHandler(InputCaptureInt[index - 1].capture.irqNumber);
    } 
    
    uint8_t InputCaptureIRQ() const {return InputCaptureInt[index - 1].capture.irqNumber;}
    uint8_t InputCaptureErrorIRQ() const {return InputCaptureInt[index - 1].error.irqNumber;}

private:
    ICxRegisters &_regs = *(ICxRegisters *) (&IC1CON + (&IC2CON - &IC1CON) * (index - 1));
};

#endif	/* INPUTCAPTURE_H */

