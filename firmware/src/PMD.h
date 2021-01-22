
#ifndef PMD_H    /* Guard against multiple inclusion */
#define PMD_H

extern "C"
{
#include "definitions.h"
}

// Implement a class for controlling the Peripheral Module Disable bits.
class PMD
{
public:
    // PMDIndex is the index of the PMD register (e.g. 1 for PMD1, 2 for PMD2, etc.)
    // b is the bit in the register for the specific device.
    // For example, the PIC32MZ data sheet says that the ADC PMD bit is PMD1<0>.
    // So we'd create a PMD object like:
    //    PMD ADCPMD(1, 0);
    // We could have more efficiency by making this a template with the PMDIndex and
    // b as template arguments, but then we wouldn't be able to put, e.g., six of
    // them in an array, like we have an array of SPIPMD objects and then the SPI class
    // can access its PMD object.
    PMD(int PMDIndex, int b) : _pmdIndex(PMDIndex), _bit(b) {}
    void Enable() const {*(&PMD1CLR + (_pmdIndex - 1) * 4) = 1 << _bit;} 
    void Disable() const {*(&PMD1SET + (_pmdIndex - 1) * 4) = 1 << _bit;}
    void Enable(bool enable) const {enable ? Enable() : Disable();}
    
private:
    int _pmdIndex, _bit;
};

extern const PMD DMAPMD;
extern const PMD ICPMD[];
extern const PMD SPIPMD[];
extern const PMD TMRPMD[];
extern const PMD UARTPMD[];

#endif /* PMD_H */

/* *****************************************************************************
 End of File
 */
