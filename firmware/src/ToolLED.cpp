/* 
 * File:   ToolLED.cpp
 * Author: Bob
 * 
 * Created on June 2, 2020, 5:50 PM
 */

#include <stdlib.h>
#include <algorithm>
extern "C"
{
#include "definitions.h"
}
#include "Utility.h"
#include "Menu.h"
#include "ToolLED.h"

static const Help help("LED Anode", NULL, NULL, 
        "Measures the forward voltage and current of an LED and allows you to see the "
        "brightness at different currents.");

static const MenuItem menuItems[5] = {
    MenuItem("I" UTF8_UPARROW, MenuType::NoChange, NULL, CB(&ToolLED::IncreaseCurrent)), 
    MenuItem("I" UTF8_DOWNARROW, MenuType::NoChange, NULL, CB(&ToolLED::DecreaseCurrent)), 
    MenuItem(),
    MenuItem(),
    MenuItem()};

const Menu menu(menuItems);
   
static const double Vcc = 3.3;

ToolLED::ToolLED() :
    Tool("LED Test", new LEDTestPane(vRef, 30000), menu, help), _state(WaitingForLED),
    _cvref(true)
{
    ADCHS_ModulesEnable(ADCHS_MODULE_MASK(ADCHS_MODULE2_MASK | ADCHS_MODULE4_MASK));
}

ToolLED::~ToolLED() 
{
    ADCHS_ModulesDisable(ADCHS_MODULE_MASK(ADCHS_MODULE2_MASK | ADCHS_MODULE4_MASK));
}

ToolLED::Sample ToolLED::ReadADCs(uint8_t opAmpValue, uint8_t cvrRange, uint8_t cvrValue)
{
    Sample sample = {opAmpValue, cvrRange, cvrValue};
    do
    {
        // Read the op amp's output
        ADCHS_ChannelConversionStart(ADCHS_CH13);
        while (!ADCHS_ChannelResultIsReady(ADCHS_CH13)) {}
        sample.opAmpReading = ADCHS_ChannelResultGet(ADCHS_CH13);
        // Read the LED anode's level
        ADCHS_ChannelConversionStart(ADCHS_CH15);
        while (!ADCHS_ChannelResultIsReady(ADCHS_CH15)) {}
        sample.ledAnodeReading = ADCHS_ChannelResultGet(ADCHS_CH15);
    }
    while (sample.opAmpReading < sample.ledAnodeReading);
    return sample;
}

LEDTestPane::XY ToolLED::TomVfuA(const Sample &sample)
{
    LEDTestPane::XY mVfuA;
    mVfuA.X = sample.ledAnodeReading * vRef / (maxADCReading + 1);
    mVfuA.Y = (sample.opAmpReading - sample.ledAnodeReading) * 1000 *
        vRef / (maxADCReading + 1) / resistorValue;
    return mVfuA;
}

void ToolLED::OnIdle()
{
    switch (_state)
    {
        case WaitingForLED :
            _state = TestingLED;
            break;
            
        case TestingLED :
            {
                _cvref.Enable();
                
                _samples.clear();
                
                // Go through all the op amp gains
                for (auto opAmpValue = _opAmp.LowValue(); opAmpValue < _opAmp.HighValue(); ++opAmpValue)
                {
                    _opAmp.Set(opAmpValue);
                    
                    // Go through all the CVR ranges
                    for (int cvrRange = 0; cvrRange < 2; ++cvrRange)
                    {
                        // Go through all the CVR values
                        for (int cvrValue = 0; cvrValue < 16; ++cvrValue)
                        {
                            _cvref.Set(cvrRange, cvrValue);
                            CORETIMER_DelayMs(1);
                            _samples.push_back(ReadADCs(opAmpValue, cvrRange, cvrValue));
                        }

                    }
                }

                // Sort the values
                std::sort(_samples.begin(), _samples.end());
                // Remove duplicate results
                auto newEnd = std::unique(_samples.begin(), _samples.end());
                _samples.resize(std::distance(_samples.begin(), newEnd));
                
                // Set the voltage output to 0
                _currentSampleIndex = 0;
                _opAmp.Set(_samples[_currentSampleIndex].opAmpValue);
                _cvref.Set(_samples[_currentSampleIndex].cvrRange, _samples[_currentSampleIndex].cvrValue);

                // Go through the samples and calculate the forward voltage and current
                _mVfuA.clear();
                _mVfuA.reserve(_samples.size());
                for (auto &sample : _samples)
                {
                    // Calculate microAmps and milli Vf
                    _mVfuA.push_back(TomVfuA(sample));
                }
                std::vector<LEDTestPane::XY> graphData = _mVfuA;
                std::sort(graphData.begin(), graphData.end(), LEDTestPane::XY::XLessThan);
                GetPane()->SetData(graphData);
                GetPane()->SetCurrentLevel(graphData[0]);

                _state = ModifyingCurrent;
            }
            break;
            
        case ModifyingCurrent :
            break;
    }
}

void ToolLED::IncreaseCurrent()
{
    // Find the sample 1000uA higher than the current setting
    size_t newSampleIndex;
    for (newSampleIndex = _currentSampleIndex; newSampleIndex < _samples.size() - 1; ++newSampleIndex)
    {
        if (_mVfuA[_currentSampleIndex].Y + 1000 <= _mVfuA[newSampleIndex].Y)
            break;
    }
    _currentSampleIndex = newSampleIndex;
    _opAmp.Set(0);
    while (_opAmp.IsBusy()) {}
    _cvref.Set(_samples[_currentSampleIndex].cvrRange, _samples[_currentSampleIndex].cvrValue);
    _opAmp.Set(_samples[_currentSampleIndex].opAmpValue);
    
    CORETIMER_DelayMs(1);
    LEDTestPane::XY iv = TomVfuA(
            ReadADCs(_samples[_currentSampleIndex].opAmpValue, _samples[_currentSampleIndex].cvrRange, _samples[_currentSampleIndex].cvrValue));
    
    GetPane()->SetCurrentLevel(iv);
}
        
void ToolLED::DecreaseCurrent()
{
    // Find the sample 1000uA lower than the current setting
    size_t newSampleIndex;
    for (newSampleIndex = _currentSampleIndex; newSampleIndex > 0; --newSampleIndex)
    {
        if (_mVfuA[_currentSampleIndex].Y - 1000 >= _mVfuA[newSampleIndex].Y)
            break;
    }
    _currentSampleIndex = newSampleIndex;
    _opAmp.Set(0);
    while (_opAmp.IsBusy()) {}
    _cvref.Set(_samples[_currentSampleIndex].cvrRange, _samples[_currentSampleIndex].cvrValue);
    _opAmp.Set(_samples[_currentSampleIndex].opAmpValue);
    
    CORETIMER_DelayMs(1);
    LEDTestPane::XY iv = TomVfuA(
            ReadADCs(_samples[_currentSampleIndex].opAmpValue, _samples[_currentSampleIndex].cvrRange, _samples[_currentSampleIndex].cvrValue));
    
    GetPane()->SetCurrentLevel(iv);
}

void ToolLED::Update()
{
}
