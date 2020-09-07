/* 
 * File:   SquareWavePane.h
 * Author: Bob
 *
 * Created on May 1, 2020, 10:02 AM
 */

#ifndef SQUAREWAVEPANE_H
#define	SQUAREWAVEPANE_H

#include <list>
#include "fixed.h"
#include "Pane.h"
#include "RichTextPainter.h"
#include "SurfaceWrapper.h"
#include "LinesPainter.h"
#include "FrequencyPeriodSpinWidget.h"
#include "DutyCycleSpinWidget.h"

class SquareWavePane : public Pane
{
public:
    struct Show 
    {
        int ShowFrequency : 1;
        int ShowPeriod : 1;
        int ShowDutyCycle : 1;
        int ShowPulseWidth : 1;
    };
    
    SquareWavePane(fixed hertz, fixed duty, Show show);
    ~SquareWavePane();
    
    virtual void Update();
    
    void EnterSetFrequency();
    void EnterSetPeriod();
    void EnterSetDutyCycle();
    void EnterSetPulseWidth();
    
    fixed Hertz() const {return _hertz;}
    fixed DutyCycle() const {return _ratio * 100;}
    
protected:
    virtual laWidget *GetWidget() const;

private:
    fixed _hertz, _ratio;
    SurfaceWrapper _waveformWidget, _cycleCaliperWidget, _pulseCaliperWidget;
    LinesPainter _waveformPainter, _cycleCaliperPainter, _pulseCaliperPainter;
    
    FrequencyPeriodSpinWidget _freqWidget;
    DutyCycleSpinWidget _dutyWidget;
    
    void FrequencySetter(fixed newValue);
    void PeriodSetter(fixed newValue);
    void DutyCycleSetter(fixed newValue);

};

#endif	/* SQUAREWAVEPANE_H */

