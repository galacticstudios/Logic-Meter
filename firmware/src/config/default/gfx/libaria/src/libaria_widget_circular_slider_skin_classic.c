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


#include "gfx/libaria/inc/libaria_widget_circular_slider.h"

#if LA_CIRCULAR_SLIDER_WIDGET_ENABLED

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
    DRAW_CIRCULAR_SLIDER,
    DRAW_BORDER,
};

static void drawBackground(laCircularSliderWidget* slider);
static void drawCircularSlider(laCircularSliderWidget* slider);
static void drawBorder(laCircularSliderWidget* slider);

static void nextState(laCircularSliderWidget* slider)
{
    switch(slider->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(slider->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                slider->widget.drawState = DRAW_BACKGROUND;
                slider->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            slider->widget.drawState = DRAW_CIRCULAR_SLIDER;
            slider->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawCircularSlider;
    
            return;
        }
        case DRAW_CIRCULAR_SLIDER:
        {            
            if(slider->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                slider->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                slider->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            slider->widget.drawState = DONE;
            slider->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laCircularSliderWidget* slider)
{
    if(slider->widget.backgroundType == LA_WIDGET_BACKGROUND_FILL)
    {
        laWidget_SkinClassic_DrawBackground((laWidget*) slider, 
                                             slider->widget.scheme->background);
    }
    else 
    {
        laWidget_SkinClassic_DrawBlit((laWidget*) slider, slider->widget.cache);
    }
    
    nextState(slider);
}

static void drawCircularSlider(laCircularSliderWidget* slider)
{
    GFX_Point p;
    GFX_Rect sliderRect, clipRect;
    laLayer* layer = laUtils_GetLayer((laWidget*)slider);
    int32_t valueCenterAngle;
    
    p.x = slider->widget.rect.width/2;
    p.y = slider->widget.rect.height/2;
    
    laUtils_PointToLayerSpace((laWidget*)slider, &p);
    
    sliderRect.x = 0;
    sliderRect.y = 0;
    sliderRect.width = slider->widget.rect.width;
    sliderRect.height = slider->widget.rect.height;

    valueCenterAngle = (int32_t) ((slider->value - slider->startValue) * slider->degPerUnit);
    
    laUtils_RectToLayerSpace((laWidget*)slider, &sliderRect);
    
    if(GFX_RectIntersects(&layer->clippedDrawingRect, &sliderRect) == GFX_TRUE)
    {
        GFX_RectClip(&sliderRect, &layer->clippedDrawingRect, &clipRect);
        
        GFX_Set(GFXF_DRAW_CLIP_RECT, &clipRect);
        GFX_Set(GFXF_DRAW_CLIP_ENABLE, GFX_TRUE);
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
        GFX_Set(GFXF_DRAW_COLOR, slider->widget.scheme->foreground);

        GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
                slider->widget.scheme->foreground & 0xffffff00, 
                slider->widget.scheme->foreground, 
                NULL, 
                NULL);

        //Draw inactive arc
        if (slider->inActiveArc.visible == LA_TRUE)
        {
            GFX_Set(GFXF_DRAW_COLOR, slider->widget.scheme->foregroundInactive);
            GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
                    slider->widget.scheme->foregroundInactive & 0xffffff00, 
                    slider->widget.scheme->foregroundInactive, 
                    NULL, 
                    NULL);

            if (slider->direction == CIRCULAR_SLIDER_DIR_COUNTER_CLOCKWISE)
            {
                GFX_Set(GFXF_DRAW_THICKNESS, slider->inActiveArc.thickness);
                GFX_DrawArc(p.x, 
                            p.y, 
                            slider->radius - slider->outsideBorderArc.thickness, 
                            (slider->startAngle + valueCenterAngle), 
                            360 - valueCenterAngle);
            }
            else
            {
                int32_t start, center;
                start = slider->startAngle;
                center = 360 - valueCenterAngle;

                GFX_Set(GFXF_DRAW_THICKNESS, slider->inActiveArc.thickness);
                GFX_DrawArc(p.x, 
                            p.y, 
                            slider->radius - slider->outsideBorderArc.thickness,
                            start, 
                            center);
            }
        }
        
        //Draw active arc
        if (slider->activeArc.visible == LA_TRUE)
        {
            GFX_Set(GFXF_DRAW_COLOR, slider->widget.scheme->foreground);
            GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
                    slider->widget.scheme->foreground & 0xffffff00, 
                    slider->widget.scheme->foreground, 
                    NULL, 
                    NULL);

            if (slider->direction == CIRCULAR_SLIDER_DIR_COUNTER_CLOCKWISE)
            {
                GFX_Set(GFXF_DRAW_THICKNESS, slider->activeArc.thickness);
                GFX_DrawArc(p.x, 
                            p.y,
                            slider->radius - slider->outsideBorderArc.thickness, 
                            slider->startAngle, 
                            valueCenterAngle);
            }
            else
            {
                GFX_Set(GFXF_DRAW_THICKNESS, slider->activeArc.thickness);
                GFX_DrawArc(p.x, 
                            p.y, 
                            slider->radius - slider->outsideBorderArc.thickness, 
                            slider->startAngle, 
                            -valueCenterAngle);
            }

            //Draw round edges
            if (slider->roundEdges == LA_TRUE)
            {
                //Draw start round edge
                GFX_Point center = _laCircularSliderWidget_GetCircleCenterPointAtValue(slider, slider->startValue);

                //Draw a full circle for now
                GFX_Set(GFXF_DRAW_COLOR, slider->widget.scheme->foreground);
                GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
                        slider->widget.scheme->foreground & 0xffffff00, 
                        slider->widget.scheme->foreground, 
                        NULL, NULL);
                GFX_Set(GFXF_DRAW_THICKNESS, slider->activeArc.thickness/2);
                GFX_DrawArc(center.x,
                            center.y,
                            slider->activeArc.thickness/2,
                            0,
                            360);
                
                //Draw end round edge if circle button is not visible
                if (slider->circleButtonArc.visible != LA_TRUE)
                {
                    //Draw end round edge
                    GFX_Point center = _laCircularSliderWidget_GetCircleCenterPointAtValue(slider, slider->value);

                    //Draw a full circle for now
                    GFX_Set(GFXF_DRAW_COLOR, slider->widget.scheme->foreground);
                    GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
                            slider->widget.scheme->foreground & 0xffffff00, 
                            slider->widget.scheme->foreground, 
                            NULL, NULL);
                    GFX_Set(GFXF_DRAW_THICKNESS, slider->activeArc.thickness/2);
                    GFX_DrawArc(center.x,
                                center.y,
                                slider->activeArc.thickness/2,
                                0,
                                360);       
                }
            }
        }

        //Draw outer arc
        if (slider->outsideBorderArc.visible == LA_TRUE)
        {
            GFX_Set(GFXF_DRAW_COLOR, slider->widget.scheme->base);
            GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
                    slider->widget.scheme->base& 0xffffff00, 
                    slider->widget.scheme->base, 
                    NULL, 
                    NULL);
            GFX_Set(GFXF_DRAW_THICKNESS, slider->outsideBorderArc.thickness);
            GFX_DrawArc(p.x, p.y, 
                        slider->radius,
                        0,
                        360);
        }
        
        //Draw inner arc
        if (slider->insideBorderArc.visible == LA_TRUE)
        {
            GFX_Set(GFXF_DRAW_COLOR, slider->widget.scheme->base);
            GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
                    slider->widget.scheme->base& 0xffffff00, 
                    slider->widget.scheme->base, 
                    NULL, NULL);
            GFX_Set(GFXF_DRAW_THICKNESS, slider->insideBorderArc.thickness);
            GFX_DrawArc(p.x, 
                        p.y, 
                        slider->radius - slider->outsideBorderArc.thickness - slider->activeArc.thickness, 
                        0, 
                        360);
        }

        //Draw circular button
        if (slider->circleButtonArc.visible == LA_TRUE)
        {
            GFX_Point center = _laCircularSliderWidget_GetCircleCenterPointAtValue(slider, slider->value);

            //Draw outside border
            GFX_Set(GFXF_DRAW_COLOR, slider->circleButtonArc.scheme->base);
            GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
                    slider->circleButtonArc.scheme->base & 0xffffff00, 
                    slider->circleButtonArc.scheme->base, 
                    NULL, NULL);
            GFX_Set(GFXF_DRAW_THICKNESS, slider->circleButtonArc.thickness);
            GFX_DrawArc(center.x,
                        center.y,
                        slider->circleButtonArc.radius,
                        0,
                        360);

            if (slider->circleButtonArc.radius != slider->circleButtonArc.thickness)
            {
                //Draw circle fill
                GFX_Set(GFXF_DRAW_COLOR, slider->circleButtonArc.scheme->foreground);
                GFX_Set(GFXF_DRAW_GRADIENT_COLOR, 
                        slider->circleButtonArc.scheme->foreground & 0xffffff00, 
                        slider->circleButtonArc.scheme->foreground,
                        NULL, NULL);
                GFX_Set(GFXF_DRAW_THICKNESS, slider->circleButtonArc.radius - slider->circleButtonArc.thickness);
                GFX_DrawArc(center.x, 
                            center.y, 
                            slider->circleButtonArc.radius - slider->circleButtonArc.thickness, 
                            0,
                            360);
            }
        }
    }

    nextState(slider);
}

static void drawBorder(laCircularSliderWidget* slider)
{    
    if(slider->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)slider);
    else if(slider->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)slider);
    
    nextState(slider);
}

void _laCircularSliderWidget_Paint(laCircularSliderWidget* slider)
{
    laContext* context = laContext_GetActive();
    
    if(slider->widget.scheme == NULL)
    {
        slider->widget.drawState = DONE;
        
        return;
    }
    
    if(slider->widget.drawState == NOT_STARTED)
        nextState(slider);
    
    while(slider->widget.drawState != DONE)
    {
        slider->widget.drawFunc((laWidget*)slider);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_CIRCULAR_SLIDER_WIDGET_ENABLED
