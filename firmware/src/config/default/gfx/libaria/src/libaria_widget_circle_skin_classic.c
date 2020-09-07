// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END


#include "gfx/libaria/inc/libaria_widget_circle.h"

#if LA_CIRCLE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
    DRAW_CIRCLE,
    DRAW_BORDER,
};

static void drawBackground(laCircleWidget* cir);
static void drawCircle(laCircleWidget* cir);
static void drawBorder(laCircleWidget* cir);

static void nextState(laCircleWidget* cir)
{
    switch(cir->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(cir->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                cir->widget.drawState = DRAW_BACKGROUND;
                cir->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            cir->widget.drawState = DRAW_CIRCLE;
            cir->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawCircle;
    
            return;
        }
        case DRAW_CIRCLE:
        {            
            if(cir->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                cir->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                cir->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            cir->widget.drawState = DONE;
            cir->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laCircleWidget* cir)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)cir);
    
    nextState(cir);
}

static void drawCircle(laCircleWidget* cir)
{
    GFX_Point p;
    GFX_Rect circleRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)cir);
    
    p.x = cir->x;
    p.y = cir->y;
    
    laUtils_PointToLayerSpace((laWidget*)cir, &p);
    
    circleRect.x = p.x - cir->radius;
    circleRect.y = p.y - cir->radius;
    circleRect.width = cir->radius * 2;
    circleRect.height = cir->radius * 2;
    
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &circleRect) == GFX_TRUE)
    {
        GFX_RectClip(&circleRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
        GFX_Set(GFXF_DRAW_CLIP_ENABLE, &clipRect);
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
        GFX_Set(GFXF_DRAW_COLOR, cir->widget.scheme->foreground);
        
        GFX_Set(GFXF_DRAW_THICKNESS, cir->thickness);
        GFX_DrawArc(p.x, p.y, cir->radius, 0, 360);
        
        if (cir->filled == LA_TRUE && 
            cir->thickness < cir->radius)
        {
            GFX_Set(GFXF_DRAW_COLOR, cir->widget.scheme->background);
            GFX_Set(GFXF_DRAW_THICKNESS, cir->radius - cir->thickness );
            GFX_DrawArc(p.x,
                        p.y,
                        cir->radius - cir->thickness,
                        0,
                        360);
        }
    }

    nextState(cir);
}

static void drawBorder(laCircleWidget* cir)
{    
    if(cir->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)cir);
    else if(cir->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)cir);
    
    nextState(cir);
}

void _laCircleWidget_Paint(laCircleWidget* cir)
{
    laContext* context = laContext_GetActive();
    
    if(cir->widget.scheme == NULL)
    {
        cir->widget.drawState = DONE;
        
        return;
    }
    
    if(cir->widget.drawState == NOT_STARTED)
        nextState(cir);
    
    while(cir->widget.drawState != DONE)
    {
        cir->widget.drawFunc((laWidget*)cir);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_CIRCLE_WIDGET_ENABLED
