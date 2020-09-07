/* 
 * File:   HWScrollPane.cpp
 * Author: Bob
 * 
 * Created on May 13, 2020, 1:11 PM
 */

extern "C" 
{
#include "definitions.h"
#include "gfx/hal/inc/gfx_context.h"
}
#include "gfx/driver/controller/ili9488/drv_gfx_ili9488_common.h"
#include "HWScroller.h"

#define ILI9488_CMD_VERTICAL_SCROLLING_DEFINITION 0x33
#define ILI9488_CMD_VERTICAL_SCROLLING_START_ADDRESS 0x37

HWScroller::HWScroller(laWidget *widget) :
    _widget(widget)
{
}

void HWScroller::EnableScrolling()
{
    laScreen *screen = laContext_GetActiveScreen();
    laWidget *layer = (laWidget *) screen->layers[0];
    int32_t layerHeight = laWidget_GetHeight(layer);
    
    int32_t startY = laWidget_GetY(_widget), height = laWidget_GetHeight(_widget);
    int32_t bottomAreaHeight = layerHeight - startY - height;
    uint8_t parms[6] = {
        uint8_t(startY >> 8), uint8_t(startY),
        uint8_t(height >> 8), uint8_t(height),
        uint8_t(bottomAreaHeight >> 8), uint8_t(bottomAreaHeight)
    };
    GFX_Context *context = GFX_ActiveContext();
    ILI9488_DRV *drv = (ILI9488_DRV *) context->driver_data;
    ILI9488_Intf_WriteCmd(drv,
        ILI9488_CMD_VERTICAL_SCROLLING_DEFINITION,
        parms, sizeof(parms));
    
    ScrollTo(0);
}

void HWScroller::ScrollTo(int32_t pos)
{   
    int32_t startY = laWidget_GetY(_widget);

    int32_t framePos = pos + startY;
    uint8_t parms[2] = {
        uint8_t(framePos >> 8), uint8_t(framePos)
    };
    GFX_Context *context = GFX_ActiveContext();
    ILI9488_DRV *drv = (ILI9488_DRV *) context->driver_data;
    ILI9488_Intf_WriteCmd(drv,
        ILI9488_CMD_VERTICAL_SCROLLING_START_ADDRESS,
        parms, sizeof(parms));
    
    _scrollPos = pos;
}

void HWScroller::ScrollBy(int32_t delta)
{
    ScrollTo(_scrollPos + delta);
}

