/* 
 * File:   SurfaceWrapper.cpp
 * Author: Bob
 * 
 * Created on May 14, 2020, 9:48 AM
 */

#include <map>
#include <algorithm>
extern "C" 
{
#include "definitions.h"
}
#include "SurfaceWrapper.h"
#include "SurfacePainter.h"

std::map<laDrawSurfaceWidget *, SurfaceWrapper *> _surfaceToWrapper;


static laBool DrawSurfaceCallback(laDrawSurfaceWidget *surface, GFX_Rect *bounds)
{
    return _surfaceToWrapper[surface]->DrawSurfaceCallback(bounds) ? LA_TRUE : LA_FALSE;
}

void SurfaceWrapper::Attach(laDrawSurfaceWidget *surface)
{
    Detach();
    _surface = surface;
    if (_surface)
    {
        _surfaceToWrapper[surface] = this;
        _oldCallback = laDrawSurfaceWidget_GetDrawCallback(surface);
        laDrawSurfaceWidget_SetDrawCallback(surface, ::DrawSurfaceCallback);
        laWidget_Invalidate((laWidget *) surface);
    }
}

laDrawSurfaceWidget *SurfaceWrapper::Detach()
{
    laDrawSurfaceWidget *rtn = _surface;
    if (_surface)
    {
        laDrawSurfaceWidget_SetDrawCallback(_surface, _oldCallback);
        _surfaceToWrapper.erase(_surface);
        _surface = NULL;
    }
    return rtn;
}

void SurfaceWrapper::Invalidate()
{
    if (_surface)
        laWidget_Invalidate((laWidget *) _surface);
}

void SurfaceWrapper::Unregister(SurfacePainter *painter) 
{
    for (size_t i = 0; i < _painters.size(); ++i)
    {
        if (_painters[i] == painter)
        {
            _painters.erase(_painters.begin() + i);
            break;
        }
    }
    Invalidate();
}

bool SurfaceWrapper::DrawSurfaceCallback(GFX_Rect *bounds)
{
    bool done = true;
    // Call all the painters
    for (auto &painter : _painters)
        done &= painter->OnDraw(this, bounds);
    return done;
}
