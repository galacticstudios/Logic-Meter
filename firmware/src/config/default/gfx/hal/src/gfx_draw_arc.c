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


#include "gfx/hal/inc/gfx_draw.h"

#if GFX_DRAW_PIPELINE_ENABLED

#include <math.h>

#include "gfx/hal/inc/gfx_context.h"
#include "gfx/hal/inc/gfx_layer.h"
#include "gfx/hal/inc/gfx_rect.h"
#include "gfx/hal/inc/gfx_util.h"
#include "gfx/hal/inc/gfx_math.h"

//#define ARC_SMOOTH_EDGE 1
#define USE_ARC_SCAN_FILL 1

typedef enum 
{
    ON_LINE,
    LEFT_OF_LINE,
    RIGHT_OF_LINE,
} POINT_LINE_POS;

static GFX_Result rejectArc(int32_t x,
                               int32_t y,
                               int32_t radius,
                               const GFX_DrawState* state)
{
    GFX_Rect rect;

#if GFX_ALPHA_BLENDING_ENABLED
    // alpha is zero
    if((state->blendMode & GFX_BLEND_GLOBAL) > 0 &&
        state->globalAlphaValue == 0)
       return GFX_FAILURE;
#endif
    
    rect.x = x - radius;
    rect.y = y - radius;
    rect.height = rect.width = 2 * radius;    

#if GFX_LAYER_CLIPPING_ENABLED
    // clip rect against layer rect
    if(GFX_RectIntersects(&state->targetClipRect, &rect) == GFX_FALSE)
        return GFX_FAILURE;
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
    // clip rect against global clipping rect
    if((state->clipEnable == GFX_TRUE) && 
        GFX_RectIntersects(&state->clipRect, &rect) == GFX_FALSE)
        return GFX_FAILURE;
#endif

    return GFX_SUCCESS;
}

GFX_Result GFX_DrawArc(int32_t x,
                       int32_t y,
                       int32_t radius,
                       int32_t startAngle,
                       int32_t centerAngle)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Point pt;
    GFX_Result res;
    GFX_DrawState state;
   
    GFX_ASSERT(context != NULL);

    if(context == NULL)
        return GFX_FAILURE;
        
    state = context->draw;
        
    if(state.target == NULL)
    {
        if(context->layer.active == NULL ||
           context->layer.active->locked == GFX_FALSE)
            return GFX_FAILURE;
            
        state.target = &context->layer.active->buffers[context->layer.active->buffer_write_idx].pb;
        state.targetClipRect = context->layer.active->rect.local;
    }
    
    state.colorMode = state.target->mode;
    
    pt.x = x;
    pt.y = y;
    
    if(rejectArc(x, y, radius, &state) == GFX_FAILURE)
        return GFX_FAILURE;
    
    res = state.pipeline->drawArc[state.mode]
                                 [state.antialias]
                                 (&pt, radius, startAngle, centerAngle, &state);

    return res;
}

GFX_Result cpuDrawArc_Line(const GFX_Point* pt,
                           int32_t r,
                           int32_t startAngle,
                           int32_t centerAngle,
                           const GFX_DrawState* state)
{
    int32_t sa = startAngle;
    GFX_Point p;

    while (sa != (startAngle + startAngle))
    {
        GFX_PolarToXY(r, sa, &p);
        
        p.x += pt->x;
        p.y += pt->y;
        state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&p, state);
        
        //Clockwise
        if (centerAngle < 0)
        {
            sa--;
        }
        //Counter-clockwise
        else
        {
            sa++;
        }
    }

    return GFX_SUCCESS;
}

// This function returns the relative horizontal position of a point from a line.
// Works only if test point and line are in the same half plane (Q1 & Q2, or Q3 & Q4).
POINT_LINE_POS pointRelPositionFromLine(GFX_Point * linePt0, GFX_Point * linePt1,
                                        GFX_Point * point)
{
    int sign = (linePt1->x - linePt0->x) * (point->y - linePt0->y) - 
            (linePt1->y - linePt0->y) * (point->x - linePt0->x);

    if (linePt1->y < 0)
        sign = -sign;

    if (sign > 0)
    {
        return LEFT_OF_LINE;
    }
    else if (sign < 0)
    {
        return RIGHT_OF_LINE;
    }

    return ON_LINE;
}

//Returns true if arc overlaps with quadrant
GFX_Bool arcsOverlapQuadrant(int32_t startAngleArc0, 
                             int32_t endAngleArc0, 
                             ARC_DIR dir, 
                             uint32_t quadrant)
{
    uint32_t baseAngle;
    int32_t angle;
    GFX_Bool retval = GFX_FALSE;
    int32_t quadStartAngle, quadEndAngle;
    int32_t startTestAngle, endTestAngle;

    switch (quadrant)
    {
        case Q1:
            quadStartAngle = 0;
            quadEndAngle = 90;
            break;
        case Q2:
            quadStartAngle = 91;
            quadEndAngle = 180;
            break;
        case Q3:
            quadStartAngle = 181;
            quadEndAngle = 270;
            break;
        case Q4:
            quadStartAngle = 271;
            quadEndAngle = 360;
            break;
        default:
            return GFX_FALSE;
    }

    //Re-orient angles to a CCW orientation
    if (dir == CW)
    {
        angle = startAngleArc0;
        startAngleArc0 = endAngleArc0;
        endAngleArc0 = angle;
    }

    //if arc wraps around 0/360, do triple arc test
    if (startAngleArc0 > endAngleArc0)
    {
        //split between (start -> 360) and (0 -> end)
        startTestAngle = startAngleArc0;
        endTestAngle = 360;

        if (startTestAngle <= quadStartAngle)
            baseAngle = 0;
        else
            baseAngle = 1;

        if (baseAngle == 0 && (endTestAngle >= quadStartAngle))
        {
            retval = GFX_TRUE;
        }
        else if (baseAngle == 1 && (quadEndAngle >= startTestAngle))
        {
            retval = GFX_TRUE;
        }
        else
        {
            //split between (start -> 360) and (0 -> end)
            startTestAngle = 0;
            endTestAngle = endAngleArc0;

            if (startTestAngle <= quadStartAngle) 
                baseAngle = 0;
            else
                baseAngle = 1;

            if (baseAngle == 0 && (endTestAngle >= quadStartAngle))
            {
                retval = GFX_TRUE;
            }
            else if (baseAngle == 1 && (quadEndAngle >= startTestAngle))
            {
                retval = GFX_TRUE;
            }
        }
    }
    else if (startAngleArc0 < endAngleArc0)
    {
        if (startAngleArc0 <= quadStartAngle)
            baseAngle = 0;
        else
            baseAngle = 1;

        if (baseAngle == 0 && (endAngleArc0 >= quadStartAngle))
        {
            retval = GFX_TRUE;
        }
        else if (baseAngle == 1 && (quadEndAngle >= startAngleArc0))
        {
            retval = GFX_TRUE;
        }
    }
    else //circle
        retval = GFX_TRUE;

    return retval;
}

GFX_Color getArcSoftEdgeColor (uint32_t oRadSqd, 
                               uint32_t iRadSqd, 
                               uint32_t thicknessSqd,
                               uint32_t ptRadSqd, 
                               const GFX_DrawState * drawState)
{
    GFX_Color color;

    //test with outer radius
    if ((oRadSqd - ptRadSqd) < 8)
    {
        color = GFX_ColorLerp(drawState->gradient.c0,
                                           drawState->gradient.c1,
                                           20,
                                           drawState->colorMode);
    }
    else if ((oRadSqd - ptRadSqd) < 32)
    {
        color = GFX_ColorLerp(drawState->gradient.c0,
                                           drawState->gradient.c1,
                                           40,
                                           drawState->colorMode);
    }
    else if ((oRadSqd - ptRadSqd) < 50)
    {
        color = GFX_ColorLerp(drawState->gradient.c0,
                                           drawState->gradient.c1,
                                           60,
                                           drawState->colorMode);
    }    
    else if ((oRadSqd - ptRadSqd) < 72)
    {
        color = GFX_ColorLerp(drawState->gradient.c0,
                                           drawState->gradient.c1,
                                           80,
                                           drawState->colorMode);
    }
    //Do not test inner edge if full circle
    else if (thicknessSqd == oRadSqd)
    {
        color = drawState->gradient.c1;
    }
    else if ((ptRadSqd - iRadSqd) < 8)
    {
        color = GFX_ColorLerp(drawState->gradient.c0,
                                           drawState->gradient.c1,
                                           20,
                                           drawState->colorMode);
    }
    else if ((ptRadSqd - iRadSqd) < 32)
    {
        color = GFX_ColorLerp(drawState->gradient.c0,
                                           drawState->gradient.c1,
                                           40,
                                           drawState->colorMode);
    }
    else if ((ptRadSqd - iRadSqd) < 50)
    {
        color = GFX_ColorLerp(drawState->gradient.c0,
                                           drawState->gradient.c1,
                                           60,
                                           drawState->colorMode);
    }    
    else if ((ptRadSqd - iRadSqd) < 72)
    {
        color = GFX_ColorLerp(drawState->gradient.c0,
                                           drawState->gradient.c1,
                                           80,
                                           drawState->colorMode);
    }
    else
    {
        color = drawState->gradient.c1;
    }

    return color;
}


#ifdef USE_ARC_SCAN_FILL
//This function does a scan fill from +y -> -y, -x -> +x or
// Q2, Q1, Q3, Q4 in terms of quadrants. Points outside the 
// arc fill area are discarded.
GFX_Result cpuDrawArc_Fill(const GFX_Point* pt,
                           uint32_t r,
                           int32_t startAngle,
                           int32_t centerAngle,
                           const GFX_DrawState* state)
{
    GFX_Point topRectPt, botRectPt; //Top and bottom points of arc rectangle
    GFX_Point arcStartPt0, arcStartPt1, arcEndPt0, arcEndPt1; //Points of arc edges
    GFX_Point drawPt; //Point to draw
    GFX_Point scanPt;
    int32_t endAngle;
    uint32_t ptRadiusSqrd, oCirRadSqrd, iCirRadSqrd; //Square of radii
    uint32_t absStartAngle, absEndAngle; //start and end angles in positive angles
    ARC_DIR dir;
    GFX_Color drawColor;
    GFX_DrawState arcDrawState;
    int32_t thickness;
    uint32_t thicknessSqd;

#ifdef ARC_SMOOTH_EDGE    
    uint32_t softEdgeThickness;
#endif
    
    thickness = (state->thickness < r) ? state->thickness : r;
    
    thicknessSqd = thickness * thickness;

    //Don't care about wrapping 
    if (centerAngle == 0)
        return GFX_SUCCESS;

    startAngle %= 360;
    centerAngle %= 360;

    arcDrawState = *state;
    drawColor = arcDrawState.color;
    
#ifdef ARC_SMOOTH_EDGE
    arcDrawState.antialias = GFX_TRUE;
    softEdgeThickness = 36;
#else
    arcDrawState.antialias = GFX_FALSE;
#endif    

    //Determine points of arc edges
    if (startAngle >= 0)
        absStartAngle = startAngle;
    else
        absStartAngle = 360 + startAngle;

    GFX_PolarToXY(r, absStartAngle, &arcStartPt1);
    GFX_PolarToXY(r - thickness, absStartAngle, &arcStartPt0);

    endAngle = startAngle + centerAngle;
    if (endAngle >= 0)
        absEndAngle = endAngle;
    else
        absEndAngle = 360 + endAngle;
    
    absEndAngle %= 360;

    GFX_PolarToXY(r, absEndAngle, &arcEndPt1);
    GFX_PolarToXY(r - thickness, absEndAngle, &arcEndPt0);

    if (centerAngle > 0)
    {
        dir  = CCW;
    }
    else
    {
        dir = CW;
    }

    //Determine bounding rectangle points
    topRectPt.x = - ((int32_t)r);
    topRectPt.y = r;
    botRectPt.x = r;
    botRectPt.y = -((int32_t)r);

    oCirRadSqrd = r * r;
    iCirRadSqrd = (r - thickness) * (r - thickness);

    //Scan thru the points in arc rectangle, per quadrant and filter points that are outside
    //Only scan if the arc overlaps with the quadrant
    if (arcsOverlapQuadrant(absStartAngle, 
                            absEndAngle, 
                            dir, 
                            Q2) == GFX_TRUE)
    {
        for (scanPt.y = topRectPt.y; scanPt.y >= 0; scanPt.y--)
        {
            arcDrawState.color = drawColor;

            for (scanPt.x = topRectPt.x; scanPt.x <= 0; scanPt.x++)
            {
                ptRadiusSqrd = (scanPt.x * scanPt.x) + (scanPt.y * scanPt.y);
                
                //If point is outside outer circle, skip
                if (ptRadiusSqrd > oCirRadSqrd)
                    continue;
                
                //If point is outside inner circle, done scanning x
                if (ptRadiusSqrd < iCirRadSqrd)
                    break;

                if (absStartAngle == absEndAngle)
                {
                    //Circle, do not filter, draw all points
                }
                else if ((absStartAngle <= 180 && absStartAngle > 90) && 
                   (absEndAngle <= 180 && absEndAngle > 90))
                {
                    //If both start and end angles are in Q2
                    //If CCW, exclude points RIGHT of start line and LEFT of end line
                    if (dir == CCW) 
                    {
                        if (absStartAngle < absEndAngle)
                        {
                            //Include points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != RIGHT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) != LEFT_OF_LINE))
                            {
                                //Do nothing
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else if (absStartAngle > absEndAngle)
                        {
                            //exclude points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == RIGHT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == LEFT_OF_LINE))
                                continue;                            
                        }
                        else
                        {
                            if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != ON_LINE)
                                continue;
                        }
                    }
                    //If CW, exclude points LEFT of start line and RIGHT of end line
                    else
                    {
                        if (absStartAngle > absEndAngle)
                        {
                            //Include points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != LEFT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) != RIGHT_OF_LINE))
                            {
                                //Do nothing
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else if (absStartAngle < absEndAngle)
                        {
                            //exclude points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == LEFT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == RIGHT_OF_LINE))
                                continue;                            
                        }
                        else
                        {
                            if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != ON_LINE)
                                continue;
                        }
                    }
                }
                else if (absStartAngle <= 180 && absStartAngle > 90)
                {
                    //If CCW, exclude points RIGHT of start line
                    if (dir == CCW)
                    {
                        if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == RIGHT_OF_LINE)
                            continue;
                    }
                    //If CW, exclude points LEFT of start line
                    else
                    {
                        if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == LEFT_OF_LINE)
                            continue;
                    }
                }
                //If end angle is in Q2, test agains Q2
                else if (absEndAngle <= 180 && absEndAngle > 90)
                {
                    //If CCW, exclude points LEFT of start line
                    if (dir == CCW)
                    {
                        if (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == LEFT_OF_LINE)
                            continue;
                    }
                    //If CCW, exclude points RIGHT of start line
                    else
                    {
                        if (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == RIGHT_OF_LINE)
                            continue;
                    }
                }

                //Soften edge colors if anti-aliased
                if (arcDrawState.antialias == GFX_TRUE)
                {
                    arcDrawState.color = getArcSoftEdgeColor(oCirRadSqrd,
                                                             iCirRadSqrd,
                                                             thicknessSqd,
                                                             ptRadiusSqrd,
                                                             &arcDrawState);
                     
                }
                
                drawPt.x = pt->x + scanPt.x;
                drawPt.y = pt->y - scanPt.y;

                if (GFX_RectContainsPoint(&arcDrawState.clipRect, &drawPt) == GFX_TRUE)
                {
                    state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&drawPt, &arcDrawState);
                }
            }
        }
    }

    //Don't scan thru Q1 points if not needed. Optimize?
    if (arcsOverlapQuadrant(absStartAngle, 
                            absEndAngle, 
                            dir, 
                            Q1) == GFX_TRUE)
    {
        for (scanPt.y = topRectPt.y; scanPt.y >= 0; scanPt.y--)
        {
            arcDrawState.color = drawColor;

            for (scanPt.x = 0; scanPt.x < botRectPt.x; scanPt.x++)
            {
                ptRadiusSqrd = (scanPt.x * scanPt.x) + (scanPt.y * scanPt.y);
                
                //If point is outside outer circle, skip
                if (ptRadiusSqrd < iCirRadSqrd)
                    continue;

                //If point is outside outer circle, done scanning x
                if (ptRadiusSqrd > oCirRadSqrd)
                    break;
                
                if (absStartAngle == absEndAngle)
                {
                    //Circle, do not filter, draw all points
                }
                else if ((absStartAngle <= 90 && absStartAngle >= 0) && 
                   (absEndAngle <= 90 && absEndAngle >= 0))
                {
                    //If both start and end angles are in Q1
                    //If CCW, exclude points RIGHT of start line and LEFT of end line
                    if (dir == CCW) 
                    {
                        if (absStartAngle < absEndAngle)
                        {
                            //Include points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != RIGHT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) != LEFT_OF_LINE))
                            {
                                //Do nothing
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else if (absStartAngle > absEndAngle)
                        {
                            //exclude points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == RIGHT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == LEFT_OF_LINE))
                                continue;                            
                        }
                        else
                        {
                            if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != ON_LINE)
                                continue;
                        }
                    }
                    else
                    {
                        if (absStartAngle > absEndAngle)
                        {
                            //Include points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != LEFT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) != RIGHT_OF_LINE))
                            {
                                //Do nothing
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else if (absStartAngle < absEndAngle)
                        {
                            //exclude points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == LEFT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == RIGHT_OF_LINE))
                                continue;                            
                        }
                        else
                        {
                            if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != ON_LINE)
                                continue;
                        }
                    }
                }
                else if (absStartAngle <= 90 && absStartAngle >= 0)
                {
                    //If CCW, exclude points RIGHT of start line
                    if (dir == CCW)
                    {
                        if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == RIGHT_OF_LINE)
                            continue;
                    }
                    //If CW, exclude points LEFT of start line
                    else
                    {
                        if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == LEFT_OF_LINE)
                            continue;
                    }
                }
                //If end angle is in Q1, test agains Q1
                else if (absEndAngle <= 90 && absEndAngle >= 0)
                {
                    //If CCW, exclude points LEFT of start line
                    if (dir == CCW)
                    {
                        if (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == LEFT_OF_LINE)
                            continue;
                    }
                    //If CW, exclude points RIGHT of start line
                    else
                    {
                        if (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == RIGHT_OF_LINE)
                            continue;
                    }
                }

                //Soften edge
                if (arcDrawState.antialias == GFX_TRUE)
                {
                    arcDrawState.color = getArcSoftEdgeColor(oCirRadSqrd,
                                                             iCirRadSqrd,
                                                             thicknessSqd,
                                                             ptRadiusSqrd,
                                                             &arcDrawState);
                }

                drawPt.x = pt->x + scanPt.x;
                drawPt.y = pt->y - scanPt.y;

                if (GFX_RectContainsPoint(&arcDrawState.clipRect, &drawPt) == GFX_TRUE)
                {
                    state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&drawPt, &arcDrawState);
                }
            }
        }
    }

    //Don't scan thru Q3 points if not needed. Optimize?
    if (arcsOverlapQuadrant(absStartAngle, 
                            absEndAngle, 
                            dir, 
                            Q3) == GFX_TRUE)
    {
        for (scanPt.y = 0; scanPt.y > botRectPt.y; scanPt.y--)
        {
            arcDrawState.color = drawColor;

            for (scanPt.x = topRectPt.x; scanPt.x <= 0; scanPt.x++)
            {
                ptRadiusSqrd = (scanPt.x * scanPt.x) + (scanPt.y * scanPt.y);
                
                //If point is outside outer circle, skip
                if (ptRadiusSqrd > oCirRadSqrd)
                    continue;
                
                //If point is outside inner circle, done scanning x
                if (ptRadiusSqrd < iCirRadSqrd)
                    break;

                if (absStartAngle == absEndAngle)
                {
                    //Circle, do not filter, draw all points
                }
                else if ((absStartAngle <= 270 && absStartAngle > 180) && 
                   (absEndAngle <= 270 && absEndAngle > 180))
                {
                    //If CCW, exclude points RIGHT of start line and LEFT of end line
                    if (dir == CCW) 
                    {
                        if (absStartAngle < absEndAngle)
                        {
                            //Include points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != LEFT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) != RIGHT_OF_LINE))
                            {
                                //Do nothing
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else if (absStartAngle > absEndAngle)
                        {
                            //exclude points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == LEFT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == RIGHT_OF_LINE))
                                continue;                            
                        }
                        else
                        {
                            if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != ON_LINE)
                                continue;
                        }
                    }
                    else
                    {
                        if (absStartAngle > absEndAngle)
                        {
                            //Include points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != RIGHT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) != LEFT_OF_LINE))
                            {
                                //Do nothing
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else if (absStartAngle < absEndAngle)
                        {
                            //exclude points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == RIGHT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == LEFT_OF_LINE))
                                continue;                            
                        }
                        else
                        {
                            if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != ON_LINE)
                                continue;
                        }
                    }
                }
                else if (absStartAngle <= 270 && absStartAngle > 180)
                {
                    //If CCW, exclude points LEFT of start line
                    if (dir == CCW)
                    {
                        if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == LEFT_OF_LINE)
                            continue;
                    }
                    //If CW, exclude points RIGHT of start line
                    else
                    {
                        if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == RIGHT_OF_LINE)
                            continue;
                    }
                }
                else if (absEndAngle <= 270 && absEndAngle > 180)
                {
                    //If CCW, exclude points RIGHT of end line
                    if (dir == CCW)
                    {
                        if (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == RIGHT_OF_LINE)
                            continue;
                    }
                    //If CW, exclude points LEFT of end line
                    else
                    {
                                                //corner case: if radius is too small, and end angle is close to 180, 
                        //the end line approximates to a horizontal line. in this case, 
                        //just draw all points in the quadrant
                        if (arcEndPt0.y == 0 && arcEndPt1.y == 0)
                        {
                            //draw all points
                        }
                        else if (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == LEFT_OF_LINE)
                            continue;
                    }
                }

                //Soften edge
                if (arcDrawState.antialias == GFX_TRUE)
                {
                    arcDrawState.color = getArcSoftEdgeColor(oCirRadSqrd,
                                                             iCirRadSqrd,
                                                             thicknessSqd,
                                                             ptRadiusSqrd,
                                                             &arcDrawState);
                }
                
                drawPt.x = pt->x + scanPt.x;
                drawPt.y = pt->y - scanPt.y;

                if (GFX_RectContainsPoint(&arcDrawState.clipRect, &drawPt) == GFX_TRUE)
                {
                    state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&drawPt, &arcDrawState);
                }
            }
        }
    }

    //Don't scan thru Q4 points if not needed. Optimize?
    if (arcsOverlapQuadrant(absStartAngle, 
                            absEndAngle, 
                            dir, 
                            Q4) == GFX_TRUE)
    {
        for (scanPt.y = 0; scanPt.y > botRectPt.y; scanPt.y--)
        {
            arcDrawState.color = drawColor;

            for (scanPt.x = 0; scanPt.x < botRectPt.x; scanPt.x++)
            {
                ptRadiusSqrd = (scanPt.x * scanPt.x) + (scanPt.y * scanPt.y);
                
                //If point is outside outer circle, skip
                if (ptRadiusSqrd < iCirRadSqrd)
                    continue;

                //If point is outside outer circle, done scanning x
                if (ptRadiusSqrd > oCirRadSqrd)
                    break;

                if (absStartAngle == absEndAngle)
                {
                    //Circle or almost a circle, do not filter, draw all points
                }
                else if ((absStartAngle < 360 && absStartAngle > 270) && 
                   (absEndAngle < 360 && absEndAngle > 270))
                {
                    //If CCW, exclude points RIGHT of start line and LEFT of end line
                    if (dir == CCW) 
                    {
                        if (absStartAngle < absEndAngle)
                        {
                            //Include points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != LEFT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) != RIGHT_OF_LINE))
                            {
                                //Do nothing
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else if (absStartAngle > absEndAngle)
                        {
                            //exclude points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == LEFT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == RIGHT_OF_LINE))
                                continue;                            
                        }
                        else
                        {
                            if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != ON_LINE)
                                continue;
                        }
                    }
                    else
                    {
                        if (absStartAngle > absEndAngle)
                        {
                            //Include points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != RIGHT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) != LEFT_OF_LINE))
                            {
                                //Do nothing
                            }
                            else
                            {
                                continue;
                            }
                        }
                        else if (absStartAngle < absEndAngle)
                        {
                            //exclude points between the two angles
                            if ((pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == RIGHT_OF_LINE)
                                && (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == LEFT_OF_LINE))
                                continue;                            
                        }
                        else
                        {
                            if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) != ON_LINE)
                                continue;
                        }
                    }
                }
                else if (absStartAngle < 360 && absStartAngle > 270)
                {
                    //If CCW, exclude points LEFT of start line
                    if (dir == CCW)
                    {
                        if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == LEFT_OF_LINE)
                            continue;
                    }
                    //If CW, exclude points RIGHT of start line
                    else
                    {
                        if (pointRelPositionFromLine(&arcStartPt0, &arcStartPt1, &scanPt) == RIGHT_OF_LINE)
                            continue;
                    }
                }
                else if (absEndAngle < 360 && absEndAngle > 270)
                {
                    //If CCW, exclude points RIGHT of end line
                    if (dir == CCW)
                    {
                        //corner case: if radius is too small, and end angle is close to 360, 
                        //the end line approximates to a horizontal line. in this case, 
                        //just draw all points in the quadrant
                        if (arcEndPt0.y == 0 && arcEndPt1.y == 0)
                        {
                            //draw all points
                        }
                        else if (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == RIGHT_OF_LINE)
                            continue;
                    }
                    //If CW, exclude points LEFT of end line
                    else
                    {
                        if (pointRelPositionFromLine(&arcEndPt0, &arcEndPt1, &scanPt) == LEFT_OF_LINE)
                            continue;
                    }
                }
                
                //Soften edge
                if (arcDrawState.antialias == GFX_TRUE)
                {
                    arcDrawState.color = getArcSoftEdgeColor(oCirRadSqrd,
                                                             iCirRadSqrd,
                                                             thicknessSqd,
                                                             ptRadiusSqrd,
                                                             &arcDrawState);
                }

                drawPt.x = pt->x + scanPt.x;
                drawPt.y = pt->y - scanPt.y;

                if (GFX_RectContainsPoint(&arcDrawState.clipRect, &drawPt) == GFX_TRUE)
                {
                    state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&drawPt, &arcDrawState);
                }
            }
        }
    }

    return GFX_SUCCESS;
}

#else

static int32_t circleGetXGivenY(int32_t r, int32_t y)
{
    if (-r < y && y < r)
        return (int32_t) sqrt(r*r - y*y);

    return 0;
}

//Draws horizontal lines to fill arc
static void drawArcFillLine(const GFX_Point * pt, 
                            const GFX_Point * drawPtStart,
                            const GFX_Point * drawPtEnd,
                            const GFX_DrawState* state)
{
    //Circle, do not filter, draw all points
    GFX_Point start, end;

    //startX should always be less or equal to endX.
    if (drawPtStart->x > drawPtEnd->x ||
        drawPtStart->y != drawPtEnd->y)
        return;
    
    start.x = pt->x + drawPtStart->x;
    start.y = pt->y - drawPtStart->y;
                
    end.x = pt->x + drawPtEnd->x;
    end.y = pt->y - drawPtEnd->y;
    
    state->pipeline->drawLine[GFX_DRAW_LINE][GFX_ANTIALIAS_OFF](&start, 
                                                     &end,
                                                     state);
}

//This fill algorithm draws lines between the arc boundaries.
//This can take advantage of the GPU's line draw operation.
GFX_Result cpuDrawArc_Fill(const GFX_Point* pt,
                           uint32_t r,
                           int32_t startAngle,
                           int32_t centerAngle,
                           const GFX_DrawState* state)
{
    GFX_Point topRectPt; //Top and bottom points of arc rectangle
    GFX_Point arcStartPt0, arcStartPt1, arcEndPt0, arcEndPt1; //Points of arc edges
    GFX_Point scanPt;
    int32_t endAngle;
    uint32_t absStartAngle, absEndAngle; //start and end angles in positive angles
    ARC_DIR dir;
    GFX_Color drawColor;
    GFX_DrawState arcDrawState;
    int32_t thickness;
    uint32_t quadrantFlag = 0;
    
    GFX_Point drawPtStart, drawPtEnd;
    
    thickness = (state->thickness < r) ? state->thickness : r;
    
    //Don't care about wrapping 
    if (centerAngle == 0)
        return GFX_SUCCESS;

    startAngle %= 360;
    centerAngle %= 360;

    arcDrawState = *state;
    drawColor = arcDrawState.color;
    
    arcDrawState.antialias = GFX_FALSE;

    //Determine points of arc edges
    if (startAngle >= 0)
        absStartAngle = startAngle;
    else
        absStartAngle = 360 + startAngle;

    GFX_PolarToXY(r, absStartAngle, &arcStartPt1);
    GFX_PolarToXY(r - thickness, absStartAngle, &arcStartPt0);

    endAngle = startAngle + centerAngle;
    if (endAngle >= 0)
        absEndAngle = endAngle;
    else
        absEndAngle = 360 + endAngle;
    
    absEndAngle %= 360;

    GFX_PolarToXY(r, absEndAngle, &arcEndPt1);
    GFX_PolarToXY(r - thickness, absEndAngle, &arcEndPt0);

    if (centerAngle > 0)
    {
        dir  = CCW;
    }
    else
    {
        dir = CW;
    }

    //Determine bounding rectangle points
    topRectPt.x = - ((int32_t)r);
    topRectPt.y = r;

    //Scan thru the points in arc rectangle, per quadrant and filter points that are outside
    
    //Set the quadrant flag
    if (arcsOverlapQuadrant(absStartAngle, 
                            absEndAngle, 
                            dir, 
                            Q1) == GFX_TRUE)
        quadrantFlag |= (1 << Q1);
    
    if (arcsOverlapQuadrant(absStartAngle, 
                            absEndAngle, 
                            dir, 
                            Q2) == GFX_TRUE)
        quadrantFlag |= (1 << Q2);
    
    if (arcsOverlapQuadrant(absStartAngle, 
                            absEndAngle, 
                            dir, 
                            Q3) == GFX_TRUE)
        quadrantFlag |= (1 << Q3);
    
    if (arcsOverlapQuadrant(absStartAngle, 
                            absEndAngle, 
                            dir, 
                            Q4) == GFX_TRUE)
        quadrantFlag |= (1 << Q4);
    
    for (scanPt.y = topRectPt.y; scanPt.y >= 0; scanPt.y--)
    {
        int32_t outX, inX;

        outX = circleGetXGivenY(r, scanPt.y);
        inX = circleGetXGivenY(r - thickness, scanPt.y);
        
        drawPtStart.y = scanPt.y;
        drawPtEnd.y = drawPtStart.y;
        arcDrawState.color = drawColor;
        
        //Only scan if the arc overlaps with the quadrant
        if (scanPt.y >= 0 &&
            quadrantFlag & (1 << Q2))
        {
            do 
            {
                drawPtStart.x = -outX;
                drawPtEnd.x = -inX;            

                if (absStartAngle == absEndAngle)
                {
                    //Otherwise, draw all points
                    drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                }
                //If both start and end angles are in Q2
                else if ((90 <= absStartAngle && absStartAngle <= 180 ) && 
                         (90 <= absEndAngle && absEndAngle <= 180 ))
                {
                    //If CCW, exclude points RIGHT of start line and LEFT of end line
                    if (dir == CCW) 
                    {
                        if (absEndAngle > absStartAngle)
                        {
                            int32_t xPoint;

                            if (drawPtStart.y > arcStartPt1.y ||
                                drawPtStart.y < arcEndPt0.y)
                                continue;

                            //Clip start of line from the arc end edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            //Clip end of line from the arc start edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtEnd.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (absStartAngle > absEndAngle) 
                        {
                            int32_t xPoint;
                            GFX_Point drawPtStartCopy = drawPtStart;
                            GFX_Point drawPtEndCopy = drawPtEnd;

                            //Draw lines right of end Angle
                            if (drawPtStartCopy.y > arcEndPt1.y)
                            {
                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);
                            }
                            else if (drawPtStartCopy.y <= arcEndPt1.y &&
                                     drawPtStartCopy.y >= arcEndPt0.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStartCopy.y);
                                if (arcEndPt0.y != arcEndPt1.y && xPoint > drawPtStartCopy.x)
                                    drawPtStartCopy.x = xPoint;

                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);
                            }

                            //Draw lines left of Start Angle Edge
                            if (drawPtEndCopy.y >= arcStartPt0.y &&
                                drawPtEndCopy.y <= arcStartPt1.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtEndCopy.y);
                                if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEndCopy.x)
                                    drawPtEndCopy.x = xPoint;

                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }
                            else if (drawPtEndCopy.y < arcStartPt0.y)
                            {
                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }
                        }
                    }
                    //If CW, exclude points LEFT of start line and RIGHT of end line
                    else
                    {
                        if (absEndAngle > absStartAngle)
                        {
                            int32_t xPoint;
                            GFX_Point drawPtStartCopy = drawPtStart;
                            GFX_Point drawPtEndCopy = drawPtEnd;

                            //Draw lines right of start Angle
                            if (drawPtStartCopy.y > arcStartPt1.y)
                            {
                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);
                            }
                            else if (drawPtStartCopy.y <= arcStartPt1.y &&
                                     drawPtStartCopy.y >= arcStartPt0.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                                if (arcStartPt0.y != arcStartPt1.y && xPoint > drawPtStartCopy.x)
                                    drawPtStartCopy.x = xPoint;

                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);
                            }

                            //Clip end point to arc start edge
                            if (drawPtEndCopy.y <= arcEndPt0.y)
                            {
                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }
                            else if (drawPtEndCopy.y > arcEndPt0.y &&
                                     drawPtEndCopy.y < arcEndPt1.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                                if (arcEndPt0.y != arcEndPt1.y && xPoint < drawPtEndCopy.x)
                                    drawPtEndCopy.x = xPoint;

                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }

                        }
                        else if (absStartAngle > absEndAngle)
                        {
                            int32_t xPoint;

                            if (drawPtStart.y > arcEndPt1.y || drawPtStart.y < arcStartPt0.y)
                                continue;

                            //Clip line from the arc end edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            //Clip line from the arc start edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtEnd.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                }
                else if (absStartAngle <= 180 && absStartAngle > 90) 
                {
                    //If CCW, exclude points RIGHT of start line
                    if (dir == CCW) 
                    {
                        int32_t xPoint;

                        if (drawPtStart.y > arcStartPt1.y)
                            continue;

                        //Clip end point to arc start edge
                        if (drawPtStart.y >= arcStartPt0.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtStart.y < arcStartPt0.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                    //If CW, start point to arc start edge
                    else 
                    {
                        int32_t xPoint;

                        if (drawPtStart.y < arcStartPt0.y)
                            continue;

                        if (drawPtStart.x < arcStartPt1.x)
                            drawPtStart.x = arcStartPt1.x;

                        if (drawPtStart.y >= arcStartPt1.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtStart.y >= arcStartPt0.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                }
                //If end angle is in Q2, test agains Q2
                else if (absEndAngle <= 180 && absEndAngle > 90) 
                {
                    if (dir == CCW) 
                    {
                        int32_t xPoint;

                        if (drawPtStart.y > arcEndPt0.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                    else 
                    {
                        int32_t xPoint;

                        //Clip end point to arc end edge
                        if (drawPtStart.y < arcEndPt0.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtStart.y >= arcEndPt0.y &&
                                 drawPtStart.y <= arcEndPt1.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }

                    }
                }
                else 
                {
                    //Otherwise, draw all points
                    drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                }
            } while(0);
        }

        //Don't scan thru Q1 points if not needed. Optimize?
        if (scanPt.y >= 0 &&
            quadrantFlag & (1 << Q1))
        {
            do
            {
                drawPtStart.x = inX;
                drawPtEnd.x = outX;            

                if (absStartAngle == absEndAngle)
                {
                    //Otherwise, draw all points
                    drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                }
                //If both start and end angles are in Q1
                else if ((absStartAngle <= 90 && absStartAngle >= 0) && 
                       (absEndAngle <= 90 && absEndAngle >= 0))
                {
                    if (dir == CCW) 
                    {
                        //If CCW, exclude points RIGHT of start line and LEFT of end line
                        if (absEndAngle > absStartAngle) 
                        {
                            int32_t xPoint;

                            if (drawPtStart.y > arcEndPt1.y || 
                                drawPtStart.y < arcStartPt0.y)
                                continue;

                            //Clip start of line from the arc end edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            //Clip end of line from the arc start edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtEnd.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (absStartAngle > absEndAngle) 
                        {
                            int32_t xPoint;
                            GFX_Point drawPtStartCopy = drawPtStart;
                            GFX_Point drawPtEndCopy = drawPtEnd;

                            //Draw lines right of End Angle
                            if (drawPtStartCopy.y < arcEndPt0.y)
                            {
                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);                            
                            }
                            else if (drawPtStartCopy.y >= arcEndPt0.y &&
                                     drawPtStartCopy.y <= arcEndPt1.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStartCopy.y);
                                if (arcEndPt0.y != arcEndPt1.y && xPoint > drawPtStartCopy.x)
                                {
                                    drawPtStartCopy.x = xPoint;
                                }

                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);
                            }

                            //Draw lines left of Start Angle Edge
                            if (drawPtEndCopy.y > arcStartPt1.y)
                            {  
                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }
                            else if (drawPtEndCopy.y <= arcStartPt1.y &&
                                     drawPtEndCopy.y >= arcStartPt0.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtEndCopy.y);
                                if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEndCopy.x)
                                {
                                    drawPtEndCopy.x = xPoint;
                                }

                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }
                        }
                    }
                    //If CW, exclude points LEFT of start line and RIGHT of end line
                    else
                    {
                        if (absEndAngle > absStartAngle) 
                        {
                            int32_t xPoint;
                            GFX_Point drawPtStartCopy = drawPtStart;
                            GFX_Point drawPtEndCopy = drawPtEnd;

                            //Draw lines right of start Angle
                            if (drawPtStartCopy.y <= arcStartPt0.y)
                            {
                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);
                            }
                            else if (drawPtStartCopy.y < arcStartPt1.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                                if (arcStartPt0.y != arcStartPt1.y && xPoint > drawPtStartCopy.x)
                                    drawPtStartCopy.x = xPoint;

                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);
                            }

                            //Clip end point to arc end edge
                            if (drawPtEndCopy.y > arcEndPt1.y)
                            {
                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }
                            else if (drawPtEndCopy.y <= arcEndPt1.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                                if (arcEndPt0.y != arcEndPt1.y && xPoint < drawPtEnd.x)
                                    drawPtEndCopy.x = xPoint;

                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);                            
                            }
                        }
                        else if (absStartAngle > absEndAngle) 
                        {
                            int32_t xPoint;

                            //Clip start point from the arc start edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            //Clip start point from the arc end edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtEnd.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                }
                else if (0 <= absStartAngle && absStartAngle <= 90 )
                {
                    //If CCW, clip end point to start edge
                    if (dir == CCW) 
                    {
                        int32_t xPoint;

                        if (drawPtEnd.x > arcStartPt1.x)
                            drawPtEnd.x = arcStartPt1.x;

                        //Clip end point to arc start edge
                        if (drawPtStart.y >= arcStartPt1.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtStart.y < arcStartPt1.y &&
                                 drawPtStart.y >= arcStartPt0.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                    //If CW, exclude points LEFT of start line
                    else 
                    {
                        int32_t xPoint;

                        if (drawPtStart.y <= arcStartPt0.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtStart.y >= arcStartPt0.y &&
                                 drawPtStart.y <= arcStartPt1.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                }
                //If end angle is in Q1, test agains Q1
                else if (absEndAngle <= 90 && absEndAngle >= 0)
                {
                    if (dir == CCW) 
                    {
                        int32_t xPoint;

                        if (drawPtStart.y <= arcEndPt0.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtStart.y <= arcEndPt1.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                    else 
                    {
                        int32_t xPoint;

                        if (drawPtEnd.x > arcEndPt1.x)
                            drawPtEnd.x = arcEndPt1.x;

                        //Clip end point to arc end edge
                        if (drawPtStart.y >= arcEndPt1.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtStart.y >= arcEndPt0.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                }
                else
                {
                    //Otherwise, draw all points
                    drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                }
            } while(0);
        }

        drawPtStart.y = -scanPt.y;
        drawPtEnd.y = drawPtStart.y;

        //Fill Q3 area
        if (drawPtStart.y <= 0 &&
            quadrantFlag & (1 << Q3))
        {
            do
            {
                drawPtStart.x = -outX;
                drawPtEnd.x = -inX;

                if (absStartAngle == absEndAngle)
                {
                    //Otherwise, draw all points
                    drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                }
                //If both start and end angles are in Q1
                else if ((180 <= absStartAngle && absStartAngle <= 270 ) && 
                         (180 <= absEndAngle && absEndAngle <= 270))
                {
                    if (dir == CCW) 
                    {
                        //If CCW, exclude points RIGHT of start line and LEFT of end line
                        if (absEndAngle > absStartAngle) 
                        {
                            int32_t xPoint;

                            if (drawPtStart.y > arcStartPt0.y ||
                                drawPtStart.y < arcEndPt1.y)
                                continue;

                            //Clip start of line from the arc start edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            //Clip end of line from the arc end edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtEnd.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (absStartAngle > absEndAngle) 
                        {
                            int32_t xPoint;
                            GFX_Point drawPtStartCopy = drawPtStart;
                            GFX_Point drawPtEndCopy = drawPtEnd;

                            //Draw lines right of Start Angle
                            if (drawPtStartCopy.y < arcStartPt1.y)
                            {
                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);                            
                            }
                            else if (drawPtStartCopy.y >= arcStartPt1.y &&
                                     drawPtStartCopy.y <= arcStartPt0.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStartCopy.y);
                                if (arcStartPt1.y != arcStartPt0.y && xPoint > drawPtStartCopy.x)
                                {
                                    drawPtStartCopy.x = xPoint;
                                }

                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);
                            }

                            //Draw lines left of End Angle Edge
                            if (drawPtEndCopy.y > arcEndPt0.y)
                            {  
                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }
                            else if (drawPtEndCopy.y <= arcEndPt0.y &&
                                     drawPtEndCopy.y >= arcEndPt1.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt1, arcEndPt0, drawPtEndCopy.y);
                                if (arcEndPt1.y != arcEndPt0.y && xPoint < drawPtEndCopy.x)
                                {
                                    drawPtEndCopy.x = xPoint;
                                }

                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }
                        }
                    }
                    //If CW, exclude points LEFT of start line and RIGHT of end line
                    else
                    {
                        if (absEndAngle > absStartAngle) 
                        {
                            int32_t xPoint;
                            GFX_Point drawPtStartCopy = drawPtStart;
                            GFX_Point drawPtEndCopy = drawPtEnd;

                            //Draw lines left of start arc edge
                            if (drawPtEndCopy.y >= arcStartPt0.y)
                            {
                                drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                            }
                            else if (drawPtEndCopy.y < arcStartPt0.y &&
                                     drawPtEndCopy.y >= arcStartPt1.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtEndCopy.y);
                                if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEndCopy.x)
                                {
                                    drawPtEndCopy.x = xPoint;
                                }

                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }

                            if (drawPtStartCopy.x < arcEndPt1.x)
                                drawPtStartCopy.x = arcEndPt1.x;

                            //Draw lines right of end arc edge
                            if (drawPtStartCopy.y <= arcEndPt1.y)
                            {
                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);                            
                            }
                            else if (drawPtEndCopy.y <= arcEndPt0.y &&
                                     drawPtEndCopy.y >= arcEndPt1.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStartCopy.y);
                                if (arcEndPt0.y != arcEndPt1.y && xPoint > drawPtStartCopy.x)
                                    drawPtStartCopy.x = xPoint;

                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);                            
                            }
                        }
                        else if (absStartAngle > absEndAngle)
                        {
                            int32_t xPoint;

                            if (drawPtStart.y > arcEndPt0.y ||
                                drawPtStart.y < arcStartPt1.y)
                                continue;

                            //Clip start point from the arc start edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            //Clip start point from the arc end edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtEnd.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                }
                else if (180 <= absStartAngle && absStartAngle <= 270 )
                {
                    //If CCW, draw lines from start arc edge
                    if (dir == CCW) 
                    {
                        int32_t xPoint;

                        if (drawPtStart.x < arcStartPt1.x)
                            drawPtStart.x = arcStartPt1.x;

                        if (drawPtStart.y <= arcStartPt1.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtStart.y > arcStartPt1.y &&
                                 drawPtStart.y <= arcStartPt0.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint > drawPtStart.x)
                            {
                                drawPtStart.x = xPoint;
                            }

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                    //If CW, draw lines to start arc edge
                    else 
                    {
                        int32_t xPoint;

                        if (drawPtEnd.y >= arcStartPt0.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtEnd.y < arcStartPt0.y &&
                                 drawPtEnd.y >= arcStartPt1.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                }
                //If end angle is in Q3, test against Q3
                else if (180 <= absEndAngle && absEndAngle <= 270)
                {
                    //CCW, draw line to end arc edge
                    if (dir == CCW) 
                    {
                        int32_t xPoint;

                        if (drawPtEnd.y >= arcEndPt0.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtEnd.y <= arcEndPt0.y && 
                                 drawPtEnd.y >= arcEndPt1.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint < drawPtEnd.x)
                            {
                                drawPtEnd.x = xPoint;
                            }

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                    //CW, draw line from end arc edge
                    else 
                    {
                        int32_t xPoint;

                        if (drawPtStart.x < arcEndPt1.x)
                            drawPtStart.x = arcEndPt1.x;

                        if (drawPtStart.y <= arcEndPt1.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtStart.y <= arcEndPt0.y &&
                                 drawPtStart.y >= arcEndPt1.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                            if (arcStartPt1.y != arcEndPt0.y && xPoint > drawPtStart.x)
                            {
                                drawPtStart.x = xPoint;
                            }

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                }
                else
                {
                    //Otherwise, draw all points
                    drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                }
            } while(0);
        }

        if (drawPtStart.y <= 0 &&
            quadrantFlag & (1 << Q4))
        {
            do
            {
                drawPtStart.x = inX;
                drawPtEnd.x = outX;            

                if (absStartAngle == absEndAngle)
                {
                    //Otherwise, draw all points
                    drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                }
                //If both start and end angles are in Q1
                else if ((270 <= absStartAngle && absStartAngle <= 360 ) && 
                         (270 <= absEndAngle && absEndAngle <= 360))
                {
                    if (dir == CCW) 
                    {
                        if (absEndAngle > absStartAngle) 
                        {
                            //Draw line from arc start to arc end
                            int32_t xPoint;

                            if (drawPtStart.y > arcEndPt0.y ||
                                drawPtStart.y < arcStartPt1.y)
                                continue;

                            //Clip start of line from the arc start edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            //Clip end of line from the arc end edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtEnd.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (absStartAngle > absEndAngle) 
                        {
                            int32_t xPoint;
                            GFX_Point drawPtStartCopy = drawPtStart;
                            GFX_Point drawPtEndCopy = drawPtEnd;

                            //Draw lines right of Start Angle
                            if (drawPtStartCopy.y > arcStartPt0.y)
                            {
                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);                            
                            }
                            else if (drawPtStartCopy.y >= arcStartPt1.y &&
                                     drawPtStartCopy.y <= arcStartPt0.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStartCopy.y);
                                if (arcStartPt1.y != arcStartPt0.y && xPoint > drawPtStartCopy.x)
                                {
                                    drawPtStartCopy.x = xPoint;
                                }

                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);
                            }

                            //Draw lines left of End Angle Edge
                            if (drawPtEndCopy.y < arcEndPt1.y)
                            {  
                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }
                            else if (drawPtEndCopy.y <= arcEndPt0.y &&
                                     drawPtEndCopy.y >= arcEndPt1.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt1, arcEndPt0, drawPtEndCopy.y);
                                if (arcEndPt1.y != arcEndPt0.y && xPoint < drawPtEndCopy.x)
                                {
                                    drawPtEndCopy.x = xPoint;
                                }

                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }
                        }
                    }
                    //If CW, exclude points LEFT of start line and RIGHT of end line
                    else
                    {
                        if (absEndAngle > absStartAngle)
                        {
                            int32_t xPoint;
                            GFX_Point drawPtStartCopy = drawPtStart;
                            GFX_Point drawPtEndCopy = drawPtEnd;

                            if (drawPtEndCopy.x > arcStartPt1.x)
                                drawPtEndCopy.x = arcStartPt1.x;

                            //Draw lines left of start arc edge
                            if (drawPtEndCopy.y <= arcStartPt1.y)
                            {
                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }
                            else if (drawPtEndCopy.y <= arcStartPt0.y &&
                                     drawPtEndCopy.y > arcStartPt1.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtEndCopy.y);
                                if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEndCopy.x)
                                {
                                    drawPtEndCopy.x = xPoint;
                                }

                                drawArcFillLine(pt, &drawPtStart, &drawPtEndCopy, &arcDrawState);
                            }

                            //Draw lines right of end arc edge
                            if (drawPtStartCopy.y >= arcEndPt0.y)
                            {
                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);                            
                            }
                            else if (drawPtEndCopy.y < arcEndPt0.y &&
                                     drawPtEndCopy.y >= arcEndPt1.y)
                            {
                                xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStartCopy.y);
                                if (arcEndPt0.y != arcEndPt1.y && xPoint > drawPtStartCopy.x)
                                    drawPtStartCopy.x = xPoint;

                                drawArcFillLine(pt, &drawPtStartCopy, &drawPtEnd, &arcDrawState);                            
                            }
                        }
                        else if (absStartAngle > absEndAngle)
                        {
                            int32_t xPoint;

                            //Draw line from the arc end edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            //Draw line to the arc start edge
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtEnd.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                }
                else if (270 <= absStartAngle && absStartAngle <= 360)
                {
                    //If CCW, draw lines from start arc edge
                    if (dir == CCW) 
                    {
                        int32_t xPoint;

                        if (drawPtStart.y >= arcStartPt0.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtStart.y >= arcStartPt1.y &&
                                 drawPtStart.y < arcStartPt0.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint > drawPtStart.x)
                            {
                                drawPtStart.x = xPoint;
                            }

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                    //If CW, draw lines to start arc edge
                    else 
                    {
                        int32_t xPoint;

                        if (drawPtEnd.x > arcStartPt1.x)
                            drawPtEnd.x = arcStartPt1.x;

                        if (drawPtEnd.y <= arcStartPt1.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtEnd.y <= arcStartPt0.y &&
                                 drawPtEnd.y > arcStartPt1.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcStartPt0, arcStartPt1, drawPtStart.y);
                            if (arcStartPt0.y != arcStartPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                }
                //If end angle is in Q4, test agains Q4
                else if (270 <= absEndAngle && absEndAngle <= 360)
                {
                    //CCW, draw line from end arc edge
                    if (dir == CCW) 
                    {
                        int32_t xPoint;

                        if (drawPtEnd.x > arcEndPt1.x)
                            drawPtEnd.x = arcEndPt1.x;

                        if (drawPtEnd.y <= arcEndPt1.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtEnd.y <= arcEndPt0.y && 
                                 drawPtEnd.y > arcEndPt1.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtEnd.y);
                            if (arcEndPt0.y != arcEndPt1.y && xPoint < drawPtEnd.x)
                                drawPtEnd.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                    //CW, draw line from end arc edge
                    else 
                    {
                        int32_t xPoint;

                        if (drawPtStart.y >= arcEndPt0.y)
                        {
                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                        else if (drawPtStart.y < arcEndPt0.y &&
                                 drawPtStart.y >= arcEndPt1.y)
                        {
                            xPoint = GFX_UtilGetXGivenYOnLine(arcEndPt0, arcEndPt1, drawPtStart.y);
                            if (arcStartPt1.y != arcEndPt0.y && xPoint > drawPtStart.x)
                                drawPtStart.x = xPoint;

                            drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                        }
                    }
                }
                else
                {
                    //Otherwise, draw all points
                    drawArcFillLine(pt, &drawPtStart, &drawPtEnd, &arcDrawState);
                }
            } while(0);
        }
    }

    return GFX_SUCCESS;
}

#endif

#endif // GFX_DRAW_PIPELINE_ENABLED
