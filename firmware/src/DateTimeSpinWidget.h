/* 
 * File:   DateTimeSpinWidget.h
 * Author: Bob
 *
 * Created on May 25, 2020, 11:01 AM
 */

#ifndef DATETIMESPINWIDGET_H
#define	DATETIMESPINWIDGET_H

#include <time.h>
#include "SpinWidget.h"

class DateTimeSpinWidget : public SpinWidget
{
public:
    DateTimeSpinWidget(laDrawSurfaceWidget *ds, time_t *t) : SpinWidget(ds, 5), _time(t) 
    {
        UpdateText();
    }
    virtual ~DateTimeSpinWidget() {}

    std::string ToString(int cursorPos) const;
    void Increment(int cursorPos, bool increment);
    
private:
    time_t *_time;
    
    DateTimeSpinWidget(const DateTimeSpinWidget& orig);

};

#endif	/* DATETIMESPINWIDGET_H */

