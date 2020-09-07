/* 
 * File:   SpinnerControl.h
 * Author: Bob
 *
 * If a Pane has spinner(s) on it, multiply inherit from PaneSpinnerControl
 * and you'll get Up, Down, Left, Right, and Done. If a Tool has a pane with
 * spinners on it, multiply inherit the tool from ToolSpinnerControl and you'll
 * get Up, Down, Left, and Right that delegate to the PaneSpinnerControl.
 */

#ifndef SPINNERCONTROL_H
#define	SPINNERCONTROL_H

#include "Pane.h"
#include "Tool.h"
#include "SurfaceWrapper.h"
#include "RichTextPainter.h"
#include "MultiSpinnerPainter.h"
#include "Display.h"


class PaneWithSpinners;

class _ToolWithSpinners : public Tool
{
public:
    using Tool::Tool;
    
protected:
    friend class PaneWithSpinners;
    virtual void OnFieldChanged(fixed newValue) = 0;
};

class PaneWithSpinners : public Pane
{
public:
    PaneWithSpinners() :
        _spinWrapper(NULL), _unregisteredPainter(NULL), _currentField(NULL),
        _callbackObj(NULL)
    {
    }

    class Field
    {
    public:
        Field(SurfaceWrapper &surface, RichTextPainter &painter,
              std::string (*defaultFormatter)(fixed value)) :
            _surface(surface), _painter(painter),
            _defaultFormatter(defaultFormatter)
        {
            _surface.Register(&painter);
            _painter.RichTextBlocks().push_back({"", &MonoFont, 0, 0xffff});
        }
        void Update(fixed value) const
        {
            _painter.RichTextBlocks()[0].text = (*_defaultFormatter)(value);
            _surface.Invalidate();
        }
        std::vector<SpinField> SpinFields;
            
    private:
        friend class PaneWithSpinners;
        SurfaceWrapper &_surface;
        RichTextPainter &_painter;
        std::string (*_defaultFormatter)(fixed value);
    };
    
    // Call this from ToolSpinnerControl::EnterSpinner
    void EnterSpinner(const Field &field, fixed initialValue, _ToolWithSpinners *callbackObj)
    {
        _unregisteredPainter = &field._painter;
        _spinWrapper = &field._surface;
        _spinWrapper->Unregister(_unregisteredPainter);
        _spinWrapper->Register(&_spinPainter);
        _spinPainter.SpinFields() = field.SpinFields;
        _currentField = &_spinPainter.SpinFields().back();
        _currentField->SetValue(initialValue);
        _callbackObj = callbackObj;
    }

    void Up()
    {
        _spinPainter.Up();
        _callbackObj->OnFieldChanged(_currentField->CurrentValue());
    }

    void Down()
    {
        _spinPainter.Down();
        _callbackObj->OnFieldChanged(_currentField->CurrentValue());
    }

    void Left()
    {
        _spinPainter.Left();
    }

    void Right()
    {
        _spinPainter.Right();
    }

    void Done()
    {
        _spinWrapper->Unregister(&_spinPainter);
        _spinWrapper->Register(_unregisteredPainter);
        _spinWrapper->Invalidate();
    }
    
private:       
    // Keep track of where the spinner is and what normally belongs there
    SurfaceWrapper *_spinWrapper;
    SurfacePainter *_unregisteredPainter;
    
    MultiSpinnerPainter _spinPainter;
    
    SpinField *_currentField;

    _ToolWithSpinners *_callbackObj;
};

template <class DerivedClass>
class ToolWithSpinners : public _ToolWithSpinners
{
public:
    void Up()
    {
        _pane->Up();
        Update();
    }

    void Down()
    {
        _pane->Down();
        Update();
    }

    void Left()
    {
        _pane->Left();
    }

    void Right()
    {
        _pane->Right();
    }

    void Done()
    {
        _pane->Done();
    }
    
    virtual void Update() = 0;
    
protected:
    using _ToolWithSpinners::_ToolWithSpinners;
    virtual ~ToolWithSpinners() {}
    
    void EnterSpinner(PaneWithSpinners *pane,
        const PaneWithSpinners::Field &field,
        fixed initialValue,
        void (DerivedClass::*currentFieldSetter)(fixed newValue))
    {
        _pane = pane;
        _currentFieldSetter = currentFieldSetter;
        _pane->EnterSpinner(field, initialValue, this);
    }
    
    void OnFieldChanged(fixed newValue)
    {
        (dynamic_cast<DerivedClass *>(this)->*_currentFieldSetter)(newValue);
    }

    PaneWithSpinners *_pane = NULL;
    void (DerivedClass::*_currentFieldSetter)(fixed newValue);
};

#endif	/* SPINNERCONTROL_H */

