/* 
 * File:   LEDTestPane.cpp
 * Author: Bob
 * 
 * Created on June 2, 2020, 5:54 PM
 */

extern "C"
{
#include "definitions.h"
}
#include "Display.h"
#include "LEDTestPane.h"

laString *LEDTestPane::_cursor;
GFX_Rect LEDTestPane::_cursorRect;
LEDTestPane *LEDTestPane::_this;

laBool LEDTestPane::SGraphAreaPaint(laDrawSurfaceWidget *sfc, GFX_Rect *bounds)
{
    if (_this)
        return _this->GraphAreaPaint(sfc, bounds);
    return LA_TRUE;
}

LEDTestPane::LEDTestPane(int32_t xMax, int32_t yMax) :
    _oldCurrent{0, 0}, _current{0, 0}, _xMax(xMax), _yMax(yMax)
{
    _this = this;
    if (!_cursor)
    {
        _cursor = laString_New(NULL);
        //laString s = laString_CreateFromUTF8Buffer(UTF8_MULTIPLICATION, &RegularFont);
        //laString_Copy(_cursor, &s);
        //laString_Destroy(&s);
        // This is NOT the right way to initialize the string. But for some reason,
        // laString_Draw() doesn't draw the sting when it's initialized the right way
        _cursor->table_index = string_WorldMapCursorChar;
        
        laString_GetRect(_cursor, &_cursorRect);
    }
    
    laDrawSurfaceWidget_SetDrawCallback(IVGraphArea, SGraphAreaPaint);
}

LEDTestPane::~LEDTestPane() 
{
    laDrawSurfaceWidget_SetDrawCallback(IVGraphArea, NULL);
    _this = NULL;
}

void LEDTestPane::Update()
{
    laWidget_Invalidate((laWidget *) IVGraphArea);
}

void LEDTestPane::SetData(const std::vector<XY> &data)
{
    _data = data;
    Update();
}

void LEDTestPane::SetCurrentLevel(XY &data)
{
    _current = data;
    
    // Set the text display
    char buf[20];
    sprintf(buf, "%2dmA", _current.Y / 1000);
    laString s = laString_CreateFromCharBuffer(buf, &MonoFont);
    laLabelWidget_SetText(mASetting, s);
    laString_Destroy(&s);
    sprintf(buf, "%d.%dV", _current.X / 1000, _current.X / 100 % 10);
    s = laString_CreateFromCharBuffer(buf, &MonoFont);
    laLabelWidget_SetText(VfSetting, s);
    laString_Destroy(&s);

    Update();
}

laBool LEDTestPane::GraphAreaPaint(laDrawSurfaceWidget *sfc, GFX_Rect *bounds)
{
    if (_data.empty())
        return LA_TRUE;
    
    // Figure out where we're drawing the points
    int32_t graphWidth = laWidget_GetWidth((laWidget *) IVGraphArea);
    int32_t graphHeight = laWidget_GetHeight((laWidget *) IVGraphArea);
    GFX_Rect graphRect = laWidget_RectToScreenSpace((laWidget *) IVGraphArea);

    // Draw the line
    int32_t oldX = 0, oldY = 0;
    GFX_Set(GFXF_DRAW_COLOR, 0);
    for (auto &pt : _data)
    {
        if (oldX >= 0 && oldX < _xMax && oldY >= 0 && oldY < _yMax &&
            pt.X >=0 && pt.X < _xMax && pt.Y >= 0 && pt.Y < _yMax)
        {
            int32_t x1 = oldX * graphWidth / _xMax;
            int32_t y1 = graphHeight - oldY * graphHeight / _yMax;
            int32_t x2 = pt.X * graphWidth / _xMax;
            int32_t y2 = graphHeight - pt.Y * graphHeight / _yMax;

            GFX_DrawLine(x1 + graphRect.x, y1 + graphRect.y,
                x2 + graphRect.x, y2 + graphRect.y);
        }

        oldX = pt.X;
        oldY = pt.Y;
    }
    
    // Show the cursor
    GFX_Set(GFXF_DRAW_COLOR, RGB(0xff, 0, 0));
    laString_Draw(_cursor, 
        _current.X * graphWidth / _xMax - _cursorRect.width / 2 + graphRect.x,
        graphHeight - _current.Y * graphHeight / _yMax - _cursorRect.height / 2 + graphRect.y,
        NULL);
    
    _oldCurrent = _current;
    return LA_TRUE;
}

laWidget *LEDTestPane::GetWidget() const
{
    return LEDTestPanel;
}
