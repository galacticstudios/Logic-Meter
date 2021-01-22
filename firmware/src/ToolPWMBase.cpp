/* 
 * File:   ToolPWMBase.cpp
 * Author: Bob
 * 
 * Created on January 5, 2021, 4:00 PM
 */

extern "C"
{
#include "definitions.h"
}
#include "ToolPWMBase.h"
#include "PPS.h"

ToolPWMBase::ToolPWMBase(const char *title, Pane *pane, const Menu &menu, const Help &help)  :
    Tool(title, pane, menu, help)
{
}

ToolPWMBase::~ToolPWMBase() 
{
    TMR2_Stop();
    OCMP4_Disable();
    RPD11R = (uint32_t) PPSGroup2Outputs::O2OFF;
    TRISDbits.TRISD11 = 1;
}

void ToolPWMBase::UpdatePWM()
{
    TMR2_Stop();
    OCMP4_Disable();
    TMR2_Initialize();
    OCMP4_Initialize();
   
    fixed period = GetPeriod();
    TMR2_PeriodSet(int(period) - 1);
    fixed ratio = GetDuty() / 100;
    fixed pulse = period * ratio;
    OCMP4_CompareValueSet(0);
    OCMP4_CompareSecondaryValueSet(int(pulse));
    
    OCMP4_Enable();
    TMR2_Start();
    
    RPD11R = (uint32_t) PPSGroup2Outputs::OC4; // OC4 outputs to RPD11
    TRISDbits.TRISD11 = 0;
}