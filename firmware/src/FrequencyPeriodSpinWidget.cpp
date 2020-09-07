/* 
 * File:   FrequencyPeriodSpinWidget.cpp
 * Author: Bob
 * 
 * Created on June 24, 2020, 10:28 AM
 */

extern "C" 
{
#include "definitions.h"
}
#include "Utility.h"
#include "FrequencyPeriodSpinWidget.h"

std::string FrequencyPeriodSpinWidget::ToString(int cursorPos) const
{
    std::string s;
    if (cursorPos == -1)
    {
        if (_show.ShowFrequency)
        {
            s = eng(double(*_freq), 3, "Hz");
            if (_show.ShowPeriod)
                s += "/";
        }
        if (_show.ShowPeriod)
        {
            if (*_freq)
                s += eng(1.0 / double(*_freq), 3, "s");
            else
                s += "---";
        }
    }
    else
    {
        char buf[20];
        int rightmostDigit;
        if (_editMode == Frequency)
        {
            int len = sprintf(buf, "%03u,%03u,%03u.%02u Hz", 
                    int(*_freq / 1000000),
                    int(*_freq / 1000 % 1000),
                    int(*_freq % 1000),
                    int(*_freq % 1 * 100));
            rightmostDigit = len - 4;
            s = buf;
        }
        else
        {
            fixed period = 1 / *_freq;
            int len = sprintf(buf, "%02u.%03u %03u %03u s", 
                    int(period),
                    int(period * 1000 % 1000),
                    int(period * 1000000 % 1000),
                    int(period * 1000000000 % 1000));
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

void FrequencyPeriodSpinWidget::Increment(int cursorPos, bool increment)
{
    if (_editMode == Frequency)
    {
        static const fixed freqPow10[] = {0.01, 0.1, 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
        if (increment)
        {
            *_freq += freqPow10[cursorPos];
            if (*_freq > _max)
                *_freq = _max;
        }
        else
        {
            *_freq -= freqPow10[cursorPos];
            if (*_freq < 0)
                *_freq = 0;
        }
    }
    
    // Else (editing period)
    else
    {
        static const fixed perPow10[] = {0.000000001, 0.00000001, 0.0000001, 
            0.000001, 0.00001, 0.0001, 
            0.001, 0.01, 0.1, 
            1, 10};
        fixed period;
        if (*_freq == 0)
            period = 0;
        else
            period = 1 / *_freq;
        if (increment)
        {
            period += perPow10[cursorPos];
        }
        else
        {
            period -= perPow10[cursorPos];
        }
        if (period == 0)
            *_freq = 0;
        else
            *_freq = 1 / period;
        if (*_freq < 0)
            *_freq = 0;
        else if (*_freq > _max)
            *_freq = _max;
    }
}

