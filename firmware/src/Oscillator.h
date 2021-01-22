/* 
 * File:   Oscillator.h
 * Author: Bob
 *
 * Created on September 29, 2020, 10:54 AM
 */

#ifndef OSCILLATOR_H
#define	OSCILLATOR_H

#include <stdint.h>

class Oscillator 
{
public:
    // Hardware oscillator speeds in Hz
    Oscillator(uint32_t primaryOsc = 0, uint32_t secondaryOsc = 0) :
        _primary(primaryOsc), _secondary(secondaryOsc) {}
        
    uint32_t SYSCLK() const;
    // index = 1..n
    uint32_t PBCLK(int index) const;
    
    uint32_t FRCDivisor() const;
    uint32_t SPLL() const;
    
private:
    Oscillator(const Oscillator& orig);
    
    uint32_t _primary, _secondary;
};

// Should be instantiated somewhere in your code with the hardware oscillator arguments
extern Oscillator oscillator;

#endif	/* OSCILLATOR_H */

