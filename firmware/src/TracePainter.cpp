/* 
 * File:   TracePainter.cpp
 * Author: Bob
 * 
 * Created on November 9, 2020, 1:15 PM
 */

extern "C" 
{
#include "definitions.h"
}
#include "TracePainter.h"

bool TracePainter::OnDraw(SurfaceWrapper *surface, GFX_Rect *bounds)
{
    GFX_Set(GFXF_DRAW_COLOR, GFX_COLOR_BLACK);
    GFX_DrawRect(bounds->x, bounds->y, bounds->width, bounds->height);

    if (_tracePoints[0] == TracePoint::Blank)
        return true;
    GFX_Set(GFXF_DRAW_COLOR, _color);
    
    for (int32_t col = 0; col < bounds->width; ++col)
    {
        switch (_tracePoints[col])
        {
            case TracePoint::Blank :
                break;
                
            case TracePoint::Low :
                GFX_DrawPixel(bounds->x + col, bounds->y + bounds->height - 1);
                break;
                
            case TracePoint::High :
                GFX_DrawPixel(bounds->x + col, bounds->y);
                break;
                
            case  TracePoint::Edge :
                GFX_DrawLine(bounds->x + col, bounds->y, bounds->x + col, bounds->y + bounds->height - 1);
                break;
        }
    }
    
    return true;
}
