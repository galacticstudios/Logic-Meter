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


#include "gfx/libaria/inc/libaria_widget_gradient.h"

#if LA_GRADIENT_WIDGET_ENABLED

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
    DRAW_BORDER,
};

static void drawBackground(laGradientWidget* grad);
static void drawBorder(laGradientWidget* grad);

static void nextState(laGradientWidget* grad)
{
    switch(grad->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(grad->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                grad->widget.drawState = DRAW_BACKGROUND;
                grad->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            if(grad->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                grad->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                grad->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            grad->widget.drawState = DONE;
            grad->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laGradientWidget* grad)
{
    GFX_Rect rect, clipRect;
    GFX_Color lclr, rclr;
    uint32_t flag;
    
    laLayer* layer = laUtils_GetLayer((laWidget*)grad);
    
    if(grad->widget.backgroundType == LA_WIDGET_BACKGROUND_FILL)
    {
        rect = laUtils_WidgetLayerRect((laWidget*)grad);
        
        if(GFX_RectIntersects(&layer->clippedDrawingRect, &rect) == GFX_TRUE)
        {        
            GFX_RectClip(&rect, &layer->clippedDrawingRect, &clipRect);
            
            if(grad->dir == LA_GRADIENT_DIRECTION_RIGHT ||
               grad->dir == LA_GRADIENT_DIRECTION_DOWN)
            {
                lclr = grad->widget.scheme->foreground;
                rclr = grad->widget.scheme->foregroundInactive;
            }
            else
            {
                lclr = grad->widget.scheme->foregroundInactive;
                rclr = grad->widget.scheme->foreground;
            }
            
            if(grad->dir == LA_GRADIENT_DIRECTION_RIGHT ||
               grad->dir == LA_GRADIENT_DIRECTION_LEFT)
            {
                flag = GFX_DRAW_GRADIENT_LEFT_RIGHT;
            }
            else
            {
                flag = GFX_DRAW_GRADIENT_TOP_BOTTOM;
            }
            
            GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
            GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);
            GFX_Set(GFXF_DRAW_GRADIENT_COLOR, lclr, rclr, NULL, NULL);
            GFX_Set(GFXF_DRAW_MODE, flag);
            GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
            
            GFX_DrawRect(rect.x, rect.y, rect.width, rect.height);
        }
    }
    else if(grad->widget.backgroundType == LA_WIDGET_BACKGROUND_CACHE)
    {
        laWidget_SkinClassic_DrawBlit((laWidget*)grad, grad->widget.cache);
    }
    
    nextState(grad);
}

static void drawBorder(laGradientWidget* grad)
{
    if(grad->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)grad);
    else if(grad->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)grad);
        
    nextState(grad);
}

void _laGradientWidget_Paint(laGradientWidget* grad)
{
    laContext* context = laContext_GetActive();
    
    if(grad->widget.scheme == NULL)
    {
        grad->widget.drawState = DONE;
        
        return;
    }
    
    if(grad->widget.drawState == NOT_STARTED)
        nextState(grad);
    
    while(grad->widget.drawState != DONE)
    {
        grad->widget.drawFunc((laWidget*)grad);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_GRADIENT_WIDGET_ENABLED