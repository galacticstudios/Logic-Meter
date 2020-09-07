/* 
 * File:   SquareWavePane.cpp
 * Author: Bob
 * 
 * Created on May 1, 2020, 10:02 AM
 */

extern "C" 
{
#include "definitions.h"
}
#include "Display.h"
#include "Utility.h"
#include "SquareWavePane.h"
#include "SurfaceWrapper.h"

SquareWavePane::SquareWavePane(fixed hertz, fixed duty, Show show) :
    _hertz(hertz), _ratio(duty / 100),
    _waveformWidget(SquareWave), _cycleCaliperWidget(CycleCaliper), _pulseCaliperWidget(PulseCaliper),
    _freqWidget(SquareWaveFrequency, &_hertz, TMR2_FrequencyGet(), {show.ShowFrequency, show.ShowPeriod}), 
    _dutyWidget(SquareWavePulse, _hertz, &_ratio, {show.ShowDutyCycle, show.ShowPulseWidth})
{
    _waveformWidget.Register(&_waveformPainter);
    _cycleCaliperWidget.Register(&_cycleCaliperPainter);
    _pulseCaliperWidget.Register(&_pulseCaliperPainter);
}

SquareWavePane::~SquareWavePane()
{
}

laWidget *SquareWavePane::GetWidget() const
{
    return SquareWavePanel;
}

void SquareWavePane::Update()
{
    // Get the dimensions of the waveform to draw
    int32_t waveWidth = laWidget_GetWidth(_waveformWidget.GetSurface());
    int32_t waveHeight = laWidget_GetHeight(_waveformWidget.GetSurface());
    int32_t leaderWidth = waveWidth / 20;
    
    // Build the lines for the square wave
    auto &points = _waveformPainter.Points();
    points.clear();
    
    // Handle the degenerate cases of always low or always high
    if (_ratio == 0)
    {
        points.push_back({0, waveHeight - 1});
        points.push_back({waveWidth - 1, waveHeight - 1});
    }
    else if (_ratio == 1)
    {
        points.push_back({0, 1});
        points.push_back({waveWidth - 1, 1});
    }
    
    // Else (there's a non-zero pulse width)
    else
    {
        // Draw the leader
        points.push_back({0, waveHeight - 1});
        points.push_back({leaderWidth, waveHeight - 1});
        // Rising edge
        points.push_back({leaderWidth, 1});
        // High pulse
        int32_t pulseWidth = (int32_t) ((waveWidth - leaderWidth * 2) * _ratio);
        points.push_back({pulseWidth + leaderWidth, 1});
        // Falling edge
        points.push_back({pulseWidth + leaderWidth, waveHeight - 1});
        // Low level
        points.push_back({waveWidth - leaderWidth, waveHeight - 1});
        // Second rising edge
        points.push_back({waveWidth - leaderWidth, 1});
        // Trailer
        points.push_back({waveWidth, 1});
    }
    _waveformWidget.Invalidate();
    
    // Draw the caliper lines. First, the cycle caliper
    int32_t caliperHeight = laWidget_GetHeight(_cycleCaliperWidget.GetSurface());
    auto &ccpoints = _cycleCaliperPainter.Points();
    ccpoints.clear();
    ccpoints.push_back({leaderWidth, 0});
    ccpoints.push_back({leaderWidth, caliperHeight});
    ccpoints.push_back({leaderWidth, caliperHeight / 2});
    ccpoints.push_back({waveWidth - leaderWidth, caliperHeight / 2});
    ccpoints.push_back({waveWidth - leaderWidth, 0});
    ccpoints.push_back({waveWidth - leaderWidth, caliperHeight});
    
    // Next, the pulse caliper
    auto &pcpoints = _pulseCaliperPainter.Points();
    int32_t pulseWidth = int32_t((waveWidth - leaderWidth * 2) * _ratio);
    pcpoints.clear();
    pcpoints.push_back({leaderWidth, 0});
    pcpoints.push_back({leaderWidth, caliperHeight});
    pcpoints.push_back({leaderWidth, caliperHeight / 2});
    pcpoints.push_back({leaderWidth + pulseWidth, caliperHeight / 2});
    pcpoints.push_back({leaderWidth + pulseWidth, 0});
    pcpoints.push_back({leaderWidth + pulseWidth, caliperHeight});
    _pulseCaliperWidget.Invalidate();
}

void SquareWavePane::EnterSetFrequency()
{
    _freqWidget.SetEditMode(FrequencyPeriodSpinWidget::Frequency);
    SetEditingWidget(&_freqWidget);
    if (_hertz)
        _freqWidget.StartEditing((int) floor(log10(double(_hertz))) + 2);
    else
        _freqWidget.StartEditing(5);
}

void SquareWavePane::EnterSetPeriod()
{
    _freqWidget.SetEditMode(FrequencyPeriodSpinWidget::Period);
    SetEditingWidget(&_freqWidget);
    if (_hertz)
        _freqWidget.StartEditing(9 - (int) floor(log10(double(_hertz))));
    else
        _freqWidget.StartEditing(7);
}

void SquareWavePane::EnterSetDutyCycle()
{
    _dutyWidget.SetEditMode(DutyCycleSpinWidget::Percentage);
    SetEditingWidget(&_dutyWidget);
    _dutyWidget.StartEditing(2);
}

void SquareWavePane::EnterSetPulseWidth()
{
    _dutyWidget.SetEditMode(DutyCycleSpinWidget::PulseWidth);
    SetEditingWidget(&_dutyWidget);
    if (_hertz)
        _dutyWidget.StartEditing(9 + (int) floor(log10(double(_ratio / _hertz))));
    else
        _dutyWidget.StartEditing(9);
}

void SquareWavePane::FrequencySetter(fixed newValue)
{
    _hertz = newValue;
}

void SquareWavePane::PeriodSetter(fixed newValue)
{
    _hertz = 1000000000 / newValue;
}

void SquareWavePane::DutyCycleSetter(fixed newValue)
{
    _ratio = newValue / 100;
}

