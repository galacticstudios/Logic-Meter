/* 
 * File:   LinesPainter.h
 * Author: Bob
 *
 * Created on May 13, 2020, 9:14 AM
 */

#ifndef LINESPAINTER_H
#define	LINESPAINTER_H

#include <vector>
#include "SurfacePainter.h"

struct Point
{
    int32_t x, y;
};

class LinesPainter : public SurfacePainter
{
public:
    LinesPainter(int32_t xOffset = 0, int32_t yOffset = 0) : SurfacePainter(xOffset, yOffset) {}
    virtual ~LinesPainter();

    std::vector<Point> &Points() {return _points;}
    
    bool OnDraw(SurfaceWrapper *surface, GFX_Rect *bounds);
    
private:
    LinesPainter(const LinesPainter& orig);
    void DrawLines(GFX_Rect *bounds, const std::vector<Point> &points);

    std::vector<Point> _points, _previouslyDrawn;
};

#endif	/* LINESPAINTER_H */

