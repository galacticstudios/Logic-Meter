/* 
 * File:   DutyCycleSpinWidget.cpp
 * Author: Bob
 * 
 * Created on June 27, 2020, 1:42 PM
 */

extern "C" 
{
#include "definitions.h"
}
#include "Utility.h"
#include "DutyCycleSpinWidget.h"

std::string DutyCycleSpinWidget::ToString(int cursorPos) const
{
    std::string s;
    char buf[20] = {0};
    if (cursorPos == -1)
    {
        if (_show.ShowDutyCycle)
        {
            int len = sprintf(buf, "%3d.%02d%%", int(*_duty * 100), int(*_duty * 10000) % 100);
            if (_show.ShowDutyCycle)
                strcpy(buf + len, "/");
        }
        s = buf;
        if (_show.ShowPulseWidth)
            s += eng(double(*_duty / _freq), 3, "s");
    }
    else
    {
        int rightmostDigit;
        if (_editMode == Percentage)
        {
            int len = sprintf(buf, "%3d.%02d%%", int(*_duty * 100), int(*_duty * 10000) % 100);
            rightmostDigit = len - 2;
            s = buf;
        }
        else
        {
            fixed pulseWidth;
            if (_freq)
                pulseWidth = *_duty / _freq;
            else
                pulseWidth = 0;
            int len = sprintf(buf, "%02u.%03u %03u %03u s", 
                    int(pulseWidth),
                    int(pulseWidth * 1000 % 1000),
                    int(pulseWidth * 1000000 % 1000),
                    int(pulseWidth * 1000000000 % 1000));
            rightmostDigit = len - 3;
            s = buf;
        }
        int strPos = rightmostDigit;
        while (cursorPos)
        {
            --strPos;
            if (s[strPos] != ',' && s[strPos] != '.' && s[strPos] != ' ')
                --cursorPos;
        }
        s.insert(strPos + 1, NORMAL_TEXT);
        s.insert(strPos, INVERSE_TEXT);
    }
    return s;
}

void DutyCycleSpinWidget::Increment(int cursorPos, bool increment)
{
    if (_editMode == Percentage)
    {
        static const fixed percentagePow10[] = {0.0001, 0.001, 0.01, 0.1, 1};
        if (increment)
        {
            *_duty += percentagePow10[cursorPos];
            if (*_duty > 1)
                *_duty = 1;
        }
        else
        {
            *_duty -= percentagePow10[cursorPos];
            if (*_duty < 0)
                *_duty = 0;
        }
    }
    
    // Else (editing pulse width)
    else
    {
        static const fixed perPow10[] = {0.000000001, 0.00000001, 0.0000001, 
            0.000001, 0.00001, 0.0001, 
            0.001, 0.01, 0.1, 
            1, 10};
        fixed pulseWidth;
        if (_freq)
            pulseWidth = *_duty / _freq;
        else
            pulseWidth = 0;
        if (increment)
        {
            pulseWidth += perPow10[cursorPos];
        }
        else
        {
            pulseWidth -= perPow10[cursorPos];
        }
        if (_freq == 0)
            *_duty = 0;
        else
            *_duty = pulseWidth * _freq;
        if (*_duty < 0)
            *_duty = 0;
        else if (*_duty > 1)
            *_duty = 1;
    }
}

