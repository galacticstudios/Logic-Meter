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


#include "gfx/libaria/inc/libaria_widget_arc.h"

#if LA_ARC_WIDGET_ENABLED

#include "gfx/hal/inc/gfx_math.h"

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
    DRAW_ARC,
    DRAW_BORDER,
};

static void drawBackground(laArcWidget* arc);
static void drawArc(laArcWidget* arc);
static void drawBorder(laArcWidget* arc);

static void nextState(laArcWidget* arc)
{
    switch(arc->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(arc->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                arc->widget.drawState = DRAW_BACKGROUND;
                arc->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            arc->widget.drawState = DRAW_ARC;
            arc->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawArc;
    
            return;
        }
        case DRAW_ARC:
        {            
            if(arc->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                arc->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                arc->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            arc->widget.drawState = DONE;
            arc->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laArcWidget* arc)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)arc);
    
    nextState(arc);
}

//Returns the relative position of the midpoint in the arc
static GFX_Point _laArcWidget_GetMidPointAtAngle(laArcWidget* arc, int32_t angle)
{
    GFX_Point point, arcCenterPoint;
    
    while (angle < 0)
        angle += 360;

    while (angle > 360)
        angle -= 360;
    
    arcCenterPoint.x = arc->widget.rect.width/2;
    arcCenterPoint.y = arc->widget.rect.height/2;
    
    laUtils_PointToLayerSpace((laWidget*)arc, &arcCenterPoint);
    
    GFX_PolarToXY(arc->radius - (arc->thickness/2), angle, &point);

    point.x = arcCenterPoint.x + point.x;
    point.y = arcCenterPoint.y - point.y;
    
    return point;
}

static void drawArc(laArcWidget* arc)
{
    GFX_Point p;
    GFX_Rect arcRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)arc);
    
    p.x = arc->widget.rect.width/2;
    p.y = arc->widget.rect.height/2;
    
    laUtils_PointToLayerSpace((laWidget*)arc, &p);
    
    arcRect.x = p.x - arc->radius;
    arcRect.y = p.y - arc->radius;
    arcRect.width = arc->radius * 2;
    arcRect.height = arc->radius * 2;
    
    
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &arcRect) == GFX_TRUE)
    {
        GFX_RectClip(&arcRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
        GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
        GFX_Set(GFXF_DRAW_COLOR, arc->widget.scheme->foreground);

        GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 0xffffffff/*(arc->widget.scheme->foreground & 0xffffff00)*/, arc->widget.scheme->foreground, NULL, NULL);

        GFX_Set(GFXF_DRAW_THICKNESS, arc->thickness);
        GFX_DrawArc(p.x, p.y, arc->radius, arc->startAngle, arc->centerAngle);
        
        //Draw the edges
        if (arc->roundEdge == LA_TRUE)
        {
            GFX_Point point = _laArcWidget_GetMidPointAtAngle(arc, arc->startAngle);
            
            GFX_Set(GFXF_DRAW_THICKNESS, arc->thickness/2);
            
            GFX_DrawArc(point.x,
                        point.y,
                        arc->thickness/2,
                        0,
                        360);
            
            point = _laArcWidget_GetMidPointAtAngle(arc, arc->startAngle + arc->centerAngle);
            
            GFX_DrawArc(point.x,
                        point.y,
                        arc->thickness/2,
                        0,
                        360);
        }
    }

    nextState(arc);
}

static void drawBorder(laArcWidget* arc)
{    
    if(arc->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)arc);
    else if(arc->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)arc);
    
    nextState(arc);
}

void _laArcWidget_Paint(laArcWidget* arc)
{
    laContext* context = laContext_GetActive();
    
    if(arc->widget.scheme == NULL)
    {
        arc->widget.drawState = DONE;
        
        return;
    }
    
    if(arc->widget.drawState == NOT_STARTED)
        nextState(arc);
    
    while(arc->widget.drawState != DONE)
    {
        arc->widget.drawFunc((laWidget*)arc);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_ARC_WIDGET_ENABLED
