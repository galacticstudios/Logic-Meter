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

ToolServo::ToolServo() :
    Tool("Servo", new SquareWavePane(50, fixed(settings.servoDuty, true), {0, 1, 0, 1}), menu, help),
    _duty(settings.servoDuty, true)
{
    UpdateServo();
}

ToolServo::~ToolServo() 
{
    TMR2_Stop();
    OCMP3_Disable();
    RPF4R = O1OFF;
    TRISFbits.TRISF4 = 1;
    delete GetPane();
}

void ToolServo::UpdateServo()
{
    TMR2_Stop();
    OCMP3_Disable();
    TMR2_Initialize();
    OCMP3_Initialize();
    
    // Set the the period
    fixed period = TMR2_FrequencyGet() / 50;
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
    UpdateServo();
}

void ToolServo::Down()
{
    GetPane()->Down();
    _duty = GetPane()->DutyCycle();
    settings.servoDuty = _duty.raw();
    SettingsModified();
    UpdateServo();
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

