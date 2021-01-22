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

static const MenuItem chipSelMenuItems[5] = {
    MenuItem("Always", MenuType::ParentMenu, NULL, CB(&ToolSPI::ToolSPI::SS0)),
    MenuItem("Pin B", MenuType::ParentMenu, NULL, CB(&ToolSPI::ToolSPI::SSB)),
    MenuItem(),
    MenuItem(),
    MenuItem("Cancel", MenuType::ParentMenu)};

static const Menu chipSelMenu(chipSelMenuItems);

static const MenuItem modeMenuItems[5] = {
    MenuItem("CPOL 0", MenuType::NoChange, NULL, CB(&ToolSPI::Polarity0)),
    MenuItem("CPOL 1", MenuType::NoChange, NULL, CB(&ToolSPI::Polarity1)),
    MenuItem("CPHA 0", MenuType::NoChange, NULL, CB(&ToolSPI::Phase0)),
    MenuItem("CPHA 1", MenuType::NoChange, NULL, CB(&ToolSPI::Phase1)),
    MenuItem("Done", MenuType::ParentMenu)};

static const Menu modeMenu(modeMenuItems);

static const MenuItem widthMenuItems[5] = {
    MenuItem("8 bits", MenuType::ParentMenu, NULL, CB(&ToolSPI::Width8)),
    MenuItem("16 bits", MenuType::ParentMenu, NULL, CB(&ToolSPI::Width16)),
    MenuItem("32 bits", MenuType::ParentMenu, NULL, CB(&ToolSPI::Width32)),
    MenuItem(),
    MenuItem("Cancel", MenuType::ParentMenu)};

static const Menu widthMenu(widthMenuItems);

static const MenuItem setupMenuItems[5] = {
    //MenuItem("Width", MenuType::ChildMenu, &widthMenu),
    MenuItem("Mode", MenuType::ChildMenu, &modeMenu),
    MenuItem("~CS", MenuType::ChildMenu, &chipSelMenu),
    MenuItem(),
    MenuItem(),
    MenuItem("Done", MenuType::ParentMenu)};

static const Menu spiSetupMenu(setupMenuItems);

static const MenuItem menuItems[5] = {
    MenuItem("Setup", MenuType::ChildMenu, &spiSetupMenu),
    MenuItem("Clear", MenuType::NoChange, nullptr, CB(&ToolSPI::Clear)),
    MenuItem(),
    MenuItem(UTF8_UPARROW, MenuType::NoChange, nullptr, CB(&ToolSPI::ScrollUp)),
    MenuItem(UTF8_DOWNARROW, MenuType::NoChange, nullptr, CB(&ToolSPI::ScrollDown))};

static const Menu menu(menuItems);

ToolSPI::ToolSPI() :
    Tool("SPI In", new TerminalPane(), menu, help)
{
    _spi.RegisterReadCallback(&ToolSPI::DataReceived, this);
    _spi.RegisterFaultCallback(&ToolSPI::SPIFault, this);

    _spi.Initialize(false, 0, true);
    _spi.SetInterruptPriorities();
    _spi.SetMode(settings.spiPolarity, settings.spiPhase);
    SDI4R = RPD11;
    SS4R = RPD4;
    _spi.UseSPISelect(settings.spiUseSelect);
    TRISDbits.TRISD11 = 1;
    TRISDbits.TRISD10 = 1;
    TRISDbits.TRISD4 = 1;
    _spi.EnableRXInterrupt();
    _spi.EnableFaultInterrupt();
    _spi.Enable();
    
    DisplayStatus();
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
    {
        uint32_t data = spi->_spi.RXData();
        spi->_receiveQueue.writeUnsafe(data);
    }
}

void ToolSPI::SPIFault(void *context) 
{
    ToolSPI *spi = ((ToolSPI *) context);
    spi->_spi.ClearReceiveOverrun();
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
    char buf[20];
    sprintf(buf, /*"%2db; "*/ "CPOL %d; CPHA %d; ~SS=%c",
            /*settings.spiWidth,*/ settings.spiPolarity, settings.spiPhase, settings.spiUseSelect ? 'B' : '0');
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

void ToolSPI::Width(int width)
{
    if (settings.spiWidth != width)
    {
        settings.spiWidth = uint8_t(width);
        SettingsModified();
        _spi.Disable();
        _spi.SetWidth(width);
        _spi.Enable();
        DisplayStatus();
    }
}

void ToolSPI::Clear()
{
    GetPane()->Clear();
}

void ToolSPI::ScrollUp()
{
    GetPane()->ScrollUp();
}

void ToolSPI::ScrollDown()
{
    GetPane()->ScrollDown();
}
