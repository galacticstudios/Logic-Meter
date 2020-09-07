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

static GFX_Result rejectLine(const GFX_Point* p1,
                             const GFX_Point* p2,
                             const GFX_DrawState* state)
{
    GFX_Rect lineRect;
    
#if GFX_COLOR_MASKING_ENABLED
    // mask color
    if(state->maskEnable == GFX_TRUE && state->color == state->maskValue)
       return GFX_FAILURE;
#endif

#if GFX_ALPHA_BLENDING_ENABLED
    // alpha is zero
    if((state->blendMode & GFX_BLEND_GLOBAL) > 0 &&
        state->globalAlphaValue == 0)
       return GFX_FAILURE;
#endif
    
    if(p1->x < p2->x)
    {
        lineRect.x = p1->x;
        lineRect.width = p2->x - p1->x + 1;
    }
    else
    {
        lineRect.x = p2->x;
        lineRect.width = p1->x - p2->x + 1;
    }
    
    if(p1->y < p2->y)
    {
        lineRect.y = p1->y;
        lineRect.height = p2->y - p1->y + 1;
    }
    else
    {
        lineRect.y = p2->y;
        lineRect.height = p1->y - p2->y + 1;
    }

#if GFX_LAYER_CLIPPING_ENABLED
    // clip rect against layer rect
    if(GFX_RectIntersects(&state->targetClipRect, &lineRect) == GFX_FALSE)
        return GFX_FAILURE;
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
    // clip rect against global clipping rect
    if((state->clipEnable == GFX_TRUE) && 
        GFX_RectIntersects(&state->clipRect, &lineRect) == GFX_FALSE)
        return GFX_FAILURE;
#endif

    return GFX_SUCCESS;
}

GFX_Result GFX_DrawLine(int32_t x1,
                        int32_t y1,
                        int32_t x2,
                        int32_t y2)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Point p1, p2;
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
    
    state.colorMode = context->colorMode;

    p1.x = x1;
    p1.y = y1;

    p2.x = x2;
    p2.y = y2;
    
    if(rejectLine(&p1, &p2, &state) == GFX_FAILURE)
        return GFX_FAILURE;

    res = state.pipeline->drawLine[context->draw.mode]
                                  [context->draw.antialias]
                                  (&p1, &p2, &state);
               
    return res;
}

static GFX_Result drawLineVert_PreClip(const GFX_Point* p1,
                                       const GFX_Point* p2,
                                       const GFX_DrawState* state)
{
    GFX_Rect rect = GFX_RectFromPoints(p1, p2);
#if (GFX_LAYER_CLIPPING_ENABLED) || (GFX_BOUNDS_CLIPPING_ENABLED)
    GFX_Rect clipRect;
#endif
    GFX_Point tfpnt1, tfpnt2;
    
#if GFX_LAYER_CLIPPING_ENABLED
    if(GFX_RectIntersects(&state->targetClipRect, &rect) == GFX_FALSE)
        return GFX_FAILURE;

    // clip rect against layer rect
    GFX_RectClip(&state->targetClipRect,
                 &rect,
                 &clipRect);
                 
    rect = clipRect;        
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
    
    if((state->clipEnable == GFX_TRUE) && 
        GFX_RectIntersects(&state->clipRect, &rect) == GFX_FALSE)
        return GFX_FAILURE;
        
    // clip rect against global clipping rect
    if(state->clipEnable == GFX_TRUE)
    {
        GFX_RectClip(&state->clipRect,
                     &rect,
                     &clipRect);
                     
        rect = clipRect; 
    }
#endif

    GFX_RectToPoints(&rect, &tfpnt1, &tfpnt2);

    //return cpuDrawLine_Vert(&tfpnt1, &tfpnt2, state);
    return state->pipeline->drawVertLine[state->mode]
                                        [state->antialias]
                                        (&tfpnt1, &tfpnt2, state);
}

static GFX_Result drawLineHorz_PreClip(const GFX_Point* p1,
                                       const GFX_Point* p2,
                                       const GFX_DrawState* state)
{
    GFX_Rect rect = GFX_RectFromPoints(p1, p2);
#if (GFX_LAYER_CLIPPING_ENABLED) || (GFX_BOUNDS_CLIPPING_ENABLED)
    GFX_Rect clipRect;
#endif
    GFX_Point tfpnt1, tfpnt2;
    
#if GFX_LAYER_CLIPPING_ENABLED
    if(GFX_RectIntersects(&state->targetClipRect, &rect) == GFX_FALSE)
        return GFX_FAILURE;
        
    // clip rect against layer rect
    GFX_RectClip(&state->targetClipRect,
                 &rect,
                 &clipRect);
                 
    rect = clipRect;        
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
    
    if((state->clipEnable == GFX_TRUE) && 
        GFX_RectIntersects(&state->clipRect, &rect) == GFX_FALSE)
        return GFX_FAILURE;
        
    // clip rect against global clipping rect
    if(state->clipEnable == GFX_TRUE)
    {
        GFX_RectClip(&state->clipRect,
                     &rect,
                     &clipRect);
                     
        rect = clipRect; 
    }
#endif

    GFX_RectToPoints(&rect, &tfpnt1, &tfpnt2);

    return state->pipeline->drawHorzLine[state->mode]
                                        [state->antialias]
                                        (&tfpnt1, &tfpnt2, state);
}

GFX_Result cpuDrawLine(const GFX_Point* p1,
                       const GFX_Point* p2,
                       const GFX_DrawState* state)
{    
    if(state->thickness > 1)
        return cpuDrawLine_Thick(p1, p2, state);
        
    // trivial cases
    else if(p1->x == p2->x)
        return drawLineVert_PreClip(p1, p2, state);
    else if(p1->y == p2->y)
        return drawLineHorz_PreClip(p1, p2, state);
    // normal case
    else
        return cpuDrawLine_Normal(p1, p2, state);
}

GFX_Result cpuDrawLine_Normal(const GFX_Point* p1,
                              const GFX_Point* p2,
                              const GFX_DrawState* state)
{
    int32_t dx, sx, dy, sy, e2, err;
    GFX_Point lp1, lp2;

    lp1.x = p1->x;
    lp1.y = p1->y;

    lp2.x = p2->x;
    lp2.y = p2->y;

    dx = lp2.x - lp1.x;
    
    if(dx < 0)
        dx *= -1;
        
    sx = lp1.x < lp2.x ? 1 : -1;
    
    dy = lp2.y - lp1.y;
    
    if(dy < 0)
        dy *= -1;
        
    dy *= -1;
    
    sy = lp1.y < lp2.y ? 1 : -1; 
    err = dx + dy;
       
    for(;;)
    {  
#if GFX_LAYER_CLIPPING_ENABLED
        // clip point against layer rect
        if(GFX_RectContainsPoint(&state->targetClipRect, &lp1) == GFX_FALSE)
            continue;
#endif
    
#if GFX_BOUNDS_CLIPPING_ENABLED
        // clip point against global clipping rect
        if((state->clipEnable == GFX_TRUE) && 
            GFX_RectContainsPoint(&state->clipRect, &lp1) == GFX_FALSE)
            continue;
#endif
    
        state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&lp1, state);
        
        if(lp1.x == lp2.x && lp1.y == lp2.y)
            break;
        
        e2 = 2 * err;
        
        if(e2 >= dy)
        {
            err += dy;
            lp1.x += sx;
        }
        
        if(e2 <= dx)
        {
            err += dx;
            lp1.y += sy;
        }
    }
   
    return GFX_SUCCESS;
}

GFX_Result cpuDrawLine_Thick(const GFX_Point* p1,
                             const GFX_Point* p2,
                             const GFX_DrawState* state)
{
    int32_t dx, sx, dy, sy, e2, err;
    GFX_Point lp1, lp2;
    
    lp1.x = p1->x;
    lp1.y = p1->y;

    lp2.x = p2->x;
    lp2.y = p2->y;

    dx = lp2.x - lp1.x;
    
    if(dx < 0)
        dx *= -1;
        
    sx = lp1.x < lp2.x ? 1 : -1;
    
    dy = lp2.y - lp1.y;
    
    if(dy < 0)
        dy *= -1;
        
    dy *= -1;
    
    sy = lp1.y < lp2.y ? 1 : -1; 
    err = dx + dy;
       
    for(;;)
    {  
        GFX_Bool clipPoint = GFX_FALSE;
#if GFX_LAYER_CLIPPING_ENABLED
        // clip point against layer rect
        if(GFX_RectContainsPoint(&state->targetClipRect, &lp1) == GFX_FALSE)
            clipPoint = GFX_TRUE;
#endif
        
#if GFX_BOUNDS_CLIPPING_ENABLED
        // clip point against global clipping rect
        if((state->clipEnable == GFX_TRUE) && 
            GFX_RectContainsPoint(&state->clipRect, &lp1) == GFX_FALSE)
            clipPoint = GFX_TRUE;
#endif

        if (clipPoint == GFX_FALSE)
            state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&lp1, state);
        
        if(lp1.x == lp2.x && lp1.y == lp2.y)
            break;
        
        e2 = 2 * err;
        
        if(e2 >= dy)
        {
            err += dy;
            lp1.x += sx;
        }
        
        if(e2 <= dx)
        {
            err += dx;
            lp1.y += sy;
        }
    }
   
    return GFX_SUCCESS;
}

#if 0
GFX_Result cpuDrawLine_Thick(GFX_Point* p1,
                             GFX_Point* p2,
                             GFX_DrawState* state)
{
    int x0, x1, x2, y0, y1, y2;
    int dx, dy, sx, sy;
    int err, e2;
    int wd, ed;
    
    GFX_Context* context;
    GFX_Point pt;
    
    x0 = p1->x;
    y0 = p1->y;
    x1 = p2->x;
    y1 = p2->y;
    
    dx = abs(x1-x0);
    sx = x0 < x1 ? 1 : -1; 
    dy = abs(y1-y0);
    sy = y0 < y1 ? 1 : -1; 
    
    err = dx - dy;
    wd = state->thickness;
    ed = dx + dy == 0 ? 1 : (int)sqrt((float)(dx*dx) + (float)(dy*dy));
    
    context = GFX_ActiveContext();

    for(wd = (wd + 1) / 2;;)
    {            
        pt.x = x0;
        pt.y = y0;
        
        context->hal.drawPixel[GFX_ANTIALIAS_OFF](&pt, state);                    
        //setPixelColor(x0,y0,max(0,255*(abs(err-dx+dy)/ed-wd+1));
        e2 = err;
        x2 = x0;
      
        /* x step */
        if(2 * e2 >= -dx)
        {                                           
            for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx)
            {
                pt.x = x0;
                pt.y = y2 += sy;

                context->hal.drawPixel[GFX_ANTIALIAS_OFF](&pt, state);
                //setPixelColor(x0, y2 += sy, max(0,255*(abs(e2)/ed-wd+1));
            }

            if(x0 == x1)
                break;

            e2 = err;
            err -= dy;
            x0 += sx; 
        }

        /* y step */
        if (2*e2 <= dy)
        {                                            
            for (e2 = dx - e2; (e2 < (ed * wd)) && (x1 != x2 || dx < dy); e2 += dy)
            {
                x2 += sx;

                pt.x = x2;
                pt.y = y0;

                context->hal.drawPixel[GFX_ANTIALIAS_OFF](&pt, state);
                //setPixelColor(x2 += sx, y0, max(0,255*(abs(e2)/ed-wd+1));
            }

            if(y0 == y1)
                break;

            err += dx;
            y0 += sy; 
        }
    }
    
    return GFX_SUCCESS;
}

GFX_Result cpuDrawLine_Thick_AA(GFX_Point* p1,
                                GFX_Point* p2,
                                GFX_DrawState* state)
{
    int x0, x1, x2, y0, y1, y2;
    int dx, dy, sx, sy;
    int err, e2;
    float wd, ed;
    
    int color;
    
    GFX_Context* context;
    GFX_Point pt;
    GFX_DrawState local;
    
    x0 = p1->x;
    y0 = p1->y;
    x1 = p2->x;
    y1 = p2->y;
    
    dx = abs(x1-x0);
    sx = x0 < x1 ? 1 : -1; 
    dy = abs(y1-y0);
    sy = y0 < y1 ? 1 : -1; 
    
    err = dx - dy;
    wd = (float)state->thickness;
    ed = dx + dy == 0 ? 1.0f : (float)sqrt((float)(dx*dx) + (float)(dy*dy));
    
    local = *state;
    local.blend = GFX_BLEND_ALPHA;
    context = GFX_ActiveContext();

    for(wd = (wd + 1) / 2;;)
    {            
        pt.x = x0;
        pt.y = y0;
        
        local.alpha = 255 * (abs(err - dx + dy) / (ed - wd + 1));
        
        if(local.alpha < 0)
            local.alpha = 0;
            
        local.alpha = 255 - local.alpha;
        
        context->hal.drawPixel[GFX_ANTIALIAS_ON](&pt, &local);                    
        //setPixelColor(x0,y0,max(0,255*(abs(err-dx+dy)/ed-wd+1));
        
        //color = 255 * (abs(err - dx + dy) / (ed - wd + 1));
        
        //if(color < 0)
        //    color = 0;
        
        e2 = err;
        x2 = x0;
      
        /* x step */
        if(2 * e2 >= -dx)
        {                                           
            for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx)
            {
                pt.x = x0;
                pt.y = y2 += sy;

                local.alpha = 255 * (abs(e2) / (ed - wd + 1));
                        
                if(local.alpha < 0)
                    local.alpha = 0;

                context->hal.drawPixel[GFX_ANTIALIAS_ON](&pt, &local);
                //setPixelColor(x0, y2 += sy, max(0,255*(abs(e2)/ed-wd+1));
                
                //color = 255 * (abs(e2) / (ed - wd + 1));
                
                //if(color < 0)
                    //color = 0;
            }

            if(x0 == x1)
                break;

            e2 = err;
            err -= dy;
            x0 += sx; 
        }

        /* y step */
        if (2*e2 <= dy)
        {                                            
            for (e2 = dx - e2; (e2 < (ed * wd)) && (x1 != x2 || dx < dy); e2 += dy)
            {
                x2 += sx;

                pt.x = x2;
                pt.y = y0;

                local.alpha = 255 * (abs(e2) / (ed - wd + 1));
                        
                if(local.alpha < 0)
                    local.alpha = 0;
                    
                context->hal.drawPixel[GFX_ANTIALIAS_ON](&pt, &local);
                //setPixelColor(x2 += sx, y0, max(0,255*(abs(e2)/ed-wd+1));
                
                //color = 255 * (abs(e2) / (ed - wd + 1));
                
                //if(color < 0)
                //    color = 0;
            }

            if(y0 == y1)
                break;

            err += dx;
            y0 += sy; 
        }
    }
    
    return GFX_SUCCESS;
}
#endif

#if 0
GFX_Result cpuDrawLine_Thick(GFX_Point* p1,
                             GFX_Point* p2,
                             GFX_DrawState* state)
{
    int x0 = p1->x;
    int y0 = p1->y;
    int x1 = p2->x;
    int y1 = p2->y;
    float wd = state->thickness;
    GFX_Context* context = GFX_ActiveContext();
    GFX_Point pnt;
    GFX_DrawState local = *state;

    int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1; 
    int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1; 
    int err = dx-dy, e2, x2, y2;                          /* error value e_xy */
    float ed = dx+dy == 0 ? 1 : (float)sqrt((float)dx*dx+(float)dy*dy);
    float t;
    int t1;
   
    /* pixel loop */
    for (wd = (wd+1)/2; ; )
    {                 
        pnt.x = x0;
        pnt.y = y0;               
        local.blend = GFX_BLEND_NONE;
        //local.alpha = GFX_Maxf(0, 255 * (abs(err-dx+dy)/ed-wd+1));
        //setPixelColor(x0,y0,max(0,255*(abs(err-dx+dy)/ed-wd+1));
        context->hal.drawPixel[GFX_ANTIALIAS_OFF](&pnt, &local);

        e2 = err; x2 = x0;
        
        /* x step */
        if (2*e2 >= -dx)
        {                                           
            for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx)
            {
                //setPixelColor(x0, y2 += sy, max(0,255*(abs(e2)/ed-wd+1));
                
                pnt.x = x0;
                pnt.y = y2 += sy;           
                t =  abs(e2)/ed-wd+1;  
                t1 = 255 * t;
                local.blend = GFX_BLEND_ALPHA;
                local.alpha = GFX_Clampi(0, 255, t1);  
                
                context->hal.drawPixel[GFX_ANTIALIAS_OFF](&pnt, &local);
            }

            if (x0 == x1)
                break;

            e2 = err; err -= dy; x0 += sx; 
        }
        
        /* y step */
        if (2*e2 <= dy)
        {                                            
            for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy)
            {
                //setPixelColor(x2 += sx, y0, max(0,255*(abs(e2)/ed-wd+1));
            
                pnt.x = x2 += sx;
                pnt.y = y0;          
                t =  abs(e2)/ed-wd+1;  
                t1 = 255 * t;
                local.blend = GFX_BLEND_ALPHA;
                local.alpha = GFX_Clampi(0, 255, t1);  
                //local.color = GFX_Maxf(0,255*(abs(e2)/ed-wd+1));
                
                context->hal.drawPixel[GFX_ANTIALIAS_OFF](&pnt, &local);
            }
                
            if (y0 == y1)
                break;
            
            err += dx; y0 += sy; 
        }
    }
   
    return GFX_SUCCESS;
}
#endif

GFX_Result cpuDrawLine_AA(const GFX_Point* p1,
                          const GFX_Point* p2,
                          const GFX_DrawState* state)
{
#if 0
    int32_t dx = abs(x2 - x1);
    int32_t sx = x1 < x2 ? 1 : -1; 
    int32_t dy = abs(y2 - y1);
    int32_t sy = y1 < y2 ? 1 : -1;
    int32_t err = dx - dy;
    int32_t e2, x3, y3;
    
    float ed = dx + dy == 0 ? 1 : sqrt((float)dx * dx + (float)dy * dy);

    for (thk = (thk+1) / 2;;)
    {                                   /* pixel loop */
        //setPixelColor(x1,y1,max(0,255*(abs(err-dx+dy)/ed-wd+1));
        //context->hal.drawPixel(context, x1, y1);
         
        e2 = err; x2 = x1;
        
        if (2*e2 >= -dx)
        {                                           /* x step */
            //for (e2 += dy, y2 = y1; e2 < ed*wd && (y2 != y3 || dx > dy); e2 += dx)
                //setPixelColor(x1, y3 += sy, max(0,255*(abs(e2)/ed-wd+1));
                //context->hal.drawPixel(context, x1, y1);
                
            if (x1 == x2)
                break;
                
            e2 = err;
            err -= dy;
            x1 += sx; 
        } 
        if (2*e2 <= dy)
        {                                            /* y step */
        for (e2 = dx-e2; e2 < ed*wd && (x2 != x3 || dx < dy); e2 += dy)
            setPixelColor(x3 += sx, y1, max(0,255*(abs(e2)/ed-wd+1));
        
            if (y1 == y2)
                break;
            
            err += dx;
            y1 += sy; 
        }
    }
    #endif
    
    return GFX_FAILURE;
}

#if 0
GFX_Result cpuDrawLineHorz(GFX_Context* context,
                           int32_t x1,
                           int32_t x2,
                           int32_t y,
                           GFX_DrawState* state)
{
    GFX_Point p;
    GFX_Rect layerRect = context->layer.active->rect.local;
    
    // ensure proper coordinate order
    if(x2 < x1)
    {
        p.x = x1;
        x1 = x2;
        x2 = p.x;
    }
    
    // see if line is completely outside the layer area
    if(x2 < 0 || x1 >= layerRect.width || y < 0 || y >= layerRect.height)
        return GFX_FAILURE;

    // perform draw operation
    p.y = y;
    
    for(p.x = x1; p.x < x2; p.x++)
        context->hal.drawPixel[GFX_ANTIALIAS_OFF](context, &p, state);
        
    return GFX_SUCCESS;
}
                                  
GFX_Result cpuDrawLineVert(GFX_Context* context,
                           int32_t x,
                           int32_t y1,
                           int32_t y2,
                           GFX_DrawState* state)
{
    GFX_Point p;
    GFX_Rect layerRect = context->layer.active->rect.local;
    
    // ensure proper coordinate order
    if(y2 < y1)
    {
        p.y = y1;
        y1 = y2;
        y2 = p.y;
    }
    
    // see if line is completely outside the layer area
    if(x < 0 || x >= layerRect.width || y2 < 0 || y1 >= layerRect.height)
        return GFX_FAILURE;

    // perform draw operation
    p.x = x;
    
    for(p.y = y1; p.y < y2; p.y++)
        context->hal.drawPixel[GFX_ANTIALIAS_OFF](context, &p, state);

    return GFX_SUCCESS;
}
#endif

GFX_Result cpuDrawLine_Gradient(const GFX_Point* p1,
                                const GFX_Point* p2,
                                const GFX_DrawState* state)
{
    return GFX_SUCCESS;
}
                                
GFX_Result cpuDrawLine_Gradient_AA(const GFX_Point* p1,
                                   const GFX_Point* p2,
                                   const GFX_DrawState* state)
{
    return GFX_SUCCESS;
}  
                 
                 
GFX_Result cpuDrawLine_Horz(const GFX_Point* p1,
                            const GFX_Point* p2,
                            const GFX_DrawState* state)
{
    GFX_Point drawPoint;
        
    drawPoint.y = p1->y;
    
    for(drawPoint.x = p1->x; drawPoint.x <= p2->x; drawPoint.x++)
        state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&drawPoint, state);
   
    return GFX_SUCCESS;
}
                                  
GFX_Result cpuDrawLine_Vert(const GFX_Point* p1,
                            const GFX_Point* p2,
                            const GFX_DrawState* state)
{
    GFX_Point drawPoint;
    
    drawPoint.x = p1->x;
    
    for(drawPoint.y = p1->y; drawPoint.y <= p2->y; drawPoint.y++)
        state->pipeline->drawPixel[GFX_ANTIALIAS_OFF](&drawPoint, state);
   
    return GFX_SUCCESS;
}              

#endif // GFX_DRAW_PIPELINE_ENABLED