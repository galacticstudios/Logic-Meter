/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _PERIPHERALS_H    /* Guard against multiple inclusion */
#define _PERIPHERALS_H

#include <stdint.h>

// Define a template for a PIC32 peripheral register that has an associated
// bit-field type and CLR, SET, and INV[ert] registers
template <class T>
struct RegisterTCSI
{
    uint32_t operator=(uint32_t rhs) {reg = rhs;}
    operator uint32_t() const {return reg;}
    union
    {
        volatile T bits;
        volatile uint32_t reg;
    };
    volatile uint32_t clr, set, inv;
};

// Define a template for a PIC32 peripheral register that DOES NOT have an associated
// bit-field type, but has CLR, SET, and INV[ert] registers
struct RegisterCSI
{
    uint32_t operator=(uint32_t rhs) {reg = rhs;}
    operator uint32_t() const {return reg;}
    volatile uint32_t reg;
    volatile uint32_t clr, set, inv;
};

// Define a template for a PIC32 peripheral register that DOES NOT have an associated
// bit-field type, CLR, SET, or INV[ert] registers
struct Register
{
    uint32_t operator=(uint32_t rhs) {reg = rhs;}
    operator uint32_t() const {return reg;}
    volatile uint32_t reg;
};

// Access a bit field in a register
struct RegisterBF
{
    volatile uint32_t &target;
    const uint8_t msb, lsb;
    operator uint32_t() const {return (target >> lsb) & ((1 << (msb - lsb + 1)) - 1);}
    template <class RHS>
    uint32_t &operator=(RHS rhs) const
    {
        uint32_t mask = (1 << (msb - lsb + 1)) - 1;
        target = (target & ~(mask << lsb)) | ((rhs & mask) << lsb);
    }
};


#endif /* _PERIPHERALS_H */

/* *****************************************************************************
 End of File
 */
