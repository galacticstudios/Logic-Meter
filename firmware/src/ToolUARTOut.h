/* 
 * File:   ToolUARTOut.h
 * Author: Bob
 *
 * Created on September 30, 2020, 12:22 PM
 */

#ifndef TOOLUARTOUT_H
#define	TOOLUARTOUT_H

#include "ToolDataOut.h"
#include "Utility.h"
#include "UART.h"

class TerminalPane;

class ToolUARTOut : public ToolDataOut
{
public:
    static Tool *Factory() {return new ToolUARTOut;}

    ToolUARTOut();
    virtual ~ToolUARTOut();
    
    void Baud110Selected() {BaudSelected(110);}
    void Baud300Selected() {BaudSelected(300);}
    void Baud1200Selected() {BaudSelected(1200);}
    void Baud2400Selected() {BaudSelected(2400);}
    void Baud4800Selected() {BaudSelected(4800);}
    void Baud9600Selected() {BaudSelected(9600);}
    void Baud14400Selected() {BaudSelected(14400);}
    void Baud19200Selected() {BaudSelected(19200);}
    void Baud38400Selected() {BaudSelected(38400);}
    void Baud57600Selected() {BaudSelected(57600);}
    void Baud115200Selected() {BaudSelected(115200);}

protected:
    virtual bool TXReady() const {return _uart.TXReady();}
    virtual void TXData(uint32_t data) {_uart.TXData(data);}
    virtual char *FileName();
    virtual void SetFileName(const char *fileName);

private:
    ToolUARTOut(const ToolUARTOut& orig);
    
    void BaudSelected(int baud);
    void SetBaudRate(int baud);
     
    UART<3> _uart;
};

#endif	/* TOOLUARTOUT_H */

