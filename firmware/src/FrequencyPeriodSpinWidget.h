/* 
 * File:   FrequencyPeriodSpinWidget.h
 * Author: Bob
 *
 * Created on June 24, 2020, 10:28 AM
 */

#ifndef FREQUENCYPERIODSPINWIDGET_H
#define	FREQUENCYPERIODSPINWIDGET_H

#include "SpinWidget.h"
#include "Fixed.h"

class FrequencyPeriodSpinWidget : public SpinWidget
{
public:
    struct Show
    {
        int ShowFrequency : 1;
        int ShowPeriod : 1;
    };
    
    FrequencyPeriodSpinWidget(laDrawSurfaceWidget *ds, fixed *freq, fixed max, Show show) : 
        SpinWidget(ds, 11), _freq(freq), _max(max), _show(show), _editMode(Frequency)
    {
        UpdateText();
    }
    virtual ~FrequencyPeriodSpinWidget() {}

    std::string ToString(int cursorPos) const;
    void Increment(int cursorPos, bool increment);
    
    enum EditMode {Frequency, Period};
    void SetEditMode(EditMode em) {_editMode = em;}
    
private:
    fixed *_freq;
    fixed _max;
    EditMode _editMode;
    Show _show;
    
    FrequencyPeriodSpinWidget(const FrequencyPeriodSpinWidget& orig);
};

#endif	/* FREQUENCYPERIODSPINWIDGET_H */

