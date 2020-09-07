/* 
 * File:   ToolSPI.cpp
 * Author: Bob
 * 
 * Created on July 13, 2020, 8:28 AM
 */

extern "C"
{
#include "definitions.h"
}
#include "Settings.h"
#include "TerminalPane.h"
#include "Display.h"
#include "Utility.h"
#include "ToolSPI.h"
#include "SPI.h"
#include "Menu.h"

static const Help help("SPI In", "SPI Clock", "SPI Select (optional)", 
        "Displays incoming SPI data.");

extern const Menu spiMenu;

static const MenuItem menu2Items[5] = {
    MenuItem("CPOL 0", MenuType::NoChange, NULL, CB(&ToolSPI::Polarity0)),
    MenuItem("CPOL 1", MenuType::NoChange, NULL, CB(&ToolSPI::Polarity1)),
    MenuItem("CPHA 0", MenuType::NoChange, NULL, CB(&ToolSPI::Phase0)),
    MenuItem("CPHA 1", MenuType::NoChange, NULL, CB(&ToolSPI::Phase1)),
    MenuItem("~SS=0", MenuType::SiblingMenu, &spiMenu, CB(&ToolSPI::SS0))};

static const Menu menu2(menu2Items);

static const MenuItem menuItems[5] = {
    MenuItem("CPOL 0", MenuType::NoChange, NULL, CB(&ToolSPI::Polarity0)),
    MenuItem("CPOL 1", MenuType::NoChange, NULL, CB(&ToolSPI::Polarity1)),
    MenuItem("CPHA 0", MenuType::NoChange, NULL, CB(&ToolSPI::Phase0)),
    MenuItem("CPHA 1", MenuType::NoChange, NULL, CB(&ToolSPI::Phase1)),
    MenuItem("~SS=B", MenuType::SiblingMenu, &menu2, CB(&ToolSPI::SSB))};

const Menu spiMenu(menuItems);

// Create a single terminal pane for the SPI. Having a persistent pane means the
// user can switch to other tools, then back to the SPI and not lose the data.
// But we can't have a static TerminalPane object because C would initialize it
// before Aria starts up, and the pane is dependent on Aria
static TerminalPane *SPITerminalPane()
{
    static TerminalPane *terminalPane;
    if (terminalPane == NULL)
        terminalPane = new TerminalPane(true);
    return terminalPane;
}

ToolSPI::ToolSPI() :
    Tool("SPI", SPITerminalPane(), spiMenu, help)
{
    _spi.RegisterReadCallback(&ToolSPI::DataReceived, this);

    _spi.Initialize(false, 0, true);
    _spi.SetInterruptPriorities();
    _spi.SetMode(settings.spiPolarity, settings.spiPhase);
    _spi.SetDIPPS(RPD11);
    _spi.SetSSPPS(RPD4);
    _spi.UseSPISelect(settings.spiUseSelect);
    TRISDbits.TRISD11 = 1;
    TRISDbits.TRISD10 = 1;
    TRISDbits.TRISD4 = 1;
    _spi.EnableRXInterrupt();
    _spi.Enable();
    
    DisplayStatus();
     
    GetPane()->Invalidate();
}

ToolSPI::~ToolSPI() 
{
    _spi.Disable();
    _spi.DisableRXInterrupt();
    _spi.UnregisterReadCallback();
}

void ToolSPI::OnIdle()
{
    uint8_t data[100];
    size_t size;
    while ((size = _receiveQueue.read(data, countof(data))) != 0)
    {
        GetPane()->AddText((char *) data, size);
    }
    
    GetPane()->Update();
}

void ToolSPI::DataReceived(void *context) 
{
    ToolSPI *spi = ((ToolSPI *) context);
    while (spi->_spi.RXReady())
        spi->_receiveQueue.write(spi->_spi.RXData());
}

void ToolSPI::Polarity0()
{
    if (settings.spiPolarity)
    {
        _spi.Disable();
        settings.spiPolarity = 0;
        SettingsModified();
        _spi.SetMode(settings.spiPolarity, settings.spiPhase);
        _spi.Enable();
        DisplayStatus();
    }
}

void ToolSPI::Polarity1()
{
    if (!settings.spiPolarity)
    {
        _spi.Disable();
        settings.spiPolarity = 1;
        SettingsModified();
        _spi.SetMode(settings.spiPolarity, settings.spiPhase);
        _spi.Enable();
        DisplayStatus();
    }
}

void ToolSPI::Phase0()
{
    if (settings.spiPhase)
    {
        _spi.Disable();
        settings.spiPhase = 0;
        SettingsModified();
        _spi.SetMode(settings.spiPolarity, settings.spiPhase);
        _spi.Enable();
        DisplayStatus();
    }
}

void ToolSPI::Phase1()
{
    if (!settings.spiPhase)
    {
        _spi.Disable();
        settings.spiPhase = 1;
        SettingsModified();
        _spi.SetMode(settings.spiPolarity, settings.spiPhase);
        _spi.Enable();
        DisplayStatus();
    }
}

void ToolSPI::DisplayStatus()
{
    static char buf[] = "CPOL x; CPHA y; ~SS=z";
    buf[5] = settings.spiPolarity + '0';
    buf[13] = settings.spiPhase + '0';
    buf[20] = settings.spiUseSelect ? 'B' : '0';
    SetStatusText(buf);
}

// Set the SPI select to come from the Aux2 pin
void ToolSPI::SSB()
{
    if (!settings.spiUseSelect)
    {
        settings.spiUseSelect = 1;
        SettingsModified();
        _spi.Disable();
        _spi.UseSPISelect(true);
        _spi.Enable();
        DisplayStatus();
    }
}

// Set the SPI select to always be 0 (so we're always selected)
void ToolSPI::SS0()
{
    if (settings.spiUseSelect)
    {
        settings.spiUseSelect = 0;
        SettingsModified();
        _spi.Disable();
        _spi.UseSPISelect(false);
        _spi.Enable();
        DisplayStatus();
    }
}
