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
#include "Settings.h"
#include "Tool.h"
#include "ToolGPS.h"
#include "ToolLED.h"
#include "ToolPWM.h"
#include "ToolServo.h"
#include "ToolSPI.h"
#include "ToolUART.h"


static const GPIO_PIN buttons[] = {Button1_PIN, Button2_PIN, Button3_PIN, 
    Button4_PIN, Button5_PIN, Button6_PIN};

static LogicMeter *lm;

typedef Tool *(*ToolFactory)();

ToolFactory toolFactories[] = {
    ToolUART::Factory,
    ToolSPI::Factory,
    ToolGPS::Factory,
    ToolServo::Factory,
    ToolPWM::Factory,
    ToolLED::Factory,
};

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
    _displayDimmed(false)
{
    display.Initialize();
    SettingsInitialize();
    
    // Turn on the PWM for the LCD backlight
    TMR4_Start();
    OCMP2_Enable();
}

LogicMeter::~LogicMeter() 
{
}

char exc[100];

void LogicMeter::Task()
{
    static int lastButtonPressed = -1;
    static uint32_t lastButtonPressedTime;
    static bool buttonFunctionCalled = false;
    
    try
    {
        int newSelection = ReadSelectorSwitch() - 1;
        
        if (newSelection >= 0)
        {
            if (_currentSelection != newSelection)
            {
                if (_currentTool)
                    _currentTool->RequestDeactivate();
                delete _currentTool;
                _currentTool = (*toolFactories[newSelection % countof(toolFactories)])();
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
                        buttonFunctionCalled = true;
                        _currentTool->OnButtonPressed(pressedButton);
                    }
                    UserInteraction();
                }
            }
            else
                buttonFunctionCalled = false;

            _currentTool->OnIdle();
        }
        
        SettingsOnIdle();
        
        if (!_displayDimmed && 
            SYS_TIME_CountToMS(SYS_TIME_CounterGet() - _lastUserInteractionTime) > settings.screenDimMinutes * 60 * 1000)
        {
            _displayDimmed = true;
            OCMP2_CompareSecondaryValueSet(TMR4_PeriodGet() * 99 / 100);
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
    
    // At 2&3 through 8&9, an output on one member of
    // the pair will show up on the other.
    static const GPIO_PIN pins[] = {
        Selector2_PIN, Selector3_PIN, Selector4_PIN, Selector5_PIN, Selector6_PIN,
        Selector7_PIN, Selector8_PIN, Selector9_PIN};
    for (int i = 0; i < countof(pins) - 1 && selected == 0; ++i)
    {
        GPIO_PinOutputEnable(pins[i]);
        GPIO_PinSet(pins[i]);
        CORETIMER_DelayUs(1);
        if (GPIO_PinRead(pins[i + 1]))
        {
            selected = i + 2;
        }
        GPIO_PinClear(pins[i]);
        GPIO_PinInputEnable(pins[i]);
    }
    // At 9&10, pin 9 will be high even with its WPD.
    GPIO_PinWPDEnable(Selector9_PIN);
    CORETIMER_DelayUs(1);
    if (GPIO_PinRead(Selector9_PIN))
    {
        selected = countof(pins) + 1;
    }
    GPIO_PinWPDDisable(Selector9_PIN);
    
    return selected;
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
        OCMP2_CompareSecondaryValueSet(TMR4_PeriodGet() * 5 / 100);
    }
}
