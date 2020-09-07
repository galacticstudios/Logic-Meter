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
    
    void Polarity0();
    void Polarity1();
    void Phase0();
    void Phase1();
    void SSB();
    void SS0();
    
private:
    ToolSPI(const ToolSPI& orig);

    TerminalPane *GetPane() const {return (TerminalPane *) Tool::GetPane();}
    
    void DisplayStatus();
    
    friend void SPICallback(uintptr_t);
    void SPICallback();
    
    CSPI4 _spi;
    CQueue<uint8_t, 20000> _receiveQueue;
};

#endif	/* TOOLSPI_H */

