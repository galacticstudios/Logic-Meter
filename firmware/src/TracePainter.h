/* 
 * File:   TracePainter.h
 * Author: Bob
 *
 * Created on November 9, 2020, 1:14 PM
 */

#ifndef TRACEPAINTER_H
#define	TRACEPAINTER_H

#include "SurfacePainter.h"
    
enum class TracePoint : uint8_t {Blank, Low, High, Edge};

class TracePainter : public SurfacePainter
{
public:
    TracePainter(uint16_t color, const TracePoint *tracePoints) : 
        SurfacePainter(), _color(color), _tracePoints(tracePoints) {}
    virtual ~TracePainter() {}
    
    bool OnDraw(SurfaceWrapper *surface, GFX_Rect *bounds);
    
private:
    TracePainter(const TracePainter& orig);
    
    uint16_t _color;
    const TracePoint *_tracePoints;
};

#endif	/* TRACEPAINTER_H */

