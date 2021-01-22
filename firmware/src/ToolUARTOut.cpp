/* 
 * File:   ToolUARTOut.cpp
 * Author: Bob
 * 
 * Created on September 30, 2020, 12:22 PM
 */

extern "C" 
{
#include "definitions.h"
}
#include <vector>
#include <string>
#include <algorithm>
#include "Settings.h"
#include "ToolUARTOut.h"
#include "Menu.h"
#include "PPS.h"
#include "Display.h"
#include "TerminalPane.h"

static const Help help(NULL, "UART Out", NULL, 
    "Outputs UART data.");

extern const Menu uartOutBaud1;

static const MenuItem selectionItems[5] = {
    MenuItem(UTF8_UPARROW, MenuType::NoChange, nullptr, CB(&ToolUARTOut::SelectionUp)), 
    MenuItem(UTF8_DOWNARROW, MenuType::NoChange, nullptr, CB(&ToolUARTOut::SelectionDown)),
    MenuItem(),
    MenuItem("Cancel", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::SelectionCancel)), 
    MenuItem("OK", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::SelectionOK))};

static const Menu selectionMenu(selectionItems);

static const MenuItem baud3Items[5] = {
    MenuItem("38400", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Baud38400Selected)), 
    MenuItem("57600", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Baud57600Selected)), 
    MenuItem("115200", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Baud115200Selected)), 
    MenuItem(),
    MenuItem("More" UTF8_RIGHTARROW, MenuType::SiblingMenu, &uartOutBaud1)};

static const Menu uartBaud3(baud3Items);

static const MenuItem baud2Items[5] = {
    MenuItem("4800", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Baud4800Selected)), 
    MenuItem("9600", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Baud9600Selected)), 
    MenuItem("14400", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Baud14400Selected)), 
    MenuItem("19200", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Baud19200Selected)), 
    MenuItem("More" UTF8_RIGHTARROW, MenuType::SiblingMenu, &uartBaud3)};

static const Menu uartBaud2(baud2Items);

static const MenuItem baud1Items[5] = {
    MenuItem("110", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Baud110Selected)), 
    MenuItem("300", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Baud300Selected)), 
    MenuItem("1200", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Baud1200Selected)), 
    MenuItem("More" UTF8_RIGHTARROW, MenuType::SiblingMenu, &uartBaud2)};

const Menu uartOutBaud1(baud1Items);

extern const Menu uartOutRunningMenu;

static const MenuItem resumeMenuItems[5] = {
    MenuItem("Resume", MenuType::SiblingMenu, &uartOutRunningMenu, CB(&ToolUARTOut::Resume)),
    MenuItem("Stop", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Stop)),
    MenuItem(),
    MenuItem(),
    MenuItem()};

const Menu resumeMenu(resumeMenuItems);

static const MenuItem runningMenuItems[5] = {
    MenuItem("Pause", MenuType::SiblingMenu, &resumeMenu, CB(&ToolUARTOut::Pause)),
    MenuItem("Stop", MenuType::ParentMenu, nullptr, CB(&ToolUARTOut::Stop)),
    MenuItem(),
    MenuItem(),
    MenuItem()};

const Menu uartOutRunningMenu(runningMenuItems);

static const MenuItem menuItems[5] = {
    MenuItem("Baud", MenuType::ChildMenu, &uartOutBaud1),
    MenuItem("Data", MenuType::ChildMenu, &selectionMenu, CB(&ToolUARTOut::SelectData)),
    MenuItem("Start", MenuType::ChildMenu, &uartOutRunningMenu, CB(&ToolUARTOut::Start)),
    MenuItem("Loop", MenuType::ChildMenu, &uartOutRunningMenu, CB(&ToolUARTOut::Loop)),
    MenuItem()};

static const Menu menu(menuItems);

ToolUARTOut::ToolUARTOut() :
    ToolDataOut("UART Out", menu, help)
{
    _uart.Initialize();
    UARTSerialSetup uss = {settings.gpsBaud, UARTSerialSetup::UART8BitParityNone, 1};
    _uart.SerialSetup(&uss, 0);
    SetBaudRate(settings.gpsBaud);
    RPD10RPPS = PPSGroup1Outputs::U3TX;
    TRISDbits.TRISD10 = 0;
}

ToolUARTOut::~ToolUARTOut() 
{
    _uart.Disable();
    RPD10RPPS = PPSGroup1Outputs::O1OFF;
    TRISDbits.TRISD10 = 1;
}

void ToolUARTOut::SetBaudRate(int baud)
{
    settings.uartOutBaud = baud;
    SettingsModified();
    UARTSerialSetup setup = {settings.uartOutBaud, UARTSerialSetup::UART8BitParityNone, 1};
    _uart.SerialSetup(&setup, 0);
    
    char buf[10];
    sprintf(buf, "%d", settings.uartOutBaud);
    SetStatusText(buf);
}

void ToolUARTOut::BaudSelected(int baud)
{
    SetBaudRate(baud);
}

char *ToolUARTOut::FileName()
{
    return settings.uartOutFile;
}

void ToolUARTOut::SetFileName(const char *fileName)
{
    strncpy(settings.uartOutFile, fileName, sizeof(settings.uartOutFile));
    settings.uartOutFile[sizeof(settings.uartOutFile) - 1] = '\0';
    SettingsModified();
}

