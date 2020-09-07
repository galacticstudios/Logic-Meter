/* 
 * File:   Pane.h
 * Author: Bob
 *
 * Created on May 1, 2020, 9:58 AM
 */

#ifndef PANE_H
#define	PANE_H

#include "Utility.h"
#include "SpinWidget.h"

struct laWidget_t;
typedef laWidget_t laWidget;

class Pane
{
public:
    void Show(bool show);
    
    virtual void Update() {}
    
    void Up() {ASSERT(_editingWidget); _editingWidget->Up(); Update();}
    void Down() {ASSERT(_editingWidget); _editingWidget->Down(); Update();}
    void Left() {ASSERT(_editingWidget); _editingWidget->Left();}
    void Right() {ASSERT(_editingWidget); _editingWidget->Right();}
    void Done() {ASSERT(_editingWidget); _editingWidget->Done(); _editingWidget = NULL;}
    
    Rect GetPosition() const;
    void SetPosition(const Rect &rect);
    
    void SetVisible(bool visible);

protected:
    Pane() : _editingWidget(NULL) {}
    
    void SetEditingWidget(SpinWidget *widget) {_editingWidget = widget;}
    virtual laWidget *GetWidget() const = 0;

private:
    SpinWidget *_editingWidget;
};

#endif	/* PANE_H */

