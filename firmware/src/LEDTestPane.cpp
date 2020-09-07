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

LEDTestPane::LEDTestPane(int32_t xMax, int32_t yMax) :
    _oldCurrent{0, 0}, _current{0, 0}, _xMax(xMax), _yMax(yMax)
{
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
}

LEDTestPane::~LEDTestPane() 
{
}

void LEDTestPane::Update()
{
    uint16_t paneX, paneY;
    uint16_t ivpaneX, ivpaneY;
    uint16_t ivpaneWidth, ivpaneHeight;
    int32_t oldX = 0, oldY = 0;
    
    if (_erase.empty() && _data.empty())
        return;
    
    // Aria doesn't draw everything at once. So make sure it's done
    // drawing all the widgets before we directly draw stuff that's
    // not in the widget tree
    while (laContext_IsDrawing())
    {
        laUpdate(0);
    }
    
    // Figure out where we're drawing the points
    paneX = laWidget_GetX(GetWidget());
    paneY = laWidget_GetY(GetWidget());
    ivpaneX = laWidget_GetX(IVPanel);
    ivpaneY = laWidget_GetY(IVPanel);
    ivpaneWidth = laWidget_GetWidth(IVPanel);
    ivpaneHeight = laWidget_GetHeight(IVPanel);
    
    GFX_Begin();
    
    // Erase the old line
    GFX_Set(GFXF_DRAW_COLOR, 0xffff);
    for (auto &pt : _erase)
    {
        int32_t x1 = oldX * ivpaneWidth / _xMax;
        int32_t y1 = ivpaneHeight - oldY * ivpaneHeight / _yMax;
        int32_t x2 = pt.X * ivpaneWidth / _xMax;
        int32_t y2 = ivpaneHeight - pt.Y * ivpaneHeight / _yMax;
        if (x1 >= 0 && y1 >= 0 && x1 < ivpaneWidth && y1 < ivpaneHeight &&
            x2 >= 0 && y2 >= 0 && x2 < ivpaneWidth && y2 < ivpaneHeight)
        {
            GFX_DrawLine(x1 + paneX + ivpaneX, y1 + paneY + ivpaneY, 
                    x2 + paneX + ivpaneX, y2 + paneY + ivpaneY);
        }
        oldX = pt.X;
        oldY = pt.Y;
    }
    
    // Erase the old cursor
    laString_Draw(_cursor, 
        _oldCurrent.X * ivpaneWidth / _xMax - _cursorRect.width / 2 + paneX + ivpaneX,
        ivpaneHeight - _oldCurrent.Y * ivpaneHeight / _yMax - _cursorRect.height / 2 + paneY + ivpaneY,
        NULL);

    // Draw the new line
    oldX = oldY = 0;
    GFX_Set(GFXF_DRAW_COLOR, 0);
    for (auto &pt : _data)
    {
        if (oldX >= 0 && oldX < _xMax && oldY >= 0 && oldY < _yMax &&
            pt.X >=0 && pt.X < _xMax && pt.Y >= 0 && pt.Y < _yMax)
        {
            int32_t x1 = oldX * ivpaneWidth / _xMax;
            int32_t y1 = ivpaneHeight - oldY * ivpaneHeight / _yMax;
            int32_t x2 = pt.X * ivpaneWidth / _xMax;
            int32_t y2 = ivpaneHeight - pt.Y * ivpaneHeight / _yMax;

            GFX_DrawLine(x1 + paneX + ivpaneX, y1 + paneY + ivpaneY,
                x2 + paneX + ivpaneX, y2 + paneY + ivpaneY);
        }

        oldX = pt.X;
        oldY = pt.Y;
    }
    
    // Show the cursor
    GFX_Set(GFXF_DRAW_COLOR, RGB(0xff, 0, 0));
    laString_Draw(_cursor, 
        _current.X * ivpaneWidth / _xMax - _cursorRect.width / 2 + paneX + ivpaneX,
        ivpaneHeight - _current.Y * ivpaneHeight / _yMax - _cursorRect.height / 2 + paneY + ivpaneY,
        NULL);
    
    GFX_End();
    
    _erase = _data;
    _oldCurrent = _current;
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

laWidget *LEDTestPane::GetWidget() const
{
    return LEDTestPanel;
}
