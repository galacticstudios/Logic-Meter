/* 
 * File:   CVRef.h
 * Author: Bob
 *
 * Created on September 4, 2020, 9:11 AM
 */

#ifndef CVREF_H
#define	CVREF_H

extern "C" 
{
#include "definitions.h"
}

class CVRef 
{
public:
    CVRef(bool enableCVrefout) 
    {
        PMD1bits.CVRMD = 0;
        CVRCON = enableCVrefout ? 0x40 : 0;
    }
    ~CVRef() 
    {
        CVRCON = 0;
        PMD1bits.CVRMD = 1;
    }
    
    void Enable() {CVRCONbits.ON = 1;}
    void Disable() {CVRCONbits.ON = 0;}
    
    // Range is 0 or 1; value is 0..15. Returns the ratio of the output to Vcc
    // (i.e. the CVREFOUT will be Vcc times the return value)
    double Set(int range, int value)
    {
        CVRCONbits.CVRR = range ? 1 : 0;
        CVRCONbits.CVR = value;
        if (range)
            return double(value) / 24;
        else
            return 0.25 + double(value) / 32;
    }
    
private:
    CVRef(const CVRef& orig);
};

#endif	/* CVREF_H */

