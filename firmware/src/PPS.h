/* 
 * File:   PPS.h
 * Author: Bob
 *
 * Created on May 26, 2020, 10:21 AM
 */

#ifndef PPS_H
#define	PPS_H

extern "C"
{
#include "definitions.h"
}

template <typename PPSOutputs, volatile uint32_t &Reg>        
class OutputPinSelection
{
public:
    OutputPinSelection operator=(PPSOutputs val) {Reg = (uint32_t) val;}
};

#if defined(__32MK1024MCF064__)

// PPS Group 1 Input Pin Selection
typedef enum {RPA0, RPB3, RPB4, RPB15, RPB7, RPC7, RPC0, na7, RPA11, RPD5, RPG6,
        RPF1, RPE0, RPA15} PPSGroup1Inputs;

// PPS Group 2 Input Pin Selection
typedef enum {RPA1, RPB5, RPB1, RPB11, RPB8, RPA8, RPC8, RPB12, RPA12, RPD6, RPG7,
        RPG0, RPE1, RPA14} PPSGroup2Inputs;

// PPS Group 3 Input Pin Selection
typedef enum {RPB6, RPC15, RPA4, RPB13, RPB2, RPC6, RPC1, RPA7, RPE14, RPC13, RPG8,
        RPF0 = 12, RPD4} PPSGroup3Inputs;

// PPS Group 3 Output Pin Selection
typedef enum {OFF, U3TX, U4RTS, SS1, na4, OC4, OC5, REFCLKO1, C5OUT, OC10, OC14,
        U6TX, C3TX, na13, SS3, SS4, SS5, SDO6, REFCLKO2, na19, QEICMP3} PPSGroup3Outputs;

extern OutputPinSelection<PPSGroup3Outputs, RPB6R> RPB6RPPS;
extern OutputPinSelection<PPSGroup3Outputs, RPC6R> RPC6RPPS;

#elif defined(__32MZ2048EFH064__)

// PPS Group 1 Input Pin Selection
enum PPSGroup1Inputs {RPD2, RPG8, RPF4, RPD10, RPF1, RPB9, RPB10, RPC14, RPB5, na1_9, RPC1,
    RPD14, RPG1, RPA14, RPD6};

// PPS Group 1 Output Pin Selection
enum class PPSGroup1Outputs {O1OFF, U3TX, U4RTS, na1_3, na1_4, SDO1, SDO2, SDO3, na1_8, SDO5, SS6,
    OC3, OC6, REFCLKO4, C2OUT, C1TX};

// PPS Group 2 Input Pin Selection
enum PPSGroup2Inputs {RPD3, RPG7, RPF5, RPD11, RPF0, RPB1, RPE5, RPC13, RPB3, na2_9, RPC4,
    RPD15, RPG0, RPA15, RPD7};

// PPS Group 2 Output Pin Selection
enum class PPSGroup2Outputs {O2OFF, U1TX, U2RTS, U5TX, U6RTS, SDO1, SDO2, SDO3, SDO4, SDO5, na2_10,
    OC4, OC7, na2_13, na2_14, REFCLKO1};

// PPS Group 3 Input Pin Selection
enum PPSGroup3Inputs {RPD9, RPG6, RPB8, RPB15, RPD4, RPB0, RPE3, RPB7, na3_8, RPF12, RPD12,
    RPF8, RPC3, RPE9};

// PPS Group 3 Output Pin Selection
enum class PPSGroup3Outputs {O3OFF, U3RTS, U4TX, na3_3, U6TX, SS1, na3_6, SS3, SS4, SS5, SDO6, OC5, 
    OC8, na4_13, C1OUT, REFCLK03};

// PPS Group 4 Input Pin Selection
//enum PPSGroup4Inputs {RPD1, RPG9, RPB14, RPD0, na4_4, RPB6, RPD5, RPB2, RPF3, RPF13, na4_10,
//    RPF2, RPC2, RPE8};

// PPS Group 4 Output Pin Selection
enum class PPSGroup4Outputs {O4OFF, U1RTS, U2TX, U5RTS, U6TX, na4_5, SS2, na4_7, SDO4, na4_9,
    SDO6, OC2, OC1, OC9, na4_14, C2TX};

extern OutputPinSelection<PPSGroup1Outputs, RPD10R> RPD10RPPS;
extern OutputPinSelection<PPSGroup1Outputs, RPF4R> RPF4RPPS;

#else

#error PPS not defined for this microcontroller

#endif

struct SPIPPS
{
    volatile uint32_t &SDIR, &SSR;
    
};

#endif	/* PPS_H */

