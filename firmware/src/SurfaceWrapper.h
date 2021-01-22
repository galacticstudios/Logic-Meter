/* 
 * File:   SurfaceWrapper.h
 * Author: Bob
 *
 * Created on May 14, 2020, 9:48 AM
 */

#ifndef SURFACEWRAPPER_H
#define	SURFACEWRAPPER_H

#include <vector>
extern "C" 
{
#include "definitions.h"
}

struct laDrawSurfaceWidget_t;
typedef laDrawSurfaceWidget_t laDrawSurfaceWidget;
struct GFX_Rect_t;
typedef struct GFX_Rect_t GFX_Rect;

class SurfacePainter;
static laBool DrawSurfaceCallback(laDrawSurfaceWidget *surface, GFX_Rect *bounds);

class SurfaceWrapper 
{
public:
    SurfaceWrapper(laDrawSurfaceWidget *surface) : _surface(NULL) {Attach(surface);}
    ~SurfaceWrapper() {Detach();}
    
    void Attach(laDrawSurfaceWidget *surface);
    laDrawSurfaceWidget *Detach();
    
    void Invalidate();
    laWidget *GetSurface() const {return (laWidget *) _surface;}
    
    void Register(SurfacePainter *painter) {_painters.push_back(painter); Invalidate();}
    void Unregister(SurfacePainter *painter);
    
private:
    friend laBool DrawSurfaceCallback(laDrawSurfaceWidget *surface, GFX_Rect *bounds);
    bool DrawSurfaceCallback(GFX_Rect *bounds);
    
    SurfaceWrapper(const SurfaceWrapper& orig);

    laDrawSurfaceWidget *_surface;
    laDrawSurfaceWidget_DrawCallback _oldCallback;
    std::vector<SurfacePainter *> _painters;
};

#endif	/* SURFACEWRAPPER_H */

