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


#include "gfx/libaria/inc/libaria_widget_line.h"

#if LA_LINE_WIDGET_ENABLED

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
    DRAW_LINE,
    DRAW_BORDER,
};

static void drawBackground(laLineWidget* line);
static void drawLine(laLineWidget* line);
static void drawBorder(laLineWidget* line);

static void nextState(laLineWidget* line)
{
    switch(line->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(line->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                line->widget.drawState = DRAW_BACKGROUND;
                line->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            line->widget.drawState = DRAW_LINE;
            line->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawLine;
    
            return;
        }
        case DRAW_LINE:
        {            
            if(line->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                line->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                line->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            line->widget.drawState = DONE;
            line->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laLineWidget* line)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)line);
    
    nextState(line);
}

static void drawLine(laLineWidget* line)
{
    GFX_Point p1, p2;
    GFX_Rect lineRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)line);
    
    p1.x = line->x1;
    p1.y = line->y1;
    p2.x = line->x2;
    p2.y = line->y2;
    
    laUtils_PointToLayerSpace((laWidget*)line, &p1);
    laUtils_PointToLayerSpace((laWidget*)line, &p2);
    
    lineRect.x = p1.x;
    lineRect.y = p1.y;
    lineRect.width = p2.x - p1.x;
    lineRect.height = p2.y - p1.y;

    if ((GFX_RectContainsPoint(&layer->clippedDrawingRect, &p1) == GFX_TRUE) || 
        (GFX_RectContainsPoint(&layer->clippedDrawingRect, &p2) == GFX_TRUE) ||
        (GFX_RectIntersects(&layer->clippedDrawingRect, &lineRect) == GFX_TRUE))
    {        
        GFX_RectClip(&lineRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
        GFX_Set(GFXF_DRAW_CLIP_ENABLE, &clipRect);
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
        GFX_Set(GFXF_DRAW_COLOR, line->widget.scheme->foreground);
        
        GFX_DrawLine(p1.x, p1.y, p2.x, p2.y);
    }
    
    nextState(line);
}

static void drawBorder(laLineWidget* line)
{
    if(line->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)line);
    else if(line->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)line);
    
    nextState(line);
}

void _laLineWidget_Paint(laLineWidget* line)
{
    laContext* context = laContext_GetActive();
    
    if(line->widget.scheme == NULL)
    {
        line->widget.drawState = DONE;
        
        return;
    }
    
    if(line->widget.drawState == NOT_STARTED)
        nextState(line);
    
    while(line->widget.drawState != DONE)
    {
        line->widget.drawFunc((laWidget*)line);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_LINE_WIDGET_ENABLED