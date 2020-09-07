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

ToolPWM::ToolPWM() :
    Tool("PWM", new SquareWavePane(fixed(settings.pwmHertz, true), fixed(settings.pwmDuty, true), {1, 1, 1, 1}), menu, help),
    _hertz(settings.pwmHertz, true), _duty(settings.pwmDuty, true)
{
    UpdatePWM();
}

ToolPWM::~ToolPWM() 
{
    TMR2_Stop();
    OCMP3_Disable();
    RPF4R = O1OFF;
    TRISFbits.TRISF4 = 1;
    delete GetPane();
}

void ToolPWM::UpdatePWM()
{
    TMR2_Stop();
    OCMP3_Disable();
    TMR2_Initialize();
    OCMP3_Initialize();
    
    // Set the the period
    fixed period;
    if (_hertz)
        period = TMR2_FrequencyGet() / _hertz;
    else
        period = 0;
    TMR2_PeriodSet(int(period) - 1);
    fixed ratio = _duty / 100;
    fixed pulse = period * ratio;
    OCMP3_CompareValueSet(0);
    OCMP3_CompareSecondaryValueSet(int(pulse));
    
    OCMP3_Enable();
    TMR2_Start();
    
    RPF4R = OC3; // OC3 outputs to RPF4
    TRISFbits.TRISF4 = 0;
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

