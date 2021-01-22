/* 
 * File:   ToolSPI.h
 * Author: Bob
 *
 * Created on July 13, 2020, 8:28 AM
 */

#ifndef TOOLSPI_H
#define	TOOLSPI_H

#include "Tool.h"
#include "SPI.h"
#include "Utility.h"

class TerminalPane;

class ToolSPI : public Tool
{
public:
    static Tool *Factory() {return new ToolSPI;}
    
    ToolSPI();
    virtual ~ToolSPI();

    void OnIdle();
    
    static void DataReceived(void *context);
    static void SPIFault(void *context);
    
    void Polarity0();
    void Polarity1();
    void Phase0();
    void Phase1();
    void SSB();
    void SS0();
    void Width8() {Width(8);}
    void Width16() {Width(16);}
    void Width32() {Width(32);}
    
    void Clear();
    
    void ScrollUp();
    void ScrollDown();
    
private:
    ToolSPI(const ToolSPI& orig);

    TerminalPane *GetPane() const {return (TerminalPane *) Tool::GetPane();}
    
    void DisplayStatus();
    
    void Width(int width);
    
    friend void SPICallback(uintptr_t);
    void SPICallback();
    
    SPI<4> _spi;
    CQueue<uint8_t, 10000> _receiveQueue;
};

#endif	/* TOOLSPI_H */

