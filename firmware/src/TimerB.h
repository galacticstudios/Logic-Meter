/* 
 * File:   TimerB.h
 * Author: Bob
 *
 * Timer Type B (16 or 32 bits)
 * Created on October 4, 2020, 12:03 PM
 */

#ifndef TIMERB_H
#define	TIMERB_H

#include "fixed.h"
#include "PPS.h"
#include "Peripherals.h"
#include "Interrupts.h"
#include "PMD.h"
#include "Oscillator.h"

struct TMRxRegisters
{
    RegisterTCSI<__T2CONbits_t> TCON;   
    RegisterCSI TMR;   
    RegisterCSI PR;   
};

template <int index> // Index is 2..n
class TimerB 
{
public:
    TimerB() 
    {
        TMRPMD[index - 1].Enable();
    }
    ~TimerB()
    {
        Disable();
        TMRPMD[index - 1].Disable();
    }

    void Initialize(uint32_t freq)
    {
        Initialize(freq, 0);
    }

    // Duration in seconds
    void InitializeDuration(fixed duration)
    {
        InitializeDuration(duration, 0);
    }
    
    void Reset(uint32_t value = 0) {_regs.TMR = value;};
    void Enable() {_regs.TCON.set = _T2CON_ON_MASK;}
    void Disable() {_regs.TCON.clr = _T2CON_ON_MASK;}
    
    double GetFrequency()
    {
        uint32_t divisor = _regs.PR & 0xffff;
        // Factor in the timer prescaler. The prescaler goes 1, 2, 4, 8, 16, 32, 64,
        // then skips 128 and goes to 256. So we need to handle that
        divisor <<= _regs.TCON.bits.TCKPS;
        if (_regs.TCON.bits.TCKPS == 7)
            divisor <<= 1;
        
        return double(oscillator.PBCLK(3)) / divisor;
    }
    
    // WARNING: Interrupts won't fire if you don't set priorities!
    void SetInterruptPriorities()
    {
        TMRInt[index - 1].timer.priority = 1;
        TMRInt[index - 1].timer.subpriority = 0;
    }
    void EnableInterrupt() {TMRInt[index - 1].timer.enable = 1;}
    void DisableInterrupt() {TMRInt[index - 1].timer.enable = 0;}

    TMRxRegisters &Regs() const {return _regs;}
    
    uint8_t TimerIRQ() const {return TMRInt[index - 1].timer.irqNumber;}
    
protected:

    // Calculate the register settings for the requested frequency as close as possible.
    static void CalculateFreqPSPR(uint32_t hertz, uint8_t &prescale, uint32_t &period)
    {
        // What's our input clock rate?
        uint32_t clock = oscillator.PBCLK(3);
        // What divisor do we want? (Add hertz / 2 for rounding)
        uint32_t divisor = (clock + hertz / 2) / hertz;
        unsigned int trailingZeros = __builtin_ctz(divisor);
        
        // Set the timer prescaler. The prescaler goes 1, 2, 4, 8, 16, 32, 64,
        // then skips 128 and goes to 256. So we need to handle that
        if (trailingZeros >= 8)
        {
            prescale = 7;
            divisor /= 256;
        }
        else if (trailingZeros == 7)
        {
            prescale = 6;
            divisor /= 64;
        }
        else
        {
            prescale = trailingZeros;
            divisor >>= trailingZeros;
        }

        period = divisor - 1;
        // If the period is 0, that's not going to work. We need to decrease
        // the prescale and increase the divisor
        if (period == 0 && prescale)
        {
            --prescale;
            // If prescale == 6, we skipped from a factor of 256 to a factor of 64
            if (prescale == 6)
                period = 3;
            else
                period = 1;
        }
    }
    // Calculate the register settings for the requested duration as close as possible.
    static void CalculateDurationPSPR(fixed seconds, uint8_t &prescale, uint32_t &period)
    {
        // What's our input clock rate?
        uint32_t clock = oscillator.PBCLK(3);
        // What divisor do we want?
        uint32_t divisor = int(fixed(clock) * seconds);
        unsigned int trailingZeros = __builtin_ctz(divisor);
        
        // Set the timer prescaler. The prescaler goes 1, 2, 4, 8, 16, 32, 64,
        // then skips 128 and goes to 256. So we need to handle that
        if (trailingZeros >= 8)
        {
            prescale = 7;
            divisor /= 256;
        }
        else if (trailingZeros == 7)
        {
            prescale = 6;
            divisor /= 64;
        }
        else
        {
            prescale = trailingZeros;
            divisor >>= trailingZeros;
        }
        
        period = divisor - 1;
        // If the period is 0, that's not going to work. We need to decrease
        // the prescale and increase the divisor
        if (period == 0 && prescale)
        {
            --prescale;
            // If prescale == 6, we skipped from a factor of 256 to a factor of 64
            if (prescale == 6)
                period = 3;
            else
                period = 1;
        }
    }
    
    void Initialize(uint32_t freq, uint32_t initTCON)
    {
        Disable();
        DisableInterrupt();
        _regs.TCON = initTCON;
        TMRInt[index - 1].timer.flag = 0;
        uint8_t prescale;
        uint32_t period;
        CalculateFreqPSPR(freq, prescale, period);
        _regs.TCON.bits.TCKPS = prescale;
        _regs.PR = period - 1;
        _regs.TMR = 0;
    }
    
    // Duration in seconds
    void InitializeDuration(fixed duration, uint32_t initTCON)
    {
        Disable();
        DisableInterrupt();
        _regs.TCON = initTCON;
        TMRInt[index - 1].timer.flag = 0;
        uint8_t prescale;
        uint32_t period;
        CalculateDurationPSPR(duration, prescale, period);
        _regs.TCON.bits.TCKPS = prescale;
        _regs.PR = period - 1;
        _regs.TMR = 0;
    }

    TimerB(const TimerB& orig);
    
    TMRxRegisters &_regs = *(TMRxRegisters *) (&T2CON + (&T3CON - &T2CON) * (index - 2));
};

template <int index>
class TimerB32 : public TimerB<index>
{
    static_assert((index & 1) == 0, "32 bit Timers must have even indexes");
    
public:
    TimerB32()
    {
        TMRPMD[index].Enable();
    }
    
    ~TimerB32()
    {
        TimerB<index>::Disable();
        TMRPMD[index].Disable();
    }
    
    void Initialize(uint32_t freq)
    {
        TimerB<index>::Initialize(freq, 1 << 3);
    }
    
    // Duration in seconds
    void InitializeDuration(fixed duration)
    {
        TimerB<index>::InitializeDuration(duration, 1 << 3);
    }
    
    // WARNING: Interrupts won't fire if you don't set priorities!
    void SetInterruptPriorities()
    {
        TMRInt[index].timer.priority = 1;
        TMRInt[index].timer.subpriority = 0;
    }
    void EnableInterrupt() {TMRInt[index].timer.enable = 1;}
    void DisableInterrupt() {TMRInt[index].timer.enable = 0;}
    
    uint8_t TimerIRQ() const {return TMRInt[index].timer.irqNumber;}
};

#endif	/* TIMERB_H */

