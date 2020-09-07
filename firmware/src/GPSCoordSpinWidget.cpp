/* 
 * File:   GPSCoordSpinWidget.cpp
 * Author: Bob
 * 
 * Created on May 25, 2020, 9:58 AM
 */

#include "Utility.h"
#include "GPSCoordSpinWidget.h"

std::string GPSCoordSpinWidget::ToString(int cursorPos) const
{
    char buf[100], *b = buf;
    int32_t posVal = *_coord;
    if (posVal < 0)
        posVal = -posVal;
    int32_t deg = posVal / 10000;
    int32_t frac = posVal % 10000;
    // For some reason, sprintf is crashing when we pass it a width field on %d.
    // So we manually do the field width here
    if (*_coord == 0)
        *b++ = ' ';
    else if (*_coord < 0)
        *b++ = '-';
    else
        *b++ = '+';
    b += LeadingChars(b, deg, _latLong == Latitude ? 10 : 100, (cursorPos < 0) ? ' ' : '0');
    b += sprintf(b, "%d.", deg);
    b += LeadingChars(b, frac, 1000, '0');
    sprintf(b, "%d" UTF8_DEGREES, frac);
    
    if (cursorPos != -1)
    {
        // cursorPos 6 is the leftmost digit (string position 1, because of the +/- sign).
        size_t charPos = ((_latLong == Latitude) ? 5 : 6) - cursorPos + 1;
        // The decimal point is before cursor position 4
        if (charPos > 3)
            ++charPos;
        std::string s = buf;
        s.insert(charPos + 1, NORMAL_TEXT);
        s.insert(charPos, INVERSE_TEXT);
        return s;
    }
    
    return buf;
}

void GPSCoordSpinWidget::Increment(int cursorPos, bool increment)
{
    static const int32_t pow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000};
    int32_t max = (_latLong == Latitude) ? 900000 : 1800000;
    int32_t newValue;
    
    if (increment)
    {
        newValue = *_coord + pow10[cursorPos];
        if (*_coord < 0 && newValue > 0)
            newValue = -*_coord;
        else if (newValue > max)
            newValue = -*_coord;
    }
    // Else (decrement)
    else
    {
        newValue = *_coord - pow10[cursorPos];
        if (*_coord > 0 && newValue < 0)
            newValue = -*_coord;
        else if (newValue < -max)
            newValue = -*_coord;
    }
    *_coord = newValue;
}

int GPSCoordSpinWidget::LeadingChars(char *b, int32_t val, int place, char leadingChar)
{
    int cnt = 0;
    while (val < place && place >= 10)
    {
        *b++ = leadingChar;
        place /= 10;
        ++cnt;
    }
    return cnt;
}