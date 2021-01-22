/* 
 * File:   ToolUART.cpp
 * Author: Bob
 * 
 * Created on May 18, 2020, 4:46 PM
 */

extern "C"
{
#include "definitions.h"
}
#include "Settings.h"
#include "InputCapture.h"
#include "TerminalPane.h"
#include "Display.h"
#include "Utility.h"
#include "ToolUART.h"
#include "Menu.h"

static const Help help("UART In", NULL, NULL, 
        "Displays incoming UART data.");

extern const Menu uartInBaud1;

static const MenuItem baud3Items[5] = {
    MenuItem("19200", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud19200Selected)), 
    MenuItem("38400", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud38400Selected)), 
    MenuItem("57600", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud57600Selected)), 
    MenuItem("115200", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud115200Selected)), 
    MenuItem("More" UTF8_RIGHTARROW, MenuType::SiblingMenu, &uartInBaud1)};

static const Menu uartBaud3(baud3Items);

static const MenuItem baud2Items[5] = {
    MenuItem("2400", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud2400Selected)), 
    MenuItem("4800", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud4800Selected)), 
    MenuItem("9600", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud9600Selected)), 
    MenuItem("14400", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud14400Selected)), 
    MenuItem("More" UTF8_RIGHTARROW, MenuType::SiblingMenu, &uartBaud3)};

static const Menu uartBaud2(baud2Items);

static const MenuItem baud1Items[5] = {
    MenuItem("Auto", MenuType::ParentMenu, nullptr, CB(&ToolUART::AutoBaudSelected)), 
    MenuItem("110", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud110Selected)), 
    MenuItem("300", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud300Selected)), 
    MenuItem("1200", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud1200Selected)), 
    MenuItem("More" UTF8_RIGHTARROW, MenuType::SiblingMenu, &uartBaud2)};

const Menu uartInBaud1(baud1Items);

static const MenuItem menuItems[5] = {
    MenuItem("Baud", MenuType::ChildMenu, &uartInBaud1),
    MenuItem("Clear", MenuType::NoChange, nullptr, CB(&ToolUART::Clear)),
    MenuItem(),
    MenuItem(UTF8_UPARROW, MenuType::NoChange, nullptr, CB(&ToolUART::ScrollUp)),
    MenuItem(UTF8_DOWNARROW, MenuType::NoChange, nullptr, CB(&ToolUART::ScrollDown))};

static const Menu menu(menuItems);

ToolUART::ToolUART() :
    Tool("UART In", new TerminalPane, menu, help), _lastInputCaptureValid(false)
{
    _uart.RegisterReadCallback(&ToolUART::ByteReceived, this);

    _uart.SetInterruptPriorities();
    _uart.Initialize();
    UARTSerialSetup uss = {settings.uartBaud, UARTSerialSetup::UART8BitParityNone, 1};
    _uart.SerialSetup(&uss, 0);
    U3RXR = RPD11; // UART 3 gets input from RPD11
    TRISDbits.TRISD11 = 1;
    _uart.EnableRXInterrupt();
    
    if (settings.uartAutobaud)
    {
        // Turn off the settings so that StartAutoBaudDetection thinks there's a change
        // in state
        settings.uartAutobaud = false;
        StartAutoBaudDetection();
    }
    else
        SetBaudRate(settings.uartBaud);
}

ToolUART::~ToolUART() 
{
    StopAutoBaudDetection();
    
    _uart.DisableRXInterrupt();
    /* Turn OFF _uart */
    _uart.Disable();
    _uart.UnregisterReadCallback();
}

uint32_t ToolUART::TimerFrequency()
{
    uint32_t freq = TMR2_FrequencyGet();
    freq >>= T2CONbits.TCKPS;
    if (T2CONbits.TCKPS == 0x7)
        freq >>= 1;
    return freq;
}

void ToolUART::OnIdle()
{
    uint8_t data[100];
    size_t size;
    while ((size = _receiveQueue.read(data, countof(data))) != 0)
    {
        GetPane()->AddText((char *) data, size);
    }
    
    if (settings.uartAutobaud)
    {
        bool error = _ic.Error();
        while (_ic.DataReady())
        {
            uint32_t newCapture = _ic.ReadData();
            
            if (_lastInputCaptureValid)
            {
                // If the timer has rolled over, don't use any IC values, because they might be really old
                // (We're using TMR3:TMR2 as a 32 bit pair, so we look at TMR3's interrupt flag)a
                if (IFS0bits.T3IF)
                {
                    IFS0bits.T3IF = 0;
                    while (_ic.DataReady())
                        _ic.ReadData();
                    _lastInputCaptureValid = false;
                }
                // Else (the timer has not rolled over since the last IC)
                else
                {
                    uint32_t baud = TimerFrequency() / (newCapture - _lastInputCapture);
                    if (baud > settings.uartBaud)
                        SetBaudRate(baud);
                }
            }
            else
                _lastInputCaptureValid = true;
            _lastInputCapture = newCapture;
        }
        
        // if the input capture buffer overflowed, the last IC we read might not
        // be the most recent signal that has come in to the pin. So mark it
        // as invalid
        if (error)
            _lastInputCaptureValid = false;
    }
    
    GetPane()->Update();
}

void ToolUART::Clear()
{
    GetPane()->Clear();
}

void ToolUART::ByteReceived(void *context) 
{
    ToolUART *toolUART = (ToolUART *) context;
    while (toolUART->_uart.RXReady())
        toolUART->_receiveQueue.write(toolUART->_uart.RXData());
}

void ToolUART::SetBaudRate(int baud)
{
    settings.uartBaud = baud;
    SettingsModified();
    UARTSerialSetup setup = {settings.uartBaud, UARTSerialSetup::UART8BitParityNone, 1};
    _uart.SerialSetup(&setup, 0);
    
    char buf[10];
    sprintf(buf, "%d", settings.uartBaud);
    if (settings.uartAutobaud)
        strcat(buf, "*");
    SetStatusText(buf);
}

void ToolUART::ScrollUp()
{
    GetPane()->ScrollUp();
}

void ToolUART::ScrollDown()
{
    GetPane()->ScrollDown();
}

void ToolUART::AutoBaudSelected()
{
    StartAutoBaudDetection();
}

void ToolUART::BaudSelected(int baud)
{
    StopAutoBaudDetection();
    SetBaudRate(baud);
    SettingsModified();
}

void ToolUART::StartAutoBaudDetection()
{
    if (!settings.uartAutobaud)
    {
        settings.uartAutobaud = true;
        IFS0bits.T2IF = 0;
        IC4R = RPD11; // UART 3 gets input from RPD11, and so does IC4
        TMR2_Initialize();
        TMR2_PeriodSet(0xffffffff);
        TMR2_Start();
        _ic.Initialize(_ic.EveryEdge, _ic.Timer32Bit);
        _ic.Enable();
        while (_ic.DataReady())
            _ic.ReadData();
        _lastInputCaptureValid = false;
    }
    SetBaudRate(110);
    SettingsModified();
}

void ToolUART::StopAutoBaudDetection()
{
    if (settings.uartAutobaud)
    {
        _ic.Disable();
        TMR2_Stop();
        settings.uartAutobaud = false;
        SettingsModified();
    }
}


