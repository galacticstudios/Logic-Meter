/* 
 * File:   ToolUART.h
 * Author: Bob
 *
 * Created on May 18, 2020, 4:46 PM
 */

#ifndef TOOLUART_H
#define	TOOLUART_H

#include "Tool.h"
#include "Utility.h"

class TerminalPane;

static void UARTCallback(uintptr_t context);

class ToolUART : public Tool
{
public:
    static Tool *Factory() {return new ToolUART;}
    
    ToolUART();
    virtual ~ToolUART();
    
    void OnIdle();
    
    static void ByteReceived(void *context);
    
    void AutoBaudSelected();
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
    
private:
    ToolUART(const ToolUART& orig);

    uint32_t TimerFrequency();
    
    void BaudSelected(int baud);
    void SetBaudRate(int baud);
    
    void StartAutoBaudDetection();
    void StopAutoBaudDetection();
    
    TerminalPane *GetPane() const {return (TerminalPane *) Tool::GetPane();}
    
    friend void UARTCallback(uintptr_t);
    void UARTCallback();
    
    uint32_t _lastInputCapture;
    bool _lastInputCaptureValid;
    
    CQueue<uint8_t, 20000> _receiveQueue;
};

#endif	/* TOOLUART_H */

