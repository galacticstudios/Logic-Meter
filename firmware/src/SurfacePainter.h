/* 
 * File:   SurfacePainter.h
 * Author: Bob
 *
 * Created on May 14, 2020, 10:02 AM
 */

#ifndef SURFACEPAINTER_H
#define	SURFACEPAINTER_H

#include <stdint.h>
class SurfaceWrapper;
struct GFX_Rect_t;
typedef struct GFX_Rect_t GFX_Rect;

class SurfacePainter 
{
public:
    SurfacePainter(int32_t xOffset = 0, int32_t yOffset = 0) :
        _xOffset(xOffset), _yOffset(yOffset) {}
    virtual ~SurfacePainter() {}
    
    void SetOffset(int32_t xOffset, int32_t yOffset)
    {
        _xOffset = xOffset;
        _yOffset = yOffset;
    }
    
    virtual bool OnDraw(SurfaceWrapper *surface, GFX_Rect *bounds) = 0;
    
protected:
    int32_t _xOffset, _yOffset;
    
private:
    SurfacePainter(const SurfacePainter& orig);
};

#endif	/* SURFACEPAINTER_H */

