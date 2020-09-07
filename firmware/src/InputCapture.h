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
#include "PPS.h"

#define IC4PPS PPSGroup2Inputs

#define InputCaptureCLASS(N) \
class CInputCapture##N \
{ \
public: \
    CInputCapture##N() {} \
    ~CInputCapture##N() {} \
    \
    void Initialize() {ICAP##N##_Initialize();} \
    void Enable() {IC##N##CONSET = _IC##N##CON_ON_MASK;} \
    void Disable() {IC##N##CONCLR = _IC##N##CON_ON_MASK;} \
    \
    void SetPPS(IC##N##PPS pin) {IC##N##R = (uint32_t) pin;} \
    \
    bool DataReady() {return (IC##N##CONbits.ICBNE != 0);} \
    uint32_t ReadData() {return IC##N##BUF;} \
    bool Error() {return IC##N##CONbits.ICOV;} \
    \
private: \
    CInputCapture##N(const CInputCapture##N &orig); \
};

InputCaptureCLASS(4)

extern CInputCapture4 InputCapture4;


#endif	/* INPUTCAPTURE_H */

