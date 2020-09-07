/* 
 * File:   RichTextPainter.cpp
 * Author: Bob
 * 
 * Created on May 1, 2020, 11:19 AM
 */

#include <map>
#include <algorithm>
extern "C" 
{
#include "definitions.h"
}
#include "RichTextPainter.h"
#include "Utility.h"

RichTextPainter::~RichTextPainter() 
{
}

bool RichTextPainter::OnDraw(SurfaceWrapper *surface, GFX_Rect *bounds)
{
    if (_richText.size())
    {
        laString strings[_richText.size()];
        GFX_Rect rects[_richText.size()];
        
        int32_t totalWidth = 0;
        
        size_t index = 0;
        for (auto &rt : _richText)
        {
            strings[index] = laString_CreateFromUTF8Buffer(rt.text.c_str(), rt.font);
            laString_GetRect(&strings[index], &rects[index]);
            totalWidth += rects[index].width;
            ++index;
        }
        
        int32_t x;
        
        if (_just == Left)
            x = bounds->x;
        else if (_just == Center)
            x = bounds->x + (bounds->width - totalWidth) / 2;
        else
            x = bounds->x + bounds->width - totalWidth;
        
        index = 0;
        for (auto &rt : _richText)
        {
            int32_t fontHeight = std::min(rects[index].height + 2, bounds->height);
            GFX_Set(GFXF_DRAW_COLOR, rt.backgroundColor);
            GFX_DrawRect(x + _xOffset, bounds->y + _yOffset, rects[index].width, fontHeight);
            
            GFX_Set(GFXF_DRAW_COLOR, rt.textColor);
            laString_DrawClipped(&strings[index], x + _xOffset, bounds->y + _yOffset, 
                    bounds->width - (x - bounds->x), fontHeight,
                    x + _xOffset, bounds->y + _yOffset + rt.verticalOffset, NULL);
            laString_Destroy(&strings[index]);
            
            x += rects[index].width;
            ++index;
        }
    }
    
    return true;
}
