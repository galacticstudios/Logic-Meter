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
#include "gfx/hal/inc/gfx_math.h"
#include "gfx/hal/inc/gfx_draw_arc.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_layer.h"

#define DEFAULT_WIDTH           101
#define DEFAULT_HEIGHT          101

#define DEFAULT_ORIGIN_X        50
#define DEFAULT_ORIGIN_Y        50

#define DEFAULT_RADIUS          50
#define DEFAULT_START_ANGLE     0
#define DEFAULT_START_VALUE     0
#define DEFAULT_END_VALUE       100
#define DEFAULT_VALUE           50

#define DEFAULT_DIRECTION       COUNTER_CLOCKWISE

#define DEFAULT_OUTSIDE_BORDER_THICKNESS    5
#define DEFAULT_INSIDE_BORDER_THICKNESS     5
#define DEFAULT_ACTIVE_ARC_THICKNESS        30
#define DEFAULT_SLIDER_BUTTON_THICKNESS     3
#define DEFAULT_SLIDER_BUTTON_RADIUS        15

static void touchDown(laCircularSliderWidget* slider, laInput_TouchDownEvent* evt);
static void touchUp(laCircularSliderWidget* slider, laInput_TouchUpEvent* evt);
static void touchMoved(laCircularSliderWidget* slider, laInput_TouchMovedEvent* evt);

//Returns the center point of the circle button in layer space
GFX_Point _laCircularSliderWidget_GetCircleCenterPointAtValue(laCircularSliderWidget* slider, int32_t value)
{
    //Determine start/end points of activeArc edge
    GFX_Point innerPoint, outerPoint, center, sliderPoint;
    int32_t valueEndAngle;
    int32_t valueCenterAngle;

    valueCenterAngle = (int32_t) ((value - slider->startValue)* slider->degPerUnit);

    sliderPoint.x = slider->widget.rect.width/2;
    sliderPoint.y = slider->widget.rect.height/2;
    
    laUtils_PointToLayerSpace((laWidget*)slider, &sliderPoint);

    if (slider->direction == CIRCULAR_SLIDER_DIR_COUNTER_CLOCKWISE)
    {
        valueEndAngle = slider->startAngle + valueCenterAngle;
    }
    else
    {
        valueEndAngle = 360 + slider->startAngle - valueCenterAngle; 
    }

    if (valueEndAngle < 0)
        valueEndAngle = -valueEndAngle;

    if (valueEndAngle > 360)
        valueEndAngle %= 360;

    GFX_PolarToXY(slider->radius - slider->outsideBorderArc.thickness, valueEndAngle , &outerPoint);
    GFX_PolarToXY((slider->radius - slider->outsideBorderArc.thickness - slider->activeArc.thickness), 
                valueEndAngle , 
                &innerPoint);

    center.x = sliderPoint.x + (innerPoint.x + ((outerPoint.x - innerPoint.x)/2));
    center.y = sliderPoint.y - (innerPoint.y + ((outerPoint.y - innerPoint.y)/2));

    return center;
}

void _laCircularSliderWidget_Constructor(laCircularSliderWidget* slider)
{
    _laWidget_Constructor((laWidget*)slider);

    slider->widget.type = LA_WIDGET_ARC;
    
    slider->widget.destructor = (laWidget_Destructor_FnPtr)&_laCircularSliderWidget_Destructor;

    // override base class methods
    slider->widget.paint = (laWidget_Paint_FnPtr)&_laCircularSliderWidget_Paint;

    slider->widget.rect.width = DEFAULT_WIDTH;
    slider->widget.rect.height = DEFAULT_HEIGHT;

    slider->widget.borderType = LA_WIDGET_BORDER_NONE;
    slider->widget.backgroundType = LA_WIDGET_BACKGROUND_NONE;

    slider->radius = DEFAULT_RADIUS;
    slider->startAngle = DEFAULT_START_ANGLE;
    slider->startValue = DEFAULT_START_VALUE;
    slider->endValue = DEFAULT_END_VALUE;
    slider->value = DEFAULT_VALUE;

    slider->direction = CIRCULAR_SLIDER_DIR_COUNTER_CLOCKWISE;
    slider->roundEdges = LA_TRUE;
    slider->sticky = LA_TRUE;
    slider->buttonTouch = LA_TRUE;

    slider->outsideBorderArc.visible = LA_TRUE;
    slider->radius = DEFAULT_RADIUS;
    slider->outsideBorderArc.thickness = DEFAULT_OUTSIDE_BORDER_THICKNESS;
    slider->outsideBorderArc.startAngle = slider->startAngle;
    slider->outsideBorderArc.centerAngle = 360;
    slider->outsideBorderArc.scheme = slider->widget.scheme;

    slider->activeArc.visible = LA_TRUE;
    slider->activeArc.radius = slider->radius - slider->outsideBorderArc.thickness;
    slider->activeArc.thickness = DEFAULT_ACTIVE_ARC_THICKNESS;
    slider->activeArc.startAngle = slider->startAngle;
    slider->activeArc.centerAngle = 180;
    slider->activeArc.scheme = slider->widget.scheme;

    slider->inActiveArc.visible = LA_TRUE;
    slider->inActiveArc.radius = slider->activeArc.radius;
    slider->inActiveArc.thickness = slider->activeArc.thickness;
    slider->inActiveArc.startAngle = slider->activeArc.startAngle + slider->activeArc.centerAngle;
    slider->inActiveArc.centerAngle = 180;
    slider->inActiveArc.scheme = slider->widget.scheme;

    slider->insideBorderArc.visible = LA_TRUE;
    slider->insideBorderArc.radius = slider->activeArc.radius - slider->activeArc.thickness;
    slider->insideBorderArc.thickness = DEFAULT_INSIDE_BORDER_THICKNESS;
    slider->insideBorderArc.startAngle = slider->startAngle;
    slider->insideBorderArc.centerAngle = 360;
    slider->insideBorderArc.scheme = slider->widget.scheme;

    slider->circleButtonArc.visible = LA_TRUE;
    slider->circleButtonArc.radius = DEFAULT_SLIDER_BUTTON_RADIUS;
    slider->circleButtonArc.thickness = DEFAULT_SLIDER_BUTTON_THICKNESS;
    slider->circleButtonArc.startAngle = 0;
    slider->circleButtonArc.centerAngle = 360;
    slider->circleButtonArc.scheme = slider->widget.scheme;

    slider->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&touchDown;
    slider->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&touchUp;
    slider->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&touchMoved;

    if (slider->endValue != slider->startValue)
        slider->degPerUnit = (float) (360) / (float) (abs(slider->endValue - slider->startValue));
    else
        slider->degPerUnit = 0;
}

void _laCircularSliderWidget_Destructor(laCircularSliderWidget* slider)
{
    _laWidget_Destructor((laWidget*)slider);
}

laCircularSliderWidget* laCircularSliderWidget_New()
{
    laCircularSliderWidget* slider = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    slider = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laCircularSliderWidget));

    if(slider == NULL)
        return NULL;
    
    _laCircularSliderWidget_Constructor(slider);

    return slider;
}

uint32_t laCircularSliderWidget_GetRadius(laCircularSliderWidget* slider)
{
    if(slider == NULL)
        return 0;
        
    return slider->radius;
}

laResult laCircularSliderWidget_SetRadius(laCircularSliderWidget* slider, uint32_t rad)
{
    if(slider == NULL)
        return LA_FAILURE;
        
    if(slider->radius == rad)
        return LA_SUCCESS;
        
    if (slider->radius < (slider->outsideBorderArc.thickness + 
                         slider->activeArc.thickness + 
                         slider->insideBorderArc.thickness))
        return LA_FAILURE;

    slider->radius = rad;
    slider->radius = rad;
    
    laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}

uint32_t laCircularSliderWidget_GetStartAngle(laCircularSliderWidget* slider)
{
    if(slider == NULL)
        return 0;
        
    return slider->startAngle;
}

laResult laCircularSliderWidget_SetStartAngle(laCircularSliderWidget* slider, uint32_t angle)
{
    if(slider == NULL)
        return LA_FAILURE;
        
    if(slider->startAngle == angle)
        return LA_SUCCESS;

    if (angle > 360)
        angle = angle % 360;
    
    slider->startAngle = angle;
    
    laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}

uint32_t laCircularSliderWidget_GetStartValue(laCircularSliderWidget* slider)
{
    if(slider == NULL)
        return 0;
        
    return slider->startValue;
}

laResult laCircularSliderWidget_SetStartValue(laCircularSliderWidget* slider, uint32_t value)
{
    if(slider == NULL)
        return LA_FAILURE;
        
    if(slider->startValue == value)
        return LA_SUCCESS;

    if (value > slider->endValue)
        return LA_FAILURE;
        
    slider->startValue = value;

    if (slider->endValue != slider->startValue)
        slider->degPerUnit = (float) (360) / (float) (abs(slider->endValue - slider->startValue));
    else
        slider->degPerUnit = 0;
    
    laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}

uint32_t laCircularSliderWidget_GetEndValue(laCircularSliderWidget* slider)
{
    if(slider == NULL)
        return 0;
        
    return slider->endValue;
}

laResult laCircularSliderWidget_SetEndValue(laCircularSliderWidget* slider, uint32_t value)
{
    if(slider == NULL)
        return LA_FAILURE;
        
    if(slider->endValue == value)
        return LA_SUCCESS;

    if (value < slider->startValue)
        return LA_FAILURE;
        
    slider->endValue = value;

    if (slider->endValue != slider->startValue)
    {
        slider->degPerUnit = (float) (360) / (float) (abs(slider->endValue - slider->startValue));
    }
    else
    {
        slider->degPerUnit = 0;
    }
    
    laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}

uint32_t laCircularSliderWidget_GetValue(laCircularSliderWidget* slider)
{
    if(slider == NULL)
        return 0;
        
    return slider->value;
}

GFX_Rect _laCircularSliderWidget_GetDamagedRect(laCircularSliderWidget* slider, uint32_t newValue)
{
    GFX_Point centerPtOld, centerPtNew;
    GFX_Rect rect, damagedRect = GFX_Rect_Zero, widgetRect;
    int32_t valueCenterAngleOld, valueCenterAngleNew, valueEndAngleOld, valueEndAngleNew;
    uint32_t offset = 0;
    uint32_t sliderThickness;
    ARC_DIR dir = CCW;
    
    valueCenterAngleOld = (int32_t) ((slider->value - slider->startValue)* slider->degPerUnit);
    valueCenterAngleNew = (int32_t) ((newValue - slider->startValue)* slider->degPerUnit);
    
    if (slider->direction == CIRCULAR_SLIDER_DIR_COUNTER_CLOCKWISE)
    {
        valueEndAngleOld = slider->startAngle + valueCenterAngleOld;
        valueEndAngleNew = slider->startAngle + valueCenterAngleNew;
    }
    else
    {
        valueEndAngleOld = 360 + slider->startAngle - valueCenterAngleOld;
        valueEndAngleNew = 360 + slider->startAngle - valueCenterAngleNew;
    }

    if (valueEndAngleOld < 0)
        valueEndAngleOld = -valueEndAngleOld;

    if (valueEndAngleOld > 360)
        valueEndAngleOld %= 360;
    
    if (valueEndAngleNew < 0)
        valueEndAngleNew = -valueEndAngleNew;

    if (valueEndAngleNew > 360)
        valueEndAngleNew %= 360;    
    
    centerPtOld = _laCircularSliderWidget_GetCircleCenterPointAtValue(slider, slider->value);
    centerPtNew = _laCircularSliderWidget_GetCircleCenterPointAtValue(slider, newValue);
    
    sliderThickness = slider->outsideBorderArc.thickness +
        slider->activeArc.thickness + slider->insideBorderArc.thickness;
            
    if (slider->circleButtonArc.visible == LA_TRUE)
        offset = slider->circleButtonArc.radius;
    
    if (offset < sliderThickness/2)
        offset = sliderThickness/2;
    
    //Combine the rects for old and new value
    rect.x = centerPtOld.x - offset;
    rect.y = centerPtOld.y - offset;
    rect.width = rect.height = offset * 2;
    
    damagedRect = rect;
        
    rect.x = centerPtNew.x - offset;
    rect.y = centerPtNew.y - offset;
    rect.width = rect.height = offset * 2;
    
    damagedRect = GFX_RectCombine(&damagedRect, &rect);
    
    //Combine the rects for traversed quadrants
    widgetRect = laUtils_WidgetLocalRect((laWidget*)slider);
    laUtils_RectToLayerSpace((laWidget*)slider, &widgetRect);
    
    rect.width = slider->radius;
    rect.height = slider->radius;
    
    if (slider->direction == CIRCULAR_SLIDER_DIR_COUNTER_CLOCKWISE)
        if (newValue < slider->value)
            dir = CW;
        else
            dir = CCW;
    else
        if (newValue < slider->value)
            dir = CCW;
        else
            dir = CW;
        
    if (arcsOverlapQuadrant(valueEndAngleOld, 
                            valueEndAngleNew, 
                            dir,
                            Q1) == GFX_TRUE)
    {
        rect.x = widgetRect.x + widgetRect.width / 2;
        rect.y = widgetRect.y + widgetRect.height / 2 - slider->radius;
        
        damagedRect = GFX_RectCombine(&damagedRect, &rect);
    }
        
    if (arcsOverlapQuadrant(valueEndAngleOld, 
                            valueEndAngleNew, 
                            dir,
                            Q2) == GFX_TRUE)
    {
        rect.x = widgetRect.x + widgetRect.width / 2 - slider->radius;
        rect.y = widgetRect.y + widgetRect.height / 2 - slider->radius;
        
        damagedRect = GFX_RectCombine(&damagedRect, &rect);
    }
        
    if (arcsOverlapQuadrant(valueEndAngleOld, 
                            valueEndAngleNew, 
                            dir,
                            Q3) == GFX_TRUE)
    {
        rect.x = widgetRect.x + widgetRect.width / 2 - slider->radius;
        rect.y = widgetRect.y + widgetRect.height / 2;
        
        damagedRect = GFX_RectCombine(&damagedRect, &rect);
    }
        
    if (arcsOverlapQuadrant(valueEndAngleOld, 
                            valueEndAngleNew, 
                            dir,
                            Q4) == GFX_TRUE)

    {
        rect.x = widgetRect.x + widgetRect.width / 2;
        rect.y = widgetRect.y + widgetRect.height / 2;
        
        damagedRect = GFX_RectCombine(&damagedRect, &rect);
    }
            
    return damagedRect;
}

laResult laCircularSliderWidget_SetValue(laCircularSliderWidget* slider, uint32_t value)
{
    GFX_Rect damagedRect;
    GFX_PipelineMode pipelineMode;
    
    if(slider == NULL)
        return LA_FAILURE;
        
    if(slider->value == value)
        return LA_SUCCESS;

    if (value < slider->startValue || value > slider->endValue)
        return LA_FAILURE;
    
    GFX_Get(GFXF_DRAW_PIPELINE_MODE, &pipelineMode);
    if (pipelineMode == GFX_PIPELINE_GPU)
    {
        laWidget_Invalidate((laWidget*)slider);
    }
    else
    {
        damagedRect = _laCircularSliderWidget_GetDamagedRect(slider, value);

        laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)slider),
                                  &damagedRect,
                                  LA_FALSE);
    }

    slider->value = value;
    
    if (slider->valueChangedCallback != NULL)
    {
        slider->valueChangedCallback(slider, slider->value);
    }
        
    return LA_SUCCESS;
}

laBool laCircularSliderWidget_GetRoundEdges(laCircularSliderWidget* slider)
{
    if(slider == NULL)
        return 0;
        
    return slider->roundEdges;
}

laResult laCircularSliderWidget_SetRoundEdges(laCircularSliderWidget* slider, laBool round)
{
    if(slider == NULL)
        return LA_FAILURE;
        
    if(slider->roundEdges == round)
        return LA_SUCCESS;
        
    slider->roundEdges = round;
    
    laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}

laBool laCircularSliderWidget_GetStickyButton(laCircularSliderWidget* slider)
{
    if(slider == NULL)
        return 0;
        
    return slider->sticky;
}

laResult laCircularSliderWidget_SetStickyButton(laCircularSliderWidget* slider, laBool sticky)
{
    if(slider == NULL)
        return LA_FAILURE;
        
    if(slider->sticky == sticky)
        return LA_SUCCESS;
        
    slider->sticky = sticky;
    
    //laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}

laBool laCircularSliderWidget_GetTouchOnButtonOnly(laCircularSliderWidget* slider)
{
    if(slider == NULL)
        return LA_FALSE;
        
    return slider->buttonTouch;
}

laResult laCircularSliderWidget_SetTouchOnButtonOnly(laCircularSliderWidget* slider, laBool buttonOnly)
{
    if(slider == NULL)
        return LA_FAILURE;
        
    if(slider->buttonTouch == buttonOnly)
        return LA_SUCCESS;
        
    slider->buttonTouch = buttonOnly;
    
    //laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}


uint32_t laCircularSliderWidget_GetArcThickness(laCircularSliderWidget* slider, 
                                                laCircularSliderWidgetArcType type)
{
    if(slider == NULL)
        return 0;

    switch(type)
    {
        case OUTSIDE_CIRCLE_BORDER:
            return slider->outsideBorderArc.thickness;
        case INSIDE_CIRCLE_BORDER:
            return slider->insideBorderArc.thickness;
        case ACTIVE_AREA:
            return slider->activeArc.thickness;
        case INACTIVE_AREA:
            return slider->inActiveArc.thickness;
        case CIRCLE_BUTTON:
            return slider->circleButtonArc.thickness;
        default:
            break;
    }
        
    return 0;
}

laResult laCircularSliderWidget_SetArcThickness(laCircularSliderWidget* slider, 
                                                laCircularSliderWidgetArcType type,
                                                uint32_t thickness)
{
    if(slider == NULL)
        return LA_FAILURE;

    switch(type)
    {
        case OUTSIDE_CIRCLE_BORDER:
        {
            //if (thickness == slider->outsideBorderArc.thickness)
            //    return LA_SUCCESS;

            if (thickness > slider->radius)
                return LA_FAILURE;
            
            if (thickness > (slider->radius - 
                            slider->activeArc.thickness - 
                            slider->insideBorderArc.thickness))
                return LA_FAILURE;

            slider->outsideBorderArc.thickness = thickness;

            break;
        }
        case ACTIVE_AREA:
        { 
            //if (thickness == slider->activeArc.thickness)
            //    return LA_SUCCESS;

            if (thickness > slider->radius - slider->outsideBorderArc.thickness)
                return LA_FAILURE;
            
            if (thickness > (slider->radius - slider->outsideBorderArc.thickness - 
                            slider->insideBorderArc.thickness))
                return LA_FAILURE;

            slider->activeArc.thickness = thickness;
            slider->inActiveArc.thickness = thickness;

            slider->insideBorderArc.radius = slider->activeArc.radius - slider->activeArc.thickness;

            break;
        }

        case INSIDE_CIRCLE_BORDER:
        {
            //if (thickness == slider->insideBorderArc.thickness)
            //    return LA_SUCCESS;

            if (thickness > slider->radius - slider->outsideBorderArc.thickness - slider->activeArc.thickness)
                return LA_FAILURE;
            
            slider->insideBorderArc.thickness = thickness;
        
            break;
        }
        case CIRCLE_BUTTON:
        {
            //if (thickness == slider->circleButtonArc.thickness)
            //    return LA_SUCCESS;

            if (thickness > slider->circleButtonArc.radius)
                return LA_FAILURE;

            slider->circleButtonArc.thickness = thickness;

            break;
        }
        default:
            return LA_FAILURE;
    }

    laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}

uint32_t laCircularSliderWidget_GetArcRadius(laCircularSliderWidget* slider,
                                             laCircularSliderWidgetArcType type)
{
    if(slider == NULL)
        return 0;

    switch(type)
    {
        case CIRCLE_BUTTON:
            return slider->circleButtonArc.radius;
        default:
            break;
    }
        
    return 0;
}

laResult laCircularSliderWidget_SetArcRadius(laCircularSliderWidget* slider,
                                             laCircularSliderWidgetArcType type,
                                             uint32_t radius)
{
    if(slider == NULL)
        return LA_FAILURE;

    switch(type)
    {
        case CIRCLE_BUTTON:
        {
            if (slider->circleButtonArc.radius == radius)
                return LA_SUCCESS;
            
            if (radius > slider->radius)
                return LA_FAILURE;

            slider->circleButtonArc.radius = radius;

            break;
        }
        default:
            return LA_FAILURE;
    }
        
    laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}

laScheme * laCircularSliderWidget_GetArcScheme(laCircularSliderWidget* slider,
                                               laCircularSliderWidgetArcType type)
{
    if(slider == NULL)
        return NULL;

    switch(type)
    {
        case CIRCLE_BUTTON:
            return slider->circleButtonArc.scheme;
        default:
            return slider->widget.scheme;
            break;
    }
        
    return NULL;
}

laResult laCircularSliderWidget_SetArcScheme(laCircularSliderWidget* slider,
                                             laCircularSliderWidgetArcType type,
                                             laScheme * scheme)
{
    if(slider == NULL)
        return LA_FAILURE;

    switch(type)
    {
        //Only the circle button has a different scheme from parent widget
        case CIRCLE_BUTTON:
            slider->circleButtonArc.scheme = scheme;
            break;
        default:
            return LA_FAILURE;
    }
        
    laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}

laBool laCircularSliderWidget_GetArcVisible(laCircularSliderWidget* slider,
                                               laCircularSliderWidgetArcType type)
{
    if(slider == NULL)
        return LA_FALSE;

    switch(type)
    {
        case OUTSIDE_CIRCLE_BORDER:
            return slider->outsideBorderArc.visible;
        case INSIDE_CIRCLE_BORDER:
            return slider->insideBorderArc.visible;
        case ACTIVE_AREA:
            return slider->activeArc.visible;
        case INACTIVE_AREA:
            return slider->inActiveArc.visible;
        case CIRCLE_BUTTON:
            return slider->circleButtonArc.visible;
        default:
            break;
    }
        
    return LA_FALSE;
}

laResult laCircularSliderWidget_SetArcVisible(laCircularSliderWidget* slider,
                                             laCircularSliderWidgetArcType type,
                                             laBool visible)
{
    if(slider == NULL)
        return LA_FAILURE;

    switch(type)
    {
        case OUTSIDE_CIRCLE_BORDER:
        {
            if (slider->outsideBorderArc.visible == visible)
                return LA_SUCCESS;

            slider->outsideBorderArc.visible = visible;
            break;
        }
        case INSIDE_CIRCLE_BORDER:
        {
            if (slider->insideBorderArc.visible == visible)
                return LA_SUCCESS;

            slider->insideBorderArc.visible = visible;
            break;
        }
        case ACTIVE_AREA:
        {
            if (slider->activeArc.visible == visible)
                return LA_SUCCESS;

            slider->activeArc.visible = visible;
            break;
        }
        case INACTIVE_AREA:
        {
            if (slider->inActiveArc.visible == visible)
                return LA_SUCCESS;

            slider->inActiveArc.visible = visible;
            break;
        }
        case CIRCLE_BUTTON:
        {
            if (slider->circleButtonArc.visible == visible)
                return LA_SUCCESS;

            slider->circleButtonArc.visible = visible;
            break;
        }
        default:
            return LA_FAILURE;
    }
        
    laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}

laCircularSliderWidgetDir laCircularSliderWidget_GetDirection(laCircularSliderWidget* slider)
{
    if(slider == NULL)
        return 0;
        
    return slider->direction;
}

laResult laCircularSliderWidget_SetDirection(laCircularSliderWidget* slider, laCircularSliderWidgetDir dir)
{
    if(slider == NULL)
        return LA_FAILURE;
        
    if(slider->direction == dir)
        return LA_SUCCESS;
        
    slider->direction = dir;
    
    laWidget_Invalidate((laWidget*)slider);
        
    return LA_SUCCESS;
}

laResult laCircularSliderWidget_SetPressedEventCallback(laCircularSliderWidget* slider, laCircularSliderWidget_PressedEvent cb)
{
    if (cb == NULL)
        return LA_FAILURE;

    slider->pressedCallback = cb;
        
    return LA_SUCCESS;
}

laResult laCircularSliderWidget_SetValueChangedEventCallback(laCircularSliderWidget* slider, laCircularSliderWidget_ValueChangedEvent cb)
{
    if (cb == NULL)
        return LA_FAILURE;

    slider->valueChangedCallback = cb;
        
    return LA_SUCCESS;
}

laResult laCircularSliderWidget_SetReleasedEventCallback(laCircularSliderWidget* slider, laCircularSliderWidget_ReleasedEvent cb)
{
    if (cb == NULL)
        return LA_FAILURE;
        
    slider->releasedCallback = cb;

    return LA_SUCCESS;
}

static uint32_t getSliderValueAtPoint(laCircularSliderWidget* slider, GFX_Point pnt)
{
    uint32_t angle;
    uint32_t value;
    
    if (pnt.x > 0 && pnt.y > 0)
    {
        //Q1
        angle = (int32_t) ((double) GFX_Atan((double) pnt.y / pnt.x) * (double)(180 / 3.1416));
    }
    else if (pnt.x < 0 && pnt.y > 0)
    {
        //Q2
        angle = (int32_t) ((double) GFX_Atan((double) pnt.y / pnt.x) * (double)(180 / 3.1416));
        angle = 180 + angle;
    }
    else if (pnt.x > 0 && pnt.y < 0)
    {
        //Q4
        angle = (int32_t) ((double) GFX_Atan((double) pnt.y / pnt.x) * (double)(180 / 3.1416));
        angle = 360 + angle;
    }
    else if (pnt.x < 0 && pnt.y < 0)
    {
        //Q3
        angle = (int32_t) ((double) GFX_Atan((double) pnt.y / pnt.x) * (double)(180 / 3.1416));
        angle = 180 + angle;
    }
    else if (pnt.x == 0 && pnt.y >= 0)
    {
        // +y
        angle = 90;
    }
    else if (pnt.x == 0 && pnt.y < 0)
    {
        // -y
        angle = 270;
    }
    else if (pnt.y == 0 && pnt.x > 0)
    {
        // +x
        angle = 0;
    }
    else
    {
        // -x
        angle = 180;
    }

    if (slider->direction == CIRCULAR_SLIDER_DIR_COUNTER_CLOCKWISE)
    {
        if (angle > slider->startAngle)
        {
            value = slider->startValue + (slider->endValue - slider->startValue) * (angle - slider->startAngle)/ 360;
        }
        else //wrapped around
        {
            value = slider->startValue + (slider->endValue - slider->startValue) * (360 - slider->startAngle)/ 360 +
                    (slider->endValue - slider->startValue) * angle/360;
        }
    }
    else
    {
        if (angle < slider->startAngle)
        {
            value = slider->startValue + (slider->endValue - slider->startValue) * (slider->startAngle - angle) / 360;
        }
        else
        {
            value = slider->startValue + (slider->endValue - slider->startValue) * slider->startAngle/360 +
                    (slider->endValue - slider->startValue) * (360 - angle)/360;
        }
    }
    
    return value;
    
}

static void touchDown(laCircularSliderWidget* slider, laInput_TouchDownEvent* evt)
{
    GFX_Point pnt;

    pnt.x = evt->x;
    pnt.y = evt->y;

    //If circular button is visible, set button state touch is within circle
    if (slider->circleButtonArc.visible == LA_TRUE)
    {
        laUtils_PointScreenToLocalSpace((laWidget*)slider, &pnt);
        
        if (slider->buttonTouch == LA_TRUE)
        {
            GFX_Point buttonCenter = _laCircularSliderWidget_GetCircleCenterPointAtValue(slider, slider->value);

            laUtils_PointScreenToLocalSpace((laWidget*)slider, &buttonCenter);

            //Measure distance of point from center and see if it is within radius
            if ((uint32_t)(((pnt.x - buttonCenter.x) * (pnt.x - buttonCenter.x)) + 
                ((pnt.y - buttonCenter.y) * (pnt.y - buttonCenter.y))) <= 
                (slider->circleButtonArc.radius * slider->circleButtonArc.radius))
            {

                slider->btnState = LA_CIRCULAR_SLIDER_STATE_DOWN;

                if (slider->pressedCallback != NULL)
                {
                    slider->pressedCallback(slider, slider->value);
                }

                evt->event.accepted = LA_TRUE;
            }
            else
            {
                slider->btnState = LA_CIRCULAR_SLIDER_STATE_UP;
            }
        }
        else
        {
            GFX_Point pntLocal;
            uint32_t ptRadSqd;
            uint32_t value;
            
            uint32_t touchOutRad = slider->radius - 
                                   (slider->activeArc.thickness/2) + 
                                   slider->circleButtonArc.radius;
            
            uint32_t touchInRad = slider->radius - 
                                   (slider->activeArc.thickness/2) -
                                   slider->circleButtonArc.radius;
            
            pntLocal.x = pnt.x - slider->widget.rect.width/2;
            pntLocal.y = slider->widget.rect.height/2 - pnt.y;
            
            ptRadSqd = (pntLocal.x) * (pntLocal.x) + 
                       (pntLocal.y) * (pntLocal.y);

            //Measure distance of point from center and see if it is within radius            
            if ((ptRadSqd <= (touchOutRad * touchOutRad)) &&
                (ptRadSqd >= (touchInRad * touchInRad)))
            {

                slider->btnState = LA_CIRCULAR_SLIDER_STATE_DOWN;

                if (slider->pressedCallback != NULL)
                {
                    slider->pressedCallback(slider, slider->value);
                }

                evt->event.accepted = LA_TRUE;
                
                value = getSliderValueAtPoint(slider, pntLocal);
                
                laCircularSliderWidget_SetValue(slider, value);
            }
            else
            {
                slider->btnState = LA_CIRCULAR_SLIDER_STATE_UP;
            }
        }
    }
}

static void touchUp(laCircularSliderWidget* slider, laInput_TouchUpEvent* evt)
{
    if (slider->btnState == LA_CIRCULAR_SLIDER_STATE_DOWN)
    {
        evt->event.accepted = LA_TRUE;
        
        slider->btnState = LA_CIRCULAR_SLIDER_STATE_UP;
    
        if (slider->releasedCallback != NULL)
        {
            slider->releasedCallback(slider, slider->value);
        }
    }
}

static void touchMoved(laCircularSliderWidget* slider, laInput_TouchMovedEvent* evt)
{
//    uint32_t touchPntRadSqd;
    GFX_Point pnt;
    uint32_t value;
    uint32_t snapThreshold;

    pnt.x = evt->x;
    pnt.y = evt->y;

    snapThreshold = (slider->endValue > slider->startValue) ? 
                     (slider->endValue - slider->startValue)/4 : 
                     (slider->startValue - slider->endValue)/4;
    
    if (slider->circleButtonArc.visible == LA_TRUE && 
        slider->btnState == LA_CIRCULAR_SLIDER_STATE_DOWN)
    {
        evt->event.accepted = LA_TRUE;
    
        //Adjust point relative to widget local
        laUtils_PointScreenToLocalSpace((laWidget*)slider, &pnt);

        //Adjust point relative widget center/origin
        pnt.x -= slider->widget.rect.width/2; 
        pnt.y = slider->widget.rect.height/2 - pnt.y;
    
//        touchPntRadSqd = pnt.x * pnt.x + pnt.y * pnt.y;

        //Limit move point to within the touch button only
//        if ((touchPntRadSqd > ((slider->activeArc.radius + (slider->activeArc.thickness/2) + slider->circleButtonArc.radius) * 
//                               (slider->activeArc.radius + (slider->activeArc.thickness/2) + slider->circleButtonArc.radius))) ||
//            (touchPntRadSqd < 
//             ((slider->activeArc.radius - (slider->activeArc.thickness/2) - slider->circleButtonArc.radius) * 
//              (slider->activeArc.radius - (slider->activeArc.thickness/2) - slider->circleButtonArc.radius))))
//        {
//            //slider->btnState = LA_CIRCULAR_SLIDER_STATE_UP;
//            return;
//        }

        value = getSliderValueAtPoint(slider, pnt);

        if (slider->sticky == LA_TRUE)
        {
            if ((slider->endValue - slider->value) < snapThreshold)
            {
                if ((value - slider->startValue) < snapThreshold)
                    value = slider->endValue;
            }
            
            else if ((slider->value - slider->startValue) < snapThreshold)
            {
                if ((slider->endValue - value) < snapThreshold)
                    value = slider->startValue;
            }
        }

        laCircularSliderWidget_SetValue(slider, value);
    }
}

#endif // LA_CIRCULAR_SLIDER_WIDGET_ENABLED
