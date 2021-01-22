/* 
 * File:   LogicAnalyzerPane.h
 * Author: Bob
 *
 * Created on November 8, 2020, 4:06 PM
 */

#ifndef LOGICANALYZERPANE_H
#define	LOGICANALYZERPANE_H

#include "Pane.h"
#include "TracePainter.h"
#include "Settings.h"

#define LA_CHANNEL_COUNT 3

class LogicAnalyzerPane : public Pane
{
public:
    LogicAnalyzerPane();
    virtual ~LogicAnalyzerPane();
    
    virtual void Update();
    
    // Update a channel pixel
    void SetTracePoint(uint32_t channel, uint32_t pixelIndex, TracePoint point) 
    {
        _traces[channel][pixelIndex] = point;
    }
    
    uint32_t TracePixelWidth() const {return _tracePixelWidth;}
    
    void SetTrigger(uint8_t triggerChannel, TriggerEdge triggerEdge);

private:
    LogicAnalyzerPane(const LogicAnalyzerPane& orig);
    
    virtual laWidget *GetWidget() const;
    
    uint32_t _tracePixelWidth;
    
    // Must be initialized after _tracePanelWidth and before _ch<n>TracePainter
    TracePoint *_traces[LA_CHANNEL_COUNT];
    
    TracePainter _ch1TracePainter;
    SurfaceWrapper _ch1TraceWidget;
    TracePainter _ch2TracePainter;
    SurfaceWrapper _ch2TraceWidget;
    TracePainter _ch3TracePainter;
    SurfaceWrapper _ch3TraceWidget;
    
    int _updateChannels;
};

#endif	/* LOGICANALYZERPANE_H */

