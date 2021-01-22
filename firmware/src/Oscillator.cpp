/* 
 * File:   Oscillator.cpp
 * Author: Bob
 * 
 * Created on September 29, 2020, 10:54 AM
 */

extern "C" 
{
#include "definitions.h"
}
#include "Oscillator.h"

uint32_t Oscillator::SYSCLK() const
{
    switch (OSCCONbits.COSC)
    {
        case 0 : return 8000000 / FRCDivisor();
        case 1 : return SPLL();
        case 2 : return _primary;
        case 3 : return 0;
        case 4 : return _secondary;
        case 5 : return 32768;
        case 6 : return 8000000;
        case 7 : return 8000000;
    }
}

uint32_t Oscillator::SPLL() const
{
    uint32_t iclk = SPLLCONbits.PLLICLK ? 8000000 : _primary;
    iclk /= SPLLCONbits.PLLIDIV + 1;
    iclk *= SPLLCONbits.PLLMULT + 1;
    return iclk / (1 << SPLLCONbits.PLLODIV);
}

uint32_t Oscillator::FRCDivisor() const
{
    if (OSCCONbits.FRCDIV == 7)
        return 256;
    return 1 << OSCCONbits.FRCDIV;
}

uint32_t Oscillator::PBCLK(int index) const
{
    volatile uint32_t *pbxdiv = &PB1DIV + (&PB2DIV - &PB1DIV) * (index - 1);
    return SYSCLK() / ((*pbxdiv & 0x7f) + 1);
}
