/* 
 * File:   SPI.h
 * Author: Bob
 *
 * Created on July 11, 2020, 1:56 PM
 */

#ifndef SPI_H
#define	SPI_H

#include "PPS.h"

#if defined(__32MK1024MCF064__)

#define SPI1DIPPS PPSGroup1Inputs
#define SPI3DIPPS PPSGroup1Inputs
#define SPI4CLKPPS PPSGroup1Inputs
#define SPI4DIPPS PPSGroup2Inputs
#define SPI4CKPPS PPSGroup1Inputs
#define SPI4SSPPS PPSGroup3Inputs
#define SPI5DIPPS PPSGroup1Inputs
#define SPI6CSPPS PPSGroup1Inputs

// Indicate which IEC/IFS registers each interrupt uses
#define SPI1PMD 5
#define SPI1FaultI 1
#define SPI1RXI 1
#define SPI1TXI 1
#define SPI1FaultIPC 8
#define SPI1RXIPC 9
#define SPI1TXIPC 9
#define SPI4PMD 5
#define SPI4FaultI 6
#define SPI4RXI 6
#define SPI4TXI 6
#define SPI4FaultIPC 55
#define SPI4RXIPC 55
#define SPI4TXIPC 55

#elif defined(__32MZ2048EFH064__)

#define SPI1DIPPS PPSGroup1Inputs
#define SPI2DIPPS PPSGroup2Inputs
#define SPI2SSPPS PPSGroup4Inputs
#define SPI3DIPPS PPSGroup1Inputs
#define SPI4DIPPS PPSGroup2Inputs
#define SPI4SSPPS PPSGroup3Inputs
#define SPI5DIPPS PPSGroup1Inputs
#define SPI6SSPPS PPSGroup1Inputs

// Indicate which IEC/IFS registers each interrupt uses
#define SPI1PMD 5
#define SPI1FaultI 3
#define SPI1RXI 3
#define SPI1TXI 3
#define SPI1FaultIPC 27
#define SPI1RXIPC 27
#define SPI1TXIPC 27

#define SPI2PMD 5
#define SPI2FaultI 4
#define SPI2RXI 4
#define SPI2TXI 4
#define SPI2FaultIPC 35
#define SPI2RXIPC 35
#define SPI2TXIPC 36

#define SPI4PMD 5
#define SPI4FaultI 5
#define SPI4RXI 5
#define SPI4TXI 5
#define SPI4FaultIPC 40
#define SPI4RXIPC 41
#define SPI4TXIPC 41

#else

#error SPI registers not defined for this microcontroller

#endif

#define _SPICLASS(N, PMDReg, FaultIECIFS, RXIECIFS, TXIECIFS, FaultIPC, RXIPC, TXIPC) \
\
class CSPI##N \
{ \
public: \
    volatile __PMD##PMDReg##bits_t &PMD = PMD##PMDReg##bits; \
    volatile uint32_t &FaultIECSET = IEC##FaultIECIFS##SET; \
    volatile uint32_t &FaultIECCLR = IEC##FaultIECIFS##CLR; \
    static const uint32_t IEC_FaultIE_MASK = _IEC##FaultIECIFS##_SPI##N##EIE_MASK; \
    volatile uint32_t &FaultIFSSET = IFS##FaultIECIFS##SET; \
    volatile uint32_t &FaultIFSCLR = IFS##FaultIECIFS##CLR; \
    static const uint32_t IFS_FaultIF_MASK = _IFS##FaultIECIFS##_SPI##N##EIF_MASK; \
    volatile uint32_t &RXIECSET = IEC##RXIECIFS##SET; \
    volatile uint32_t &RXIECCLR = IEC##RXIECIFS##CLR; \
    static const uint32_t IEC_RXIE_MASK = _IEC##RXIECIFS##_SPI##N##RXIE_MASK; \
    volatile uint32_t &RXIFSSET = IFS##RXIECIFS##SET; \
    volatile uint32_t &RXIFSCLR = IFS##RXIECIFS##CLR; \
    static const uint32_t IFS_RXIF_MASK = _IFS##RXIECIFS##_SPI##N##RXIF_MASK; \
    volatile uint32_t &TXIECSET = IEC##TXIECIFS##SET; \
    volatile uint32_t &TXIECCLR = IEC##TXIECIFS##CLR; \
    static const uint32_t IEC_TXIE_MASK = _IEC##TXIECIFS##_SPI##N##TXIE_MASK; \
    volatile uint32_t &TXIFSSET = IFS##TXIECIFS##SET; \
    volatile uint32_t &TXIFSCLR = IFS##TXIECIFS##CLR; \
    static const uint32_t IFS_TXIF_MASK = _IFS##TXIECIFS##_SPI##N##TXIF_MASK; \
    volatile __IPC##RXIPC##bits_t &RXIPCbits = IPC##RXIPC##bits; \
    volatile __IPC##FaultIPC##bits_t &EIPCbits = IPC##FaultIPC##bits; \
    volatile __IPC##TXIPC##bits_t &TXIPCbits = IPC##TXIPC##bits; \
\
    CSPI##N() : \
        _readCallback(NULL), _readContext(NULL), _writeCallback(NULL), _writeContext(NULL) \
    {  \
        _spiObj = this; \
        PMD.SPI##N##MD = 0; \
    } \
    ~CSPI##N() \
    { \
        Disable(); \
        _spiObj = NULL; \
        PMD.SPI##N##MD = 1; \
    } \
    \
    void Initialize(bool master, uint32_t masterBaud, bool enhbuf, int dataSize = 8) \
    { \
        Disable(); \
        DisableFaultInterrupt(); \
        DisableRXInterrupt(); \
        DisableTXInterrupt(); \
        uint32_t dummy = SPI##N##BUF; \
        SPI##N##CONbits.ENHBUF = enhbuf; \
        SPI##N##CONbits.SRXISEL = 1; \
        FaultIFSCLR = IFS_FaultIF_MASK; \
        RXIFSCLR = IFS_RXIF_MASK; \
        TXIFSCLR = IFS_TXIF_MASK; \
        SPI##N##BRG = masterBaud; \
        SPI##N##STATbits.SPIROV = 0; \
        SPI##N##CONbits.MSTEN = master; \
        if (dataSize == 16) SPI##N##CONbits.MODE16 = 1; \
        else if (dataSize == 32) SPI##N##CONbits.MODE32 = 1; \
        else SPI##N##CONbits.MODE16 = SPI##N##CONbits.MODE32 = 0; \
    } \
    \
    void SetMode(int polarity, int phase) \
    { \
        SPI##N##CONbits.CKP = polarity; \
        /* Everyone on the web who says that CKE is just the inverse */ \
        /* of CPHA is talking about SPI Masters. When you're an SPI */ \
        /* slave, CKE should be equal to CPHA!! */ \
        SPI##N##CONbits.CKE = SPI##N##CONbits.MSTEN ? !phase : phase; \
    } \
    \
    void UseSPISelect(bool useSPISelect) \
    { \
        SPI##N##CONbits.SSEN = SPI##N##CONbits.MSSEN = useSPISelect ? 1 : 0; \
    } \
    \
    void Enable() {SPI##N##CONSET = _SPI##N##CON_ON_MASK;} \
    void Disable() {SPI##N##CONCLR = _SPI##N##CON_ON_MASK;} \
    \
    void EnableFaultInterrupt() {FaultIECSET = IEC_FaultIE_MASK;} \
    void DisableFaultInterrupt() {FaultIECCLR = IEC_FaultIE_MASK;} \
    \
    void EnableRXInterrupt() {RXIECSET = IEC_RXIE_MASK;} \
    void DisableRXInterrupt() {RXIECCLR = IEC_RXIE_MASK;} \
    \
    void EnableTXInterrupt() {TXIECSET = IEC_TXIE_MASK;} \
    void DisableTXInterrupt() {TXIECCLR = IEC_TXIE_MASK;} \
    \
    void SetInterruptPriorities() \
    { \
        RXIPCbits.SPI##N##RXIP = 1; \
        RXIPCbits.SPI##N##RXIS = 0; \
        EIPCbits.SPI##N##EIP = 1; \
        EIPCbits.SPI##N##EIS = 0; \
        TXIPCbits.SPI##N##TXIP = 1; \
        TXIPCbits.SPI##N##TXIS = 0; \
    } \
    \
    void SetDIPPS(SPI##N##DIPPS pin) {SDI##N##R = pin;} \
/*    void SetCKPPS(SPI##N##CKPPS pin) {SCK##N##R = pin;} Not used in PIC32MZ */ \
    void SetSSPPS(SPI##N##SSPPS pin) {SS##N##R = pin;} \
    \
    bool RXReady() {return !SPI##N##STATbits.SPIRBE;} \
    uint32_t RXData() {return SPI##N##BUF;} \
    \
    void RegisterReadCallback(void (*callback)(void *), void *context) {_readCallback = callback; _readContext = context;} \
    void UnregisterReadCallback() {_readCallback = NULL;} \
    \
    void RXHandler() {if (_readCallback) (*_readCallback)(_readContext);} \
    \
    bool TXReady() {return !SPI##N##STATbits.SPITBF;} \
    void TXData(uint32_t data) {SPI##N##BUF = data;} \
    \
    void RegisterWriteCallback(void (*callback)(void *), void *context) {_writeCallback = callback; _writeContext = context;} \
    void UnregisterWriteCallback() {_writeCallback = NULL;} \
    \
    void TXHandler() {if (_writeCallback) (*_writeCallback)(_writeContext);} \
    \
    static CSPI##N *_spiObj; \
    \
private: \
    void (*_readCallback)(void *context); \
    void *_readContext; \
    void (*_writeCallback)(void *context); \
    void *_writeContext; \
    \
    CSPI##N(const CSPI##N& orig); \
};

#define SPICLASS(N, PMDReg, FaultIECIFS, RXIECIFS, TXIECIFS, FaultIPC, RXIPC, TXIPC) _SPICLASS(N, PMDReg, FaultIECIFS, RXIECIFS, TXIECIFS, FaultIPC, RXIPC, TXIPC)

SPICLASS(2, SPI2PMD, SPI2FaultI, SPI2RXI, SPI2TXI, SPI2FaultIPC, SPI2RXIPC, SPI2TXIPC)
SPICLASS(4, SPI4PMD, SPI4FaultI, SPI4RXI, SPI4TXI, SPI4FaultIPC, SPI4RXIPC, SPI4TXIPC)

#endif	/* SPI_H */

