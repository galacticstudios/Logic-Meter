/* 
 * File:   DutyCycleSpinWidget.h
 * Author: Bob
 *
 * Created on June 27, 2020, 1:42 PM
 */

#ifndef DUTYCYCLESPINWIDGET_H
#define	DUTYCYCLESPINWIDGET_H

#include "SpinWidget.h"
#include "fixed.h"

class DutyCycleSpinWidget : public SpinWidget
{
public:
    struct Show
    {
        int ShowDutyCycle : 1;
        int ShowPulseWidth : 1;
    };
    // duty is actually the ratio, [0..1]
    DutyCycleSpinWidget(laDrawSurfaceWidget *ds, const fixed &freq, fixed *duty, Show show) : 
        SpinWidget(ds, 11), _duty(duty), _freq(freq), _show(show), _editMode(Percentage)
    {
        UpdateText();
    }
    virtual ~DutyCycleSpinWidget() {}

    std::string ToString(int cursorPos) const;
    void Increment(int cursorPos, bool increment);
    
    enum EditMode {Percentage, PulseWidth};
    void SetEditMode(EditMode em) {_editMode = em;}
    
private:
    fixed *_duty;
    fixed _freq;
    EditMode _editMode;
    Show _show;
    
    DutyCycleSpinWidget(const DutyCycleSpinWidget& orig);
};

#endif	/* DUTYCYCLESPINWIDGET_H */

