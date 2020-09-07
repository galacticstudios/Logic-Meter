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


#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"

#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_utils.h"

extern int32_t rectCounter;

void laWidget_SkinClassic_FillRoundCornerRect(const GFX_Rect* rect,
                                        GFX_Color clr,
                                        uint32_t radius,
                                        const GFX_Rect* dmgRect)
{
    GFX_Rect clipRect;
    GFX_Rect drawRect;
    
    GFX_Set(GFXF_DRAW_COLOR, clr);
    GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        
    //Draw the center rectangle
    drawRect.x = rect->x + radius;
    drawRect.y = rect->y + radius;
    drawRect.width = rect->width - (radius * 2);
    drawRect.height = rect->height - (radius * 2);
    
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, dmgRect, &clipRect);

        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
    }
    
    //Draw the top rectangle
    drawRect.x = rect->x + radius;
    drawRect.y = rect->y;
    drawRect.width = rect->width - (radius * 2);
    drawRect.height = radius;
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, dmgRect, &clipRect);

        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
    }
    
    //Draw the bottom rectangle
    drawRect.x = rect->x + radius;
    drawRect.y = rect->y + rect->height - radius;
    drawRect.width = rect->width - (radius * 2);
    drawRect.height = radius;
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, dmgRect, &clipRect);

        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
    }
    
    //Draw the left rectangle
    drawRect.x = rect->x;
    drawRect.y = rect->y + radius;
    drawRect.width = radius;
    drawRect.height = rect->height - (radius * 2);
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, dmgRect, &clipRect);

        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
    }
    
    //Draw the right rectangle
    drawRect.x = rect->x + rect->width - radius;
    drawRect.y = rect->y + radius;
    drawRect.width = radius;
    drawRect.height = rect->height - (radius * 2);
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, dmgRect, &clipRect);

        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
    }    
    
    //Draw the corners
    //Upper left
    drawRect.x = rect->x;
    drawRect.y = rect->y;
    drawRect.width = radius;
    drawRect.height = radius;
    
    GFX_Set(GFXF_DRAW_THICKNESS, radius);
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_DrawArc(drawRect.x + radius,
                    drawRect.y + radius,
                    radius,
                    90,
                    90);
    }
    
    //Upper right
    drawRect.x = rect->x + rect->width - radius;
    drawRect.y = rect->y;
    drawRect.width = radius;
    drawRect.height = radius;
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_DrawArc(drawRect.x,
                    drawRect.y + radius,
                    radius,
                    0,
                    90);
    }
    
    //Lower left
    drawRect.x = rect->x;
    drawRect.y = rect->y + rect->height - (radius * 2);
    drawRect.width = radius;
    drawRect.height = radius;
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_DrawArc(drawRect.x + radius,
                    drawRect.y + radius,
                    radius,
                    180,
                    90);
    }
    
    //Lower right
    drawRect.x = rect->x + rect->width - radius;
    drawRect.y = rect->y + rect->height - radius;
    drawRect.width = radius;
    drawRect.height = radius;
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_DrawArc(drawRect.x,
                    drawRect.y,
                    radius,
                    270,
                    90);
    }
}

void laWidget_SkinClassic_FillRect(const GFX_Rect* rect,
                                   GFX_Color clr,
                                   const GFX_Rect* dmgRect)
{
    GFX_Rect clipRect;
    
    /*GFX_SetDebugRect(dmgRect->x,
                     dmgRect->y,
                     dmgRect->width,
                     dmgRect->height);
    
    GFX_SetDebugRectColor(0xFF0000);                     
    GFX_ShowDebugRect(GFX_TRUE);
    GFX_NextDebugRect();*/
    
    if(GFX_RectIntersects(rect, dmgRect) == GFX_TRUE)
    {
        GFX_Set(GFXF_DRAW_COLOR, clr);
        GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_FILL);
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    
        GFX_RectClip(rect, dmgRect, &clipRect);

        GFX_DrawRect(clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);
                     
        /*GFX_SetDebugRect(clipRect.x,
                         clipRect.y,
                         clipRect.width,
                         clipRect.height);
                    
        GFX_SetDebugRectColor(0xFF00);     
        GFX_ShowDebugRect(GFX_TRUE);
        GFX_NextDebugRect();*/
        
        /*printf("filling %i, %i, %i, %i\n", clipRect.x,
                     clipRect.y,
                     clipRect.width,
                     clipRect.height);*/
    }
}

void laWidget_SkinClassic_DrawBackground(laWidget* wgt, GFX_Color clr)
{
    GFX_Rect layerRect = laUtils_WidgetLayerRect(wgt);

    laLayer* layer = laUtils_GetLayer(wgt);
    
    //Only support round corners for no or line borders
    if (wgt->cornerRadius > 0 && 
        (wgt->borderType == LA_WIDGET_BORDER_NONE || 
         wgt->borderType == LA_WIDGET_BORDER_LINE))
    {
        laWidget_SkinClassic_FillRoundCornerRect(&layerRect, 
                                                 clr,
                                                 wgt->cornerRadius,
                                                 &layer->clippedDrawingRect); 
    }    
    else
    {
        laWidget_SkinClassic_FillRect(&layerRect, clr, &layer->clippedDrawingRect);
    }

}

void laWidget_SkinClassic_DrawRoundCornerBackground(laWidget* wgt, 
                                                    GFX_Color clr)
{
    GFX_Rect layerRect = laUtils_WidgetLayerRect(wgt);

    laLayer* layer = laUtils_GetLayer(wgt);
    
    laWidget_SkinClassic_FillRoundCornerRect(&layerRect, 
                                             clr,
                                             wgt->cornerRadius,
                                             &layer->clippedDrawingRect);
}

void laWidget_SkinClassic_DrawStandardBackground(laWidget* wgt)
{
    if(wgt->backgroundType == LA_WIDGET_BACKGROUND_FILL)
    {
        laWidget_SkinClassic_DrawBackground(wgt, wgt->scheme->base);
    }
    else if(wgt->backgroundType == LA_WIDGET_BACKGROUND_CACHE)
    {
        laWidget_SkinClassic_DrawBlit(wgt, wgt->cache);
    }
}

void laWidget_SkinClassic_DrawLineBorderLeft(const GFX_Rect* rect,
                                             GFX_Color clr,
                                             const GFX_Rect* dmgRect)
{
    GFX_Rect lineRect, clipRect;

    lineRect.x = rect->x;
    lineRect.y = rect->y + 1;
    lineRect.height = rect->height - 2;
    lineRect.width = 1;

    if(GFX_RectIntersects(&lineRect, dmgRect) == GFX_TRUE)
    {
        GFX_Set(GFXF_DRAW_COLOR, clr);
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    
        GFX_RectClip(&lineRect, dmgRect, &clipRect);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x,
                     clipRect.y + clipRect.height - 1);
    }
}

void laWidget_SkinClassic_DrawLineBorderTop(const GFX_Rect* rect,
                                            GFX_Color clr,
                                            const GFX_Rect* dmgRect)
{
    GFX_Rect lineRect, clipRect;

    lineRect.x = rect->x;
    lineRect.y = rect->y;
    lineRect.height = 1;
    lineRect.width = rect->width;

    if(GFX_RectIntersects(&lineRect, dmgRect) == GFX_TRUE)
    {
        GFX_Set(GFXF_DRAW_COLOR, clr);
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    
        GFX_RectClip(&lineRect, dmgRect, &clipRect);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x + clipRect.width - 1,
                     clipRect.y);
    }
}

void laWidget_SkinClassic_DrawLineBorderRight(const GFX_Rect* rect,
                                              GFX_Color clr,
                                              const GFX_Rect* dmgRect)
{
    GFX_Rect lineRect, clipRect;

    lineRect.x = rect->x + rect->width - 1;
    lineRect.y = rect->y + 1;
    lineRect.height = rect->height - 2;
    lineRect.width = 1;

    if(GFX_RectIntersects(&lineRect, dmgRect) == GFX_TRUE)
    {
        GFX_Set(GFXF_DRAW_COLOR, clr);
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    
        GFX_RectClip(&lineRect, dmgRect, &clipRect);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x,
                     clipRect.y + clipRect.height - 1);
    }
}

void laWidget_SkinClassic_DrawLineBorderBottom(const GFX_Rect* rect,
                                               GFX_Color clr,
                                               const GFX_Rect* dmgRect)
{
    GFX_Rect lineRect, clipRect;

    lineRect.x = rect->x;
    lineRect.y = rect->y + rect->height - 1;
    lineRect.height = 1;
    lineRect.width = rect->width;

    if(GFX_RectIntersects(&lineRect, dmgRect) == GFX_TRUE)
    {
        GFX_Set(GFXF_DRAW_COLOR, clr);
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    
        GFX_RectClip(&lineRect, dmgRect, &clipRect);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x + clipRect.width - 1,
                     clipRect.y);
    }
}

void laWidget_SkinClassic_DrawRoundCornerLineBorder(const GFX_Rect* rect,
                                                    GFX_Color clr,
                                                    uint32_t radius,
                                                    const GFX_Rect* dmgRect)
{
    GFX_Rect clipRect;
    GFX_Rect drawRect;
    
    GFX_Set(GFXF_DRAW_COLOR, clr);
    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
        
    //Draw the top line
    drawRect.x = rect->x + radius;
    drawRect.y = rect->y;
    drawRect.width = rect->width - (radius * 2);
    drawRect.height = 1;
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, dmgRect, &clipRect);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x + clipRect.width,
                     clipRect.y);

    }
    
    //Draw the bottom line
    drawRect.x = rect->x + radius;
    drawRect.y = rect->y + rect->height - 1;
    drawRect.width = rect->width - (radius * 2);
    drawRect.height = 1;
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, dmgRect, &clipRect);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x + clipRect.width,
                     clipRect.y);
    }
    
    //Draw the left line
    drawRect.x = rect->x;
    drawRect.y = rect->y + radius;
    drawRect.width = 1;
    drawRect.height = rect->height - (radius * 2);
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, dmgRect, &clipRect);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x,
                     clipRect.y + clipRect.height);
    }
    
    //Draw the right line
    drawRect.x = rect->x + rect->width - 1;
    drawRect.y = rect->y + radius;
    drawRect.width = 1;
    drawRect.height = rect->height - (radius * 2);
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_RectClip(&drawRect, dmgRect, &clipRect);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x,
                     clipRect.y + clipRect.height);
    }    
    
    //Draw the corners
    GFX_Set(GFXF_DRAW_THICKNESS, 1);
    //Upper left
    drawRect.x = rect->x;
    drawRect.y = rect->y;
    drawRect.width = radius;
    drawRect.height = radius;
    
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_DrawArc(drawRect.x + radius,
                    drawRect.y + radius,
                    radius,
                    90,
                    90);
    }
    
    //Upper right
    drawRect.x = rect->x + rect->width - radius;
    drawRect.y = rect->y;
    drawRect.width = radius;
    drawRect.height = radius;
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_DrawArc(drawRect.x,
                    drawRect.y + radius,
                    radius,
                    0,
                    90);
    }
    
    //Lower left
    drawRect.x = rect->x;
    drawRect.y = rect->y + rect->height - (radius * 2);
    drawRect.width = radius;
    drawRect.height = radius;
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_DrawArc(drawRect.x + radius,
                    drawRect.y + radius,
                    radius,
                    180,
                    90);
    }
    
    //Lower right
    drawRect.x = rect->x + rect->width - radius;
    drawRect.y = rect->y + rect->height - radius;
    drawRect.width = radius;
    drawRect.height = radius;
    if(GFX_RectIntersects(&drawRect, dmgRect) == GFX_TRUE)
    {
        GFX_DrawArc(drawRect.x,
                    drawRect.y,
                    radius,
                    270,
                    90);
    }
}

void laWidget_SkinClassic_DrawLineBorder(const GFX_Rect* rect,
                                         GFX_Color clr,
                                         const GFX_Rect* dmgRect)
{
    laWidget_SkinClassic_DrawLineBorderLeft(rect, clr, dmgRect);
    laWidget_SkinClassic_DrawLineBorderTop(rect, clr, dmgRect);
    laWidget_SkinClassic_DrawLineBorderRight(rect, clr, dmgRect);
    laWidget_SkinClassic_DrawLineBorderBottom(rect, clr, dmgRect);
}

void laWidget_SkinClassic_DrawBevelBorderLeft(const GFX_Rect* rect,
                                              GFX_Color outer,
                                              GFX_Color inner,
                                              const GFX_Rect* dmgRect)
{
    GFX_Rect clipRect;
    GFX_Rect outerLine, innerLine;

    outerLine.x = rect->x;
    outerLine.y = rect->y + 1;
    outerLine.height = rect->height - 2;
    outerLine.width = 1;

    innerLine.x = rect->x + 1;
    innerLine.y = rect->y + 1;
    innerLine.height = rect->height - 2;
    innerLine.width = 1;

    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);

    if(GFX_RectIntersects(dmgRect, &outerLine) == GFX_TRUE)
    {
        GFX_RectClip(dmgRect, &outerLine, &clipRect);

        GFX_Set(GFXF_DRAW_COLOR, outer);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x,
                     clipRect.y + clipRect.height - 1);
    }

    if(GFX_RectIntersects(dmgRect, &innerLine) == GFX_TRUE)
    {
        GFX_RectClip(dmgRect, &innerLine, &clipRect);

        GFX_Set(GFXF_DRAW_COLOR, inner);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x,
                     clipRect.y + clipRect.height - 1);
    }
}

void laWidget_SkinClassic_DrawBevelBorderTop(const GFX_Rect* rect,
                                             GFX_Color outer,
                                             GFX_Color inner,
                                             const GFX_Rect* dmgRect)
{
    GFX_Rect clipRect;
    GFX_Rect outerLine, innerLine;
    
    outerLine.x = rect->x;
    outerLine.y = rect->y;
    outerLine.width = rect->width;
    outerLine.height = 1;

    innerLine.x = rect->x + 1;
    innerLine.y = rect->y + 1;
    innerLine.width = rect->width - 2;
    innerLine.height = 1;

    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);

    if(GFX_RectIntersects(dmgRect, &outerLine) == GFX_TRUE)
    {
        GFX_RectClip(dmgRect, &outerLine, &clipRect);

        GFX_Set(GFXF_DRAW_COLOR, outer);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x + clipRect.width - 1,
                     clipRect.y);
    }

    if(GFX_RectIntersects(dmgRect, &innerLine) == GFX_TRUE)
    {
        GFX_RectClip(dmgRect, &innerLine, &clipRect);

        GFX_Set(GFXF_DRAW_COLOR, inner);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x + clipRect.width - 1,
                     clipRect.y);
    }
}

void laWidget_SkinClassic_DrawBevelBorderRight(const GFX_Rect* rect,
                                               GFX_Color outer,
                                               GFX_Color inner,
                                               const GFX_Rect* dmgRect)
{
    GFX_Rect clipRect;
    GFX_Rect outerLine, innerLine;

    outerLine.x = rect->x + rect->width - 1;
    outerLine.y = rect->y + 1;
    outerLine.height = rect->height - 2;
    outerLine.width = 1;

    innerLine.x = rect->x + rect->width - 2;
    innerLine.y = rect->y + 1;
    innerLine.height = rect->height - 2;
    innerLine.width = 1;

    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);

    if(GFX_RectIntersects(dmgRect, &outerLine) == GFX_TRUE)
    {
        GFX_RectClip(dmgRect, &outerLine, &clipRect);

        GFX_Set(GFXF_DRAW_COLOR, outer);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x,
                     clipRect.y + clipRect.height - 1);
    }

    if(GFX_RectIntersects(dmgRect, &innerLine) == GFX_TRUE)
    {
        GFX_RectClip(dmgRect, &innerLine, &clipRect);

        GFX_Set(GFXF_DRAW_COLOR, inner);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x,
                     clipRect.y + clipRect.height - 1);
    }
}

void laWidget_SkinClassic_DrawBevelBorderBottom(const GFX_Rect* rect,
                                                GFX_Color outer,
                                                GFX_Color inner,
                                                const GFX_Rect* dmgRect)
{
    GFX_Rect clipRect;
    GFX_Rect outerLine, innerLine;

    outerLine.x = rect->x;
    outerLine.y = rect->y + rect->height - 1;
    outerLine.width = rect->width;
    outerLine.height = 1;

    innerLine.x = rect->x + 1;
    innerLine.y = rect->y + rect->height - 2;
    innerLine.width = rect->width - 2;
    innerLine.height = 1;

    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);

    if(GFX_RectIntersects(dmgRect, &outerLine) == GFX_TRUE)
    {
        GFX_RectClip(dmgRect, &outerLine, &clipRect);

        GFX_Set(GFXF_DRAW_COLOR, outer);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x + clipRect.width - 1,
                     clipRect.y);
    }

    if(GFX_RectIntersects(dmgRect, &innerLine) == GFX_TRUE)
    {
        GFX_RectClip(dmgRect, &innerLine, &clipRect);

        GFX_Set(GFXF_DRAW_COLOR, inner);

        GFX_DrawLine(clipRect.x,
                     clipRect.y,
                     clipRect.x + clipRect.width - 1,
                     clipRect.y);
    }
}

void laWidget_SkinClassic_Draw2x2BeveledBorder(const GFX_Rect* rect,
                                               GFX_Color leftUpOuter,
                                               GFX_Color leftUpInner,
                                               GFX_Color bottomRightOuter,
                                               GFX_Color bototmRightInner,
                                               const GFX_Rect* dmgRect)
{
    laWidget_SkinClassic_DrawBevelBorderLeft(rect,
                                             leftUpOuter,
                                             leftUpInner,
                                             dmgRect);
                                             
    laWidget_SkinClassic_DrawBevelBorderRight(rect,
                                              bottomRightOuter,
                                              bototmRightInner,
                                              dmgRect);                                             
                                             
    laWidget_SkinClassic_DrawBevelBorderTop(rect,
                                            leftUpOuter,
                                            leftUpInner,
                                            dmgRect);

    laWidget_SkinClassic_DrawBevelBorderBottom(rect,
                                               bottomRightOuter,
                                               bototmRightInner,
                                               dmgRect);
}                                           
                                            
void laWidget_SkinClassic_Draw1x2BeveledBorder(const GFX_Rect* rect,
                                               GFX_Color leftUp,
                                               GFX_Color bottomRightOuter,
                                               GFX_Color bototmRightInner,
                                               const GFX_Rect* dmgRect)
{
    laWidget_SkinClassic_DrawLineBorderLeft(rect, leftUp, dmgRect);
    
    laWidget_SkinClassic_DrawBevelBorderRight(rect,
                                              bottomRightOuter,
                                              bototmRightInner,
                                              dmgRect);
                                            
    laWidget_SkinClassic_DrawLineBorderTop(rect, leftUp, dmgRect);


    laWidget_SkinClassic_DrawBevelBorderBottom(rect,
                                               bottomRightOuter,
                                               bototmRightInner,
                                               dmgRect);
}

void laWidget_SkinClassic_DrawBlit(laWidget* wgt,
                                   GFX_PixelBuffer* buffer)
{
    GFX_Rect widgetRect = laUtils_WidgetLayerRect(wgt);
    GFX_Rect clipRect, imgSrcRect;

    laLayer* layer = laUtils_GetLayer(wgt);
    
    imgSrcRect.x = 0;
    imgSrcRect.y = 0;
    imgSrcRect.width = widgetRect.width;
    imgSrcRect.height = widgetRect.height;

    GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);

    if(GFX_RectIntersects(&widgetRect, &layer->clippedDrawingRect) == GFX_TRUE)
    {
        clipRect = GFX_RectClipAdj(&widgetRect, &layer->clippedDrawingRect, &imgSrcRect);

        GFX_DrawBlit(buffer,
                     imgSrcRect.x,
                     imgSrcRect.y,
                     imgSrcRect.width,
                     imgSrcRect.height,
                     clipRect.x,
                     clipRect.y);
    }
    
}

void laWidget_SkinClassic_DrawStandardLineBorder(laWidget* wgt)
{
    GFX_Rect localRect = laUtils_WidgetLocalRect(wgt);
    laLayer* layer = laUtils_GetLayer(wgt);
    
    laUtils_RectToLayerSpace(wgt, &localRect);
    
    if (wgt->cornerRadius == 0)
    {
        laWidget_SkinClassic_DrawLineBorder(&localRect,
                                            wgt->scheme->shadowDark,
                                            &layer->clippedDrawingRect);
    }
    else
    {
        laWidget_SkinClassic_DrawRoundCornerLineBorder(&localRect,
                                                       wgt->scheme->shadowDark,
                                                       wgt->cornerRadius,
                                                       &layer->clippedDrawingRect);        
    }
}

void laWidget_SkinClassic_DrawStandardRoundCornerLineBorder(laWidget* wgt)
{
    GFX_Rect localRect = laUtils_WidgetLocalRect(wgt);
    laLayer* layer = laUtils_GetLayer(wgt);
    
    laUtils_RectToLayerSpace(wgt, &localRect);
    
    laWidget_SkinClassic_DrawRoundCornerLineBorder(&localRect,
                                                   wgt->scheme->shadowDark,
                                                   wgt->cornerRadius,
                                                   &layer->clippedDrawingRect);
}

void laWidget_SkinClassic_DrawStandardRaisedBorder(laWidget* wgt)
{
    GFX_Rect localRect = laUtils_WidgetLocalRect(wgt);
    laLayer* layer = laUtils_GetLayer(wgt);
    
    laUtils_RectToLayerSpace(wgt, &localRect);
    
    laWidget_SkinClassic_Draw2x2BeveledBorder(&localRect,
                                             wgt->scheme->highlightLight,
                                             wgt->scheme->highlightLight,
                                             wgt->scheme->shadowDark,
                                             wgt->scheme->shadow,
                                             &layer->clippedDrawingRect);
}

void laWidget_SkinClassic_DrawStandardLoweredBorder(laWidget* wgt)
{
    GFX_Rect localRect = laUtils_WidgetLocalRect(wgt);
    laLayer* layer = laUtils_GetLayer(wgt);
    
    laUtils_RectToLayerSpace(wgt, &localRect);
    
    laWidget_SkinClassic_Draw2x2BeveledBorder(&localRect,
                                             wgt->scheme->shadow,
                                             wgt->scheme->shadowDark,
                                             wgt->scheme->highlightLight,
                                             wgt->scheme->highlightLight,
                                             &layer->clippedDrawingRect);
}

void laWidget_SkinClassic_DrawStandardHybridBorder(laWidget* wgt)
{
    GFX_Rect localRect = laUtils_WidgetLocalRect(wgt);
    laLayer* layer = laUtils_GetLayer(wgt);
    
    laUtils_RectToLayerSpace(wgt, &localRect);
    
    laWidget_SkinClassic_Draw1x2BeveledBorder(&localRect,
                                              wgt->scheme->highlightLight,
                                              wgt->scheme->shadowDark,
                                              wgt->scheme->shadow,
                                              &layer->clippedDrawingRect);
}

void laWidget_SkinClassic_InvalidateBorderAreas(laWidget* wgt)
{
	GFX_Rect rect, dmgRect;
	int32_t width, height;
	laLayer* layer;
	
	if(wgt->borderType == LA_WIDGET_BORDER_NONE)
	    return;
	
	rect = laUtils_WidgetLayerRect(wgt);
	layer = laUtils_GetLayer(wgt);
	
	if(wgt->borderType == LA_WIDGET_BORDER_LINE)
	{
	    if(rect.width == 0 || rect.height == 0)
			return;

		if(rect.width <= 1 || rect.height <= 1)
		{
			laLayer_AddDamageRect(layer, &rect, LA_TRUE);

			return;
		}
	
	    width = 1;
	    height = 1;
	}
	else
	{
	    if(rect.width == 0 || rect.height == 0)
			return;

		if(rect.width <= 3 || rect.height <= 3)
		{
			laLayer_AddDamageRect(layer, &rect, LA_TRUE);
			
			return;
        }
        
        width = 2;
	    height = 2;
	}
	
	// left line
	dmgRect.x = rect.x;
	dmgRect.y = rect.y;
	dmgRect.width = width;
	dmgRect.height = rect.height;

	laLayer_AddDamageRect(layer, &dmgRect, LA_TRUE);

	// top line
	dmgRect.width = rect.width;
	dmgRect.height = height;

	laLayer_AddDamageRect(layer, &dmgRect, LA_TRUE);

	// right line
	dmgRect.x = rect.x + rect.width - width;
	dmgRect.y = rect.y;
	dmgRect.width = width;
	dmgRect.height = rect.height;

	laLayer_AddDamageRect(layer, &dmgRect, LA_TRUE);

	// bottom line
	dmgRect.x = rect.x;
	dmgRect.y = rect.y + rect.height - height;
	dmgRect.width = rect.width;
	dmgRect.height = height;

	laLayer_AddDamageRect(layer, &dmgRect, LA_TRUE);
}