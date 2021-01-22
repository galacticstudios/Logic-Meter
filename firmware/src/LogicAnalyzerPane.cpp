/* 
 * File:   LogicAnalyzerPane.cpp
 * Author: Bob
 * 
 * Created on November 8, 2020, 4:06 PM
 */

extern "C" 
{
#include "definitions.h"
}
#include "LogicAnalyzerPane.h"
#include "Display.h"

LogicAnalyzerPane::LogicAnalyzerPane() :
    _updateChannels(0),
    _ch1TraceWidget(Ch1Trace), _ch2TraceWidget(Ch2Trace), _ch3TraceWidget(Ch3Trace), 
    _tracePixelWidth(laWidget_GetWidth((laWidget *) Ch1Trace)),
    _traces{new TracePoint[_tracePixelWidth], new TracePoint[_tracePixelWidth], new TracePoint[_tracePixelWidth]},
    _ch1TracePainter(0xf800, _traces[0]), _ch2TracePainter(0xf800, _traces[1]), _ch3TracePainter(0xf800, _traces[2])
{       
    _ch1TraceWidget.Register(&_ch1TracePainter);
    _ch2TraceWidget.Register(&_ch2TracePainter);
    _ch3TraceWidget.Register(&_ch3TracePainter);
    for (int i = 0; i < countof(_traces); ++i)
        _traces[i][0] = TracePoint::Blank;
}

LogicAnalyzerPane::~LogicAnalyzerPane() 
{
    for (int i = 0; i < countof(_traces); ++i)
        delete [] _traces[i];
}

void LogicAnalyzerPane::Update()
{
    _ch1TraceWidget.Invalidate();
    _ch2TraceWidget.Invalidate();
    _ch3TraceWidget.Invalidate();
}

void LogicAnalyzerPane::SetTrigger(uint8_t triggerChannel, TriggerEdge triggerEdge)
{
    if (triggerChannel == 0)
    {
        laWidget_SetVisible((laWidget *) TriggerLabel, LA_FALSE);
    }
    else
    {
        int32_t x = laWidget_GetX((laWidget *) TriggerLabel);
        int32_t y = (triggerChannel - 1) * (laWidget_GetY(_ch2TraceWidget.GetSurface()) - laWidget_GetY(_ch1TraceWidget.GetSurface()));
        laWidget_SetPosition((laWidget *) TriggerLabel, x, y);
        char text[20];
        sprintf(text, "Trig: %s", triggerEdge == TriggerEdge::Rising ? "rising" :
            triggerEdge == TriggerEdge::Falling ? "falling" : "either");
        laString s = laString_CreateFromCharBuffer(text, &MonoFont);
        laLabelWidget_SetText(TriggerLabel, s);
        laString_Destroy(&s);
        laWidget_SetVisible((laWidget *) TriggerLabel, LA_TRUE);
    }
}

laWidget *LogicAnalyzerPane::GetWidget() const
{
    return LogicAnalyzerPanel;
}
