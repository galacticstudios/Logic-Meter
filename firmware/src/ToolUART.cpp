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
#include "UART.h"
#include "Menu.h"

static const Help help("UART In", NULL, NULL, 
        "Displays incoming UART data.");

extern const Menu uartBaud1;

static const MenuItem baud3Items[5] = {
    MenuItem("19200", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud19200Selected)), 
    MenuItem("38400", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud38400Selected)), 
    MenuItem("57600", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud57600Selected)), 
    MenuItem("115200", MenuType::ParentMenu, nullptr, CB(&ToolUART::Baud115200Selected)), 
    MenuItem("More" UTF8_RIGHTARROW, MenuType::SiblingMenu, &uartBaud1)};

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

const Menu uartBaud1(baud1Items);

static const MenuItem menuItems[5] = {
    MenuItem("Baud", MenuType::ChildMenu, &uartBaud1),
    MenuItem(),
    MenuItem(),
    MenuItem(),
    MenuItem()};

static const Menu menu(menuItems);

// Create a single terminal pane for the UART. Having a persistent pane means the
// user can switch to other tools, then back to the UART and not lose the data.
// But we can't have a static TerminalPane object because C would initialize it
// before Aria starts up, and the pane is dependent on Aria
static TerminalPane *UARTTerminalPane()
{
    static TerminalPane *terminalPane;
    if (terminalPane == NULL)
        terminalPane = new TerminalPane;
    return terminalPane;
}

ToolUART::ToolUART() :
    Tool("UART", UARTTerminalPane(), menu, help), _lastInputCaptureValid(false)
{
    UART3.RegisterReadCallback(&ToolUART::ByteReceived, this);

    UART3.SetInterruptPriorities();
    UART3.Initialize();
    UART_SERIAL_SETUP uss = {settings.uartBaud, UART_PARITY_NONE, UART_DATA_8_BIT, UART_STOP_1_BIT};
    UART3.SerialSetup(&uss, 0);
    UART3.SetRXPPS(RPD11); // UART 3 gets input from RPD11
    TRISDbits.TRISD11 = 1;
    UART3.EnableRXInterrupt();
    
    if (settings.uartAutobaud)
    {
        // Turn off the settings so that StartAutoBaudDetection thinks there's a change
        // in state
        settings.uartAutobaud = false;
        StartAutoBaudDetection();
    }
    else
        SetBaudRate(settings.uartBaud);
     
    GetPane()->Invalidate();
}

ToolUART::~ToolUART() 
{
    StopAutoBaudDetection();
    
    UART3.DisableRXInterrupt();
    /* Turn OFF UART3 */
    UART3.Disable();
    UART3.UnregisterReadCallback();
}

uint32_t ToolUART::TimerFrequency()
{
    uint32_t freq = TMR2_FrequencyGet();
    freq >>= T2CONbits.TCKPS;
    if (T2CONbits.TCKPS == 0x7)
        freq >>= 1;
    return freq;
}

uint32_t ics[1000], iccount;

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
        bool error = InputCapture4.Error();
        while (InputCapture4.DataReady())
        {
            uint32_t ic = InputCapture4.ReadData();
            if (iccount < 1000)
                ics[iccount++] = ic;
            
            if (_lastInputCaptureValid)
            {
                // If the timer has rolled over, don't use any IC values, because they might be really old
                if (IFS0bits.T3IF)
                {
                    IFS0bits.T3IF = 0;
                    while (InputCapture4.DataReady())
                        InputCapture4.ReadData();
                    _lastInputCaptureValid = false;
                }
                // Else (the timer has not rolled over since the last IC)
                else
                {
                    uint32_t baud = TimerFrequency() / (ic - _lastInputCapture);
                    if (baud > settings.uartBaud)
                        SetBaudRate(baud);
                }
            }
            else
                _lastInputCaptureValid = true;
            _lastInputCapture = ic;
        }
        
        // if the input capture buffer overflowed, the last IC we read might not
        // be the most recent signal that has come in to the pin. So mark it
        // as invalid
        if (error)
            _lastInputCaptureValid = false;
    }
    
    GetPane()->Update();
}

void ToolUART::ByteReceived(void *context) 
{
    while (UART3.RXReady())
        ((ToolUART *) context)->_receiveQueue.write(UART3.RXData());
}

void ToolUART::SetBaudRate(int baud)
{
    settings.uartBaud = baud;
    SettingsModified();
    UART_SERIAL_SETUP setup = {settings.uartBaud, UART_PARITY_NONE, UART_DATA_8_BIT, UART_STOP_1_BIT};
    UART3.SerialSetup(&setup, 0);
    
    char buf[10];
    sprintf(buf, "%d", settings.uartBaud);
    if (settings.uartAutobaud)
        strcat(buf, "*");
    SetStatusText(buf);
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
        InputCapture4.SetPPS(RPD11); // UART 3 gets input from RPD11, and so does IC4
        TMR2_Initialize();
        TMR2_PeriodSet(0xffffffff);
        TMR2_Start();
        InputCapture4.Initialize();
        InputCapture4.Enable();
        _lastInputCaptureValid = false;
    }
    SetBaudRate(110);
    SettingsModified();
}

void ToolUART::StopAutoBaudDetection()
{
    if (settings.uartAutobaud)
    {
        InputCapture4.Disable();
        TMR2_Stop();
        settings.uartAutobaud = false;
        SettingsModified();
    }
}


