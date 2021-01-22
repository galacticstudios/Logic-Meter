/* 
 * File:   ToolPWM.cpp
 * Author: Bob
 * 
 * Created on April 25, 2020, 9:50 AM
 */

extern "C"
{
#include "definitions.h"
}
#include "Settings.h"
#include "PPS.h"
#include "ToolServo.h"
#include "Menu.h"
#include "Display.h"

static const Help help("Servo Out", NULL, NULL, 
        "Outputs a 50 Hz signal compatible with most servos, with adjustable pulse width.");

static const MenuItem modifyMenuItems[5] = {
    MenuItem(UTF8_UPARROW, MenuType::NoChange, nullptr, CB(&ToolServo::Up)), 
    MenuItem(UTF8_DOWNARROW, MenuType::NoChange, nullptr, CB(&ToolServo::Down)), 
    MenuItem(UTF8_LEFTARROW, MenuType::NoChange, nullptr, CB(&ToolServo::Left)), 
    MenuItem(UTF8_RIGHTARROW, MenuType::NoChange, nullptr, CB(&ToolServo::Right)), 
    MenuItem("Done", MenuType::ParentMenu, nullptr, CB(&ToolServo::Done))};

static const Menu modifyMenu(modifyMenuItems);

static const MenuItem menuItems[5] = {
    MenuItem("Pulse", MenuType::ChildMenu, &modifyMenu, CB(&ToolServo::OnPulseWidth)),
    MenuItem(), 
    MenuItem(), 
    MenuItem(), 
    MenuItem()};

static const Menu menu(menuItems);

const long long servoDuty = (long long) (1.5 / 20 * 100 * fixed::scale);

ToolServo::ToolServo() :
    ToolPWMBase("Servo", new SquareWavePane(50, fixed(settings.servoDuty, true), {0, 1, 0, 1}), menu, help),
    _duty(settings.servoDuty, true)
{
    // This uses a virtual function, so it needs to be called from here,
    // not the base class c'tor
    UpdatePWM();
}

ToolServo::~ToolServo() 
{
}

fixed ToolServo::GetPeriod() const
{
    fixed period = TMR2_FrequencyGet() / 50;
    return period;
}

void ToolServo::OnPulseWidth()
{
    GetPane()->EnterSetPulseWidth();
}

void ToolServo::Up()
{
    GetPane()->Up();
    _duty = GetPane()->DutyCycle();
    settings.servoDuty = _duty.raw();
    SettingsModified();
    UpdatePWM();
}

void ToolServo::Down()
{
    GetPane()->Down();
    _duty = GetPane()->DutyCycle();
    settings.servoDuty = _duty.raw();
    SettingsModified();
    UpdatePWM();
}

void ToolServo::Left()
{
    GetPane()->Left();
}

void ToolServo::Right()
{
    GetPane()->Right();
}

void ToolServo::Done()
{
    GetPane()->Done();
}

