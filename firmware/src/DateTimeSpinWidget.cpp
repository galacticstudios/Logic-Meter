/* 
 * File:   DateTimeSpinWidget.cpp
 * Author: Bob
 * 
 * Created on May 25, 2020, 11:01 AM
 */

#include "Utility.h"
#include "DateTimeSpinWidget.h"

std::string DateTimeSpinWidget::ToString(int cursorPos) const
{
    char buf[100];
    struct tm *t = gmtime(_time);
    strftime(buf, countof(buf), "%Y/%m/%d %H:%M", t);
    if (cursorPos != -1)
    {
        static const size_t fieldBoundaries[][2] = {{14, 16}, {11, 13}, {8, 10}, {5, 7}, {0, 4}};
        std::string s = buf;
        s.insert(fieldBoundaries[cursorPos][1], NORMAL_TEXT);
        s.insert(fieldBoundaries[cursorPos][0], INVERSE_TEXT);
        return s;
    }
    return buf;
}

void DateTimeSpinWidget::Increment(int cursorPos, bool increment)
{
    static const int32_t inc[] = {offsetof(struct tm, tm_min), offsetof(struct tm, tm_hour),
        offsetof(struct tm, tm_mday), offsetof(struct tm, tm_mon), offsetof(struct tm, tm_year)};
    struct tm t = *localtime(_time);
    int *field;
    switch (cursorPos)
    {
        case 4 :
            field = &t.tm_year;
            break;
        case 3 :
            field = &t.tm_mon;
            break;
        case 2 :
            field = &t.tm_mday;
            break;
        case 1 :
            field = &t.tm_hour;
            break;
        case 0 :
            field = &t.tm_min;
            break;
        default :
            __builtin_software_breakpoint();
            break;
    }
    if (increment)
        ++*field;
    else
        --*field;
    time_t newTime = mktime(&t);
    *_time = newTime;
}
