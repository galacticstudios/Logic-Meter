/* 
 * File:   LinesWidget.cpp
 * Author: Bob
 * 
 * Created on May 13, 2020, 9:14 AM
 */

extern "C" 
{
#include "definitions.h"
}
#include "LinesPainter.h"

LinesPainter::~LinesPainter() 
{
}

bool LinesPainter::OnDraw(SurfaceWrapper *surface, GFX_Rect *bounds)
{
    // Erase any previously drawn lines
    if (_previouslyDrawn.size())
    {
        GFX_Set(GFXF_DRAW_COLOR, 0xffff);
        DrawLines(bounds, _previouslyDrawn);
        _previouslyDrawn.clear();
    }
    if (_points.size())
    {
        GFX_Set(GFXF_DRAW_COLOR, 0x0);
        DrawLines(bounds, _points);
        _previouslyDrawn = _points;
    }
    
    return true;
}

void LinesPainter::DrawLines(GFX_Rect *bounds, const std::vector<Point> &points)
{
    int32_t originx = bounds->x + _xOffset, originy = bounds->y + _yOffset;
    for (size_t i = 1; i < points.size(); ++i)
    {
        GFX_DrawLine(points[i - 1].x + originx, points[i - 1].y + originy, 
                points[i].x + originx, points[i].y + originy);
    }
}
