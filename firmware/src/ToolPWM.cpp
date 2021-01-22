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
#include "ToolPWM.h"
#include "Menu.h"
#include "Display.h"

static const Help help("PWM Out", NULL, NULL, 
        "Outputs an adjustable pulse width modulated signal.");

static const MenuItem modifyMenuItems[5] = {
    MenuItem(UTF8_UPARROW, MenuType::NoChange, nullptr, CB(&ToolPWM::Up)), 
    MenuItem(UTF8_DOWNARROW, MenuType::NoChange, nullptr, CB(&ToolPWM::Down)), 
    MenuItem(UTF8_LEFTARROW, MenuType::NoChange, nullptr, CB(&ToolPWM::Left)), 
    MenuItem(UTF8_RIGHTARROW, MenuType::NoChange, nullptr, CB(&ToolPWM::Right)), 
    MenuItem("Done", MenuType::ParentMenu, nullptr, CB(&ToolPWM::Done))};

static const Menu modifyMenu(modifyMenuItems);

static const MenuItem menuItems[5] = {
    MenuItem("Freq", MenuType::ChildMenu, &modifyMenu, CB(&ToolPWM::OnFreq)), 
    MenuItem("Period", MenuType::ChildMenu, &modifyMenu, CB(&ToolPWM::OnPeriod)), 
    MenuItem("Duty", MenuType::ChildMenu, &modifyMenu, CB(&ToolPWM::OnDuty)), 
    MenuItem("Pulse", MenuType::ChildMenu, &modifyMenu, CB(&ToolPWM::OnPulseWidth)),
    MenuItem()};

static const Menu menu(menuItems);

const long long pwmHertz = 1000 * fixed::scale, pwmDuty = 50 * fixed::scale;

ToolPWM::ToolPWM() :
    ToolPWMBase("PWM", new SquareWavePane(fixed(settings.pwmHertz, true), fixed(settings.pwmDuty, true), {1, 1, 1, 1}), menu, help),
    _hertz(settings.pwmHertz, true), _duty(settings.pwmDuty, true)
{
    // This uses a virtual function, so it needs to be called from here,
    // not the base class c'tor
    UpdatePWM();
}

ToolPWM::~ToolPWM() 
{
}

fixed ToolPWM::GetPeriod() const
{
    // Set the the period
    fixed period;
    if (_hertz)
        period = TMR2_FrequencyGet() / _hertz;
    else
        period = 0;
    return period;
 }

void ToolPWM::OnFreq()
{
    GetPane()->EnterSetFrequency();
}

void ToolPWM::OnPeriod()
{
    GetPane()->EnterSetPeriod();
}

void ToolPWM::OnDuty()
{
    GetPane()->EnterSetDutyCycle();
}

void ToolPWM::OnPulseWidth()
{
    GetPane()->EnterSetPulseWidth();
}

void ToolPWM::Up()
{
    GetPane()->Up();
    _hertz = GetPane()->Hertz();
    _duty = GetPane()->DutyCycle();
    settings.pwmHertz = _hertz.raw();
    settings.pwmDuty = _duty.raw();
    SettingsModified();
    UpdatePWM();
}

void ToolPWM::Down()
{
    GetPane()->Down();
    _hertz = GetPane()->Hertz();
    _duty = GetPane()->DutyCycle();
    settings.pwmHertz = _hertz.raw();
    settings.pwmDuty = _duty.raw();
    SettingsModified();
    UpdatePWM();
}

void ToolPWM::Left()
{
    GetPane()->Left();
}

void ToolPWM::Right()
{
    GetPane()->Right();
}

void ToolPWM::Done()
{
    GetPane()->Done();
}

