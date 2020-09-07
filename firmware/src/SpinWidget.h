/* 
 * File:   SpinWidget.h
 * Author: Bob
 *
 * Created on May 24, 2020, 1:18 PM
 */

#ifndef SPINWIDGET_H
#define	SPINWIDGET_H

#include <string>
extern "C" 
{
#include "definitions.h"
}
#include "RichLabelWidget.h"

class SpinWidget
{
public:
    SpinWidget(laDrawSurfaceWidget *ds, int fieldCount) :
        _labelWidget(ds), _fieldCount(fieldCount), _cursorPos(-1), _editing(false)
    {
    }
    ~SpinWidget() {}
    
    // cursorPos 0 is the rightmost position; cursorPos 1 is to the left of that, etc.
    void StartEditing(int cursorPos)
    {
        _cursorPos = cursorPos;
        _editing = true;
        UpdateText();
    }
    void StopEditing()
    {
        _editing = false;
        UpdateText();
    }
    void Up()
    {
        Increment(_cursorPos, true);
        UpdateText();
    }
    void Down()
    {
        Increment(_cursorPos, false);
        UpdateText();
    }
    void Left()
    {
        if (++_cursorPos < _fieldCount)
            UpdateText();
        else
            --_cursorPos;
    }
    void Right()
    {
        if (_cursorPos > 0)
        {
            --_cursorPos;
            UpdateText();
        }
    }
    void Done()
    {
        StopEditing();
        UpdateText();
    }
    
    virtual std::string ToString(int cursorPos) const = 0;
    virtual void Increment(int cursorPos, bool increment) = 0;
    
    void UpdateText() 
    {
        std::string s;
        s = ToString(_editing ? _cursorPos : -1);
        _labelWidget.SetText(s.c_str());
    }
    
private:
    SpinWidget(const SpinWidget& orig);
    
    RichLabelWidget _labelWidget;
    int _fieldCount, _cursorPos;
    bool _editing;
};


#endif	/* SPINWIDGET_H */

