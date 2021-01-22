/* 
 * File:   LogicMeter.cpp
 * Author: Bob
 * 
 * Created on April 24, 2020, 5:38 PM
 */

#include <cstddef>
extern "C"
{
#include "definitions.h"
}
#include "Utility.h"
#include "GPIO.h"
#include "LogicMeter.h"
#include "Display.h"
#include "Oscillator.h"
#include "Settings.h"
#include "Tool.h"
#include "ToolGPS.h"
#include "ToolLED.h"
#include "ToolLogicAnalyzer.h"
#include "ToolPWM.h"
#include "ToolServo.h"
#include "ToolSPI.h"
#include "ToolUART.h"
#include "ToolUARTOut.h"
#include "ToolUnimplemented.h"
#include "ToolUtility.h"

extern const uint8_t diskImage[];
volatile const uint8_t *forceLinkerToKeepDiskImage = diskImage;

Oscillator oscillator(DEVCFG2bits.UPLLFSEL ? 24000000 : 12000000, 0);

static const GPIO_PIN buttons[] = {Button1_PIN, Button2_PIN, Button3_PIN, 
    Button4_PIN, Button5_PIN, Button6_PIN};

static LogicMeter *lm;

typedef Tool *(*ToolFactory)();

ToolFactory toolFactories[] = {
    ToolServo::Factory,
    ToolPWM::Factory,
    ToolUART::Factory,
    ToolUARTOut::Factory,
    ToolSPI::Factory,
    ToolGPS::Factory,
//    ToolLED::Factory, // Version 7 hardware does not support this
    ToolLogicAnalyzer::Factory,
    ToolUnimplemented::Factory,
    ToolUtility::Factory,
};

//#define FORCE_TOOL 2

void LogicMeterStart()
{
    lm = new LogicMeter();   
}

void LogicMeterTask()
{
    lm->Task();
}

LogicMeter::LogicMeter() :
    _currentTool(NULL), _currentSelection(-1), _lastUserInteractionTime(SYS_TIME_CounterGet()), 
    _displayDimmed(true)
{
    display.Initialize();
    SettingsInitialize();
    
    // Turn on the PWM for the LCD backlight
    TMR4_Start();
    OCMP3_Enable();
}

LogicMeter::~LogicMeter() 
{
}

char exc[100];

void LogicMeter::Task()
{
    static bool firstTime = true;
    static int lastButtonPressed = -1;
    static uint32_t lastButtonPressedTime;
    static bool buttonFunctionCalled = false;
    
    if (firstTime)
    {
        printf("Copyright (C) 2021 by Robert E. Alexander.\r\nReleased under the MIT license.\r\n");
        firstTime = false;
    }
    
    try
    {
        int newSelection = ReadSelectorSwitch() - 1;
        
        if (newSelection >= 0)
        {
            if (_currentSelection != newSelection)
            {
                if (_currentTool)
                {
                    _currentTool->RequestDeactivate();
                }
                delete _currentTool;
                if (newSelection < countof(toolFactories))
                    _currentTool = (*toolFactories[newSelection])();
                else
                    _currentTool = ToolUnimplemented::Factory();
                if (_currentTool == nullptr) __builtin_software_breakpoint();
                _currentSelection = newSelection;
                UserInteraction();
            }
        }

        if (_currentTool)
        {
            int pressedButton = ReadButtons();        
            if (pressedButton != -1)
            {
                // Debounce
                uint32_t now = SYS_TIME_CounterGet();
                if (lastButtonPressed != pressedButton)
                {
                    lastButtonPressed = pressedButton;
                    lastButtonPressedTime = now;
                    buttonFunctionCalled = false;
                }
                else if (SYS_TIME_CountToMS(now - lastButtonPressedTime) > 100 && !buttonFunctionCalled)
                {
                    if (!_displayDimmed)
                    {
                        _currentTool->OnButtonPressed(pressedButton);
                    }
                    UserInteraction();
                    buttonFunctionCalled = true;
                }
            }
            else
                buttonFunctionCalled = false;

            _currentTool->OnIdle();
        }
        
        SettingsOnIdle();
        
        // If the display is dimmed, and we're not getting power from USB, and
        // there has been no user interaction for a while
        if (!_displayDimmed && 
            USBOTGbits.VBUS < 2 &&
            SYS_TIME_CountToMS(SYS_TIME_CounterGet() - _lastUserInteractionTime) > settings.screenDimMinutes * 60 * 1000)
        {
            // Dim the display
            _displayDimmed = true;
            OCMP3_CompareSecondaryValueSet(TMR4_PeriodGet() * 99 / 100);
        }
        
        // Else if we're getting power from USB
        else
        {
            // Pretend there's user interaction so that we keep the screen bright
            UserInteraction();
        }
    }
    catch (const std::exception &e)
    {
        strcpy(exc, e.what());
        __builtin_software_breakpoint();
    }
}

// Returns 1..n for the selector switch position
int LogicMeter::ReadSelectorSwitch()
{
#ifdef FORCE_TOOL
    return FORCE_TOOL + 1;
#else
    int selected = 0;
    
    // Scan the selector switch
    // When selector is at 10&1, regulator's enable is pulled
    // low and we turn off. At any other setting, EN is pulled
    // high.
    
    // When selector is at 1&2, pin 2 will be pulled to 0 even though it has a WPU.
    GPIO_PinWPUEnable(Selector2_PIN);
    CORETIMER_DelayUs(1);
    if (Selector2_Get() == 0)
    {
        selected = 1;
    }
    GPIO_PinWPUDisable(Selector2_PIN);

    // When selector is at 2&3, pin 2 will be pulled to 1 even though it has a WPD.
    GPIO_PinWPDEnable(Selector2_PIN);
    CORETIMER_DelayUs(1);
    if (Selector2_Get() == 1)
    {
        selected = 2;
    }
    GPIO_PinWPUDisable(Selector2_PIN);
    
    // When selector is at 3&4, pin 4 will be pulled to 1 even though it has a WPD.
    GPIO_PinWPDEnable(Selector4_PIN);
    CORETIMER_DelayUs(1);
    if (Selector4_Get() == 1)
    {
        selected = 3;
    }
    GPIO_PinWPDDisable(Selector4_PIN);

    // When selector is at 4&5, pin 4 will be pulled to 0 even though it has a WPU.
    GPIO_PinWPUEnable(Selector4_PIN);
    CORETIMER_DelayUs(1);
    if (Selector4_Get() == 0)
    {
        selected = 4;
    }
    GPIO_PinWPUDisable(Selector4_PIN);
    
    // When selector is at 5&6, pin 6 will be pulled to 0 even though it has a WPU.
    GPIO_PinWPUEnable(Selector6_PIN);
    CORETIMER_DelayUs(1);
    if (Selector6_Get() == 0)
    {
        selected = 5;
    }
    GPIO_PinWPUDisable(Selector6_PIN);

    // When selector is at 6&7, pin 6 will be pulled to 1 even though it has a WPD.
    GPIO_PinWPDEnable(Selector6_PIN);
    CORETIMER_DelayUs(1);
    if (Selector6_Get() == 1)
    {
        selected = 6;
    }
    GPIO_PinWPUDisable(Selector6_PIN);
    
    // When selector is at 7&8, pin 8 will be pulled to 1 even though it has a WPD.
    GPIO_PinWPDEnable(Selector8_PIN);
    CORETIMER_DelayUs(1);
    if (Selector8_Get() == 1)
    {
        selected = 7;
    }
    GPIO_PinWPDDisable(Selector8_PIN);

    // When selector is at 8&9, pin 8 will be pulled to pin 9
    Selector9_OutputEnable();
    Selector9_Set();
    CORETIMER_DelayUs(1);
    if (Selector8_Get() == 1)
    {
        Selector9_Clear();
        CORETIMER_DelayUs(1);
        if (Selector8_Get() == 0)
            selected = 8;
    }
    else
        Selector9_Clear();

    // When selector is at 9&10, pin 9 will be pulled to 1 
    Selector9_InputEnable();
    GPIO_PinWPDEnable(Selector9_PIN);
    CORETIMER_DelayUs(1);
    if (Selector9_Get() == 1)
    {
        selected = 9;
    }
    GPIO_PinWPDDisable(Selector9_PIN);
    
    return selected;
#endif
}

// Returns 0..n for the pressed button, or -1
int LogicMeter::ReadButtons()
{
    for (size_t i = 0; i < countof(buttons); ++i)
    {
        // If the button is pressed
        if (GPIO_PinRead(buttons[i]) == 0)
            return int(i);
    }
    return -1;
}

void LogicMeter::UserInteraction()
{
    _lastUserInteractionTime = SYS_TIME_CounterGet();
    if (_displayDimmed)
    {
        _displayDimmed = false;
        OCMP3_CompareSecondaryValueSet(TMR4_PeriodGet() * settings.screenBrightness / 100);
    }
}
