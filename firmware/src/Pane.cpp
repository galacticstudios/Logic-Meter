/* 
 * File:   Pane.cpp
 * Author: Bob
 * 
 * Created on May 1, 2020, 9:58 AM
 */

#include <vector>
extern "C" 
{
#include "definitions.h"
}
#include "Utility.h"
#include "Pane.h"

void Pane::Show(bool show)
{
    laWidget_SetVisible(GetWidget(), show ? LA_TRUE : LA_FALSE);
}

Rect Pane::GetPosition() const
{
    Rect rect;
    rect.x = laWidget_GetX(GetWidget());
    rect.y = laWidget_GetY(GetWidget());
    rect.width = laWidget_GetWidth(GetWidget());
    rect.height = laWidget_GetHeight(GetWidget());
    return rect;
}

void Pane::SetPosition(const Rect &rect)
{
    laWidget_SetPosition(GetWidget(), rect.x, rect.y);
    laWidget_SetSize(GetWidget(), rect.width, rect.height);
}

void Pane::SetVisible(bool visible)
{
    laWidget_SetVisible(GetWidget(), visible ? LA_TRUE : LA_FALSE);
}
