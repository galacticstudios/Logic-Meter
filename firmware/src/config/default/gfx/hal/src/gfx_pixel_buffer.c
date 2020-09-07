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


#include "gfx/hal/inc/gfx_pixel_buffer.h"

#include "gfx/hal/inc/gfx_color.h"
#include "gfx/hal/inc/gfx_rect.h"

static GFX_Color indexGet1bpp(const GFX_PixelBuffer* const buffer,
                              const uint32_t idx)
{
    uint32_t offs;
    uint8_t byte;
    GFX_Color color;
    
    offs = (idx % 8);
            
    byte = *(((uint8_t*)buffer->pixels) + (idx >> 3));
    
    color = byte & (0x80 >> offs);
    
    color >>= 7 - offs;
    
    return color;
}

static GFX_Color indexGet4bpp(const GFX_PixelBuffer* const buffer,
                              const uint32_t idx)
{
    uint32_t offs;
    uint8_t byte;
    GFX_Color color;
    
    byte = *(((uint8_t*)buffer->pixels) + (idx >> 1));
    
    if(idx % 2 > 0)
    {
        offs = 0xF;
        color = byte & offs;
    }
    else
    {
        offs = 0xF0;
        color = (byte & offs) >> 0x4;
    }
    
    return color;
}

static GFX_Color indexGet(const GFX_PixelBuffer* const buffer,
                          const uint32_t idx)
{
    uint8_t* buf_ptr;
    GFX_Color color = 0;
    uint32_t i;
    
    buf_ptr = ((uint8_t*)buffer->pixels) + (GFX_ColorInfo[buffer->mode].bpp >> 3) * idx;
    
    for(i = 0; i < GFX_ColorInfo[buffer->mode].bpp >> 3; i++)
        ((uint8_t*)&color)[i] = buf_ptr[i];
    
    return color;
}
                               
typedef GFX_Color (*PixelBufferIndexGet_FnPtr)(const GFX_PixelBuffer* const,
                                               const uint32_t);
                                               
PixelBufferIndexGet_FnPtr indexGet_FnTable[] =
{
    &indexGet1bpp,
    &indexGet4bpp,
    &indexGet,
    &indexGet,
    &indexGet,
    &indexGet
};

typedef GFX_Color (*GetPixel_FnPtr)(uint8_t*);

static GFX_Color get1byte(uint8_t* ptr)
{
    GFX_Color clr = ptr[0];
    
    return clr;
}

static GFX_Color get2byte(uint8_t* ptr)
{
    GFX_Color clr = ((uint16_t*)ptr)[0];
    
    return clr;
}

static GFX_Color get3byte(uint8_t* ptr)
{
    GFX_Color clr;
    
    clr = ptr[0];
    clr |= ptr[1] << 8;
    clr |= ptr[2] << 16;
    
    return clr;
}

static GFX_Color get4byte(uint8_t* ptr)
{
    GFX_Color clr = ((uint32_t*)ptr)[0];
    
    return clr;
}

GetPixel_FnPtr pixelGet_FnTable[] =
{
    &get1byte,
    &get2byte,
    &get3byte,
    &get4byte
};

typedef GFX_Result (*SetPixel_FnPtr)(uint8_t*, GFX_Color);

static GFX_Result set1byte(uint8_t* ptr, GFX_Color clr)
{
    ptr[0] = (uint8_t)clr;
    
    return GFX_SUCCESS;
}

static GFX_Result set2byte(uint8_t* ptr, GFX_Color clr)
{
    *((uint16_t*)ptr) = (uint16_t)clr;
    
    return GFX_SUCCESS;
}

static GFX_Result set3byte(uint8_t* ptr, GFX_Color clr)
{
    ptr[0] = (uint8_t)(clr & 0xFF);
    ptr[1] = (uint8_t)((clr & 0xFF00) >> 8);
    ptr[2] = (uint8_t)((clr & 0xFF0000) >> 16);
    
    return GFX_SUCCESS;
}

static GFX_Result set4byte(uint8_t* ptr, GFX_Color clr)
{
    *((uint32_t*)ptr) = clr;
    
    return GFX_SUCCESS;
}

SetPixel_FnPtr pixelSet_FnTable[] =
{
    &set1byte,
    &set2byte,
    &set3byte,
    &set4byte
};

GFX_Result GFX_PixelBufferCreate(const int32_t width,
                                 const int32_t height,
                                 const GFX_ColorMode mode,
                                 const void* const address,
                                 GFX_PixelBuffer* buffer)
{
    if(buffer == NULL)
        return GFX_FAILURE;

    buffer->size.width = width;
    buffer->size.height = height;
    buffer->pixel_count = width * height;

    buffer->mode = mode;

    switch(mode)
    {
        case GFX_COLOR_MODE_INDEX_1:
        {
            buffer->buffer_length = (width * height) / 8;
            
            if((width * height) % 8 > 0)
                buffer->buffer_length += 1;
                        
            break;
        }
        case GFX_COLOR_MODE_INDEX_4:
        {
            buffer->buffer_length = (width * height) / 2;
            
            if((width * height) % 2 > 0)
                buffer->buffer_length += 1;
        
            break;
        }
        case GFX_COLOR_MODE_INDEX_8:
        {
            buffer->buffer_length = (width * height);
        
            break;
        }
        default:
        {
            buffer->buffer_length = width * height * ((GFX_ColorInfo[buffer->mode].bpp >> 3));
        }
    }

    buffer->pixels = (void*)address;

    return GFX_SUCCESS;
}

GFX_Result GFX_PixelBufferDestroy(GFX_PixelBuffer* const buffer,
                                  GFX_MemoryIntf* mem_intf)
{
    if(buffer->pixels != GFX_NULL)
    {
        mem_intf->free(buffer->pixels);
        
        buffer->pixels = GFX_NULL;
    }
    
    mem_intf->memset(buffer, 0, sizeof(GFX_PixelBuffer));
    
    return GFX_SUCCESS;
}

GFX_Result GFX_PixelBufferCopy(const GFX_PixelBuffer* const buffer,
                               GFX_MemoryIntf* mem_intf,
                               GFX_PixelBuffer* result)
{
    if(buffer == NULL || result == NULL)
        return GFX_FAILURE;
        
    // create destination buffer
    GFX_PixelBufferCreate(buffer->size.width,
                          buffer->size.height,
                          buffer->mode,
                          NULL,
                          result);

    if(buffer->pixels == NULL)
        return GFX_SUCCESS;

    // allocate memory
    result->pixels = mem_intf->malloc(result->buffer_length);
    
    if(result->pixels == NULL)
        return GFX_FAILURE;

    // perform bulk buffer copy
    mem_intf->memcpy(result->pixels, buffer->pixels, buffer->buffer_length);

    return GFX_SUCCESS;
}

GFX_Buffer GFX_PixelBufferOffsetGet(const GFX_PixelBuffer* const buffer,
                                    const GFX_Point* const p)
{
    uint8_t* buf_ptr;

    if(buffer == NULL)
        return NULL;

    buf_ptr = (uint8_t*)buffer->pixels;
    
    if(p->x * p->y >= buffer->pixel_count)
        return NULL;
    
    return buf_ptr +
           GFX_ColorInfo[buffer->mode].size *
           (p->x + (p->y * buffer->size.width));
}

GFX_Buffer GFX_PixelBufferOffsetGet_Unsafe(const GFX_PixelBuffer* const buffer,
                                           const GFX_Point* const p)
{
    uint8_t* buf_ptr;

    buf_ptr = (uint8_t*)buffer->pixels;
    
    return buf_ptr +
           GFX_ColorInfo[buffer->mode].size *
           (p->x + (p->y * buffer->size.width));
}

GFX_Color GFX_PixelBufferGet(const GFX_PixelBuffer* const buffer,
                             const GFX_Point* const p)
{
    if(buffer == NULL || p->x * p->y >= buffer->pixel_count)
        return 0x0;
        
    return GFX_PixelBufferGet_Unsafe(buffer, p);
}

GFX_Color GFX_PixelBufferGet_Unsafe(const GFX_PixelBuffer* const buffer,
                                    const GFX_Point* const p)
{
    uint8_t* offs_ptr;
    GFX_Color color = 0;
    
    offs_ptr = (uint8_t*)GFX_PixelBufferOffsetGet(buffer, p);
    
    color = pixelGet_FnTable[GFX_ColorInfo[buffer->mode].size - 1](offs_ptr);
            
    return color;
}

GFX_Result GFX_PixelBufferAreaGet(const GFX_PixelBuffer* const buffer,
                                  const GFX_Rect* const rect,
                                  GFX_MemoryIntf* mem_intf,
                                  GFX_PixelBuffer* out)
{
    GFX_Rect buffer_rect;
    
    if(buffer == NULL || rect == NULL || out == NULL)
        return GFX_FAILURE;

    buffer_rect.x = 0;
    buffer_rect.y = 0;
    buffer_rect.width = buffer->size.width;
    buffer_rect.height = buffer->size.height;

    // nothing to do if the desired rectangle is not completely inside
    if(GFX_RectContainsRect(&buffer_rect, rect) == GFX_FALSE)
        return GFX_FAILURE;

    return GFX_PixelBufferAreaGet_Unsafe(buffer, rect, mem_intf, out);
}

GFX_Result GFX_PixelBufferAreaGet_Unsafe(const GFX_PixelBuffer* const buffer,
                                         const GFX_Rect* const rect,
                                         GFX_MemoryIntf* mem_intf,
                                         GFX_PixelBuffer* out)
{
    GFX_Buffer buf_ptr, dest_ptr;
    int32_t row;
    GFX_Point pnt, dest_pnt;
    
    GFX_PixelBufferDestroy(out, mem_intf);
    
    // prepare the pixel buffer result
    GFX_PixelBufferCreate(rect->width,
                          rect->height,
                          buffer->mode,
                          NULL,
                          out);
                           
    out->pixels = mem_intf->malloc(out->buffer_length);

    if(out->pixels == NULL)
        return GFX_FAILURE;

    pnt.x = rect->x;
    pnt.y = rect->y;
    
    dest_pnt.x = 0;
    
    // perform memory copy for each row
    for(row = 0; row < rect->height; row++)
    {
        pnt.y = rect->y + row;
        dest_pnt.y = row;
        
        buf_ptr = GFX_PixelBufferOffsetGet_Unsafe(buffer, &pnt);
        dest_ptr = GFX_PixelBufferOffsetGet_Unsafe(out, &dest_pnt);

        mem_intf->memcpy((void*)dest_ptr,
                         (void*)buf_ptr,
                         out->size.width * GFX_ColorInfo[buffer->mode].bpp >> 3);
    }
    
    return GFX_SUCCESS;
}

#if GFX_DRAW_PIPELINE_ENABLED

GFX_Result GFX_PixelBufferConvert(const GFX_PixelBuffer* const source,
                                  const GFX_ColorMode result_mode,
                                  GFX_MemoryIntf* mem_intf,
                                  GFX_PixelBuffer* result)
{
    GFX_Color color;
    GFX_Point pnt;
    
    if(source == NULL || result == NULL)
        return GFX_FAILURE;
     
    // color spaces are homogenous, use the fast copy method   
    if(source->mode == result_mode)
        return GFX_PixelBufferCopy(source, mem_intf, result);
        
    // create destination buffer
    GFX_PixelBufferCreate(source->size.width,
                          source->size.height,
                          result_mode,
                          NULL,
                          result);

    if(source->pixels == NULL)
        return GFX_SUCCESS;

    // allocate buffer memory
    result->pixels = mem_intf->malloc(result->buffer_length);
    
    if(result->pixels == NULL)
        return GFX_FAILURE;

    // perform pixel by pixel color conversion and copy
    for(pnt.y = 0; pnt.y < source->size.height; pnt.y++)
    {
        for(pnt.x = 0; pnt.x < source->size.width; pnt.x++)
        {            
            color = GFX_PixelBufferGet(source, &pnt);

            color = GFX_ColorConvert(source->mode, result_mode, color);
            
            GFX_PixelBufferSet(result, &pnt, color);
        }
    }

    return GFX_SUCCESS;
}

GFX_Result GFX_PixelBufferClipRect(const GFX_PixelBuffer* const buffer,
                                   const GFX_Rect* const rect,
                                   GFX_Rect* result)
{
    GFX_Rect buffer_rect;

    if(buffer == NULL || rect == NULL || result == NULL)
        return GFX_FAILURE;

    buffer_rect.x = 0;
    buffer_rect.y = 0;
    buffer_rect.width = buffer->size.width;
    buffer_rect.height = buffer->size.height;

    // can't clip if rectangles do not overlap in any way
    if(GFX_RectIntersects(&buffer_rect, rect) == GFX_FALSE)
        return GFX_FAILURE;

    // clip the rectangles
    GFX_RectClip(&buffer_rect, rect, result);

    return GFX_SUCCESS;
}

GFX_Color GFX_PixelBufferGetIndex(const GFX_PixelBuffer* const buffer,
                                  const int32_t idx)
{
    if(idx >= buffer->pixel_count)
        return 0;
        
    return indexGet_FnTable[GFX_ColorInfo[buffer->mode].bppOrdinal](buffer, idx);
}

GFX_Result GFX_PixelBufferSet(const GFX_PixelBuffer* const buffer,
                              const GFX_Point* const pnt,
                              GFX_Color color)
{
    if(buffer == NULL || pnt == NULL)
        return GFX_FAILURE;

    // cull the pixel if it falls outside the draw area
    if(pnt->x * pnt->y >= buffer->pixel_count)
        return GFX_FAILURE;
        
    // set the buffer pixel value
    GFX_PixelBufferSet_Unsafe(buffer, pnt, color);

    return GFX_SUCCESS;
}

GFX_Result GFX_PixelBufferSet_Unsafe(const GFX_PixelBuffer* const buffer,
                                     const GFX_Point* const pnt,
                                     GFX_Color color)
{
    uint8_t* dest_ptr;

    // get address of dest pixel
    dest_ptr = (uint8_t*)GFX_PixelBufferOffsetGet_Unsafe(buffer, pnt);
    
    pixelSet_FnTable[(GFX_ColorInfo[buffer->mode].bpp >> 3) - 1](dest_ptr, color);

    return GFX_SUCCESS;
}

GFX_Result GFX_PixelBufferAreaSet(const GFX_PixelBuffer* const source,
                                  const GFX_Rect* const source_rect,
                                  const GFX_PixelBuffer* const dest,
                                  const GFX_Point* const pnt,
                                  GFX_MemoryIntf* mem_intf)
{
    GFX_Rect source_clip_rect;
    GFX_Rect result_rect;
    GFX_Point actualPt;

    if(source == NULL || source_rect == NULL ||
       dest == NULL || pnt == NULL)
        return GFX_FAILURE;

    actualPt.x = pnt->x;
    actualPt.y = pnt->y;

    // clip the source rect to the source buffer
    if(GFX_PixelBufferClipRect(source,
                               source_rect,
                               &source_clip_rect) == GFX_FAILURE)
        return GFX_FAILURE;

    source_clip_rect.x += actualPt.x;
    source_clip_rect.y += actualPt.y;

    // clip the clipped source rect to the destination buffer
    if(GFX_PixelBufferClipRect(dest,
                               &source_clip_rect,
                               &result_rect) == GFX_FAILURE)
        return GFX_FAILURE;

    source_clip_rect = result_rect;

    source_clip_rect.x -= actualPt.x;
    source_clip_rect.y -= actualPt.y;

    if(pnt->x < 0)
        actualPt.x -= pnt->x;

    if(pnt->y < 0)
        actualPt.y -= pnt->y;

    // perform the buffer copy
    return GFX_PixelBufferAreaSet_Unsafe(source,
                                         &source_clip_rect,
                                         dest,
                                         &actualPt,
                                         mem_intf);
}

GFX_Result GFX_PixelBufferAreaSet_Unsafe(const GFX_PixelBuffer* const source,
                                         const GFX_Rect* const source_rect,
                                         const GFX_PixelBuffer* const dest,
                                         const GFX_Point* const pnt,
                                         GFX_MemoryIntf* mem_intf)
{
    GFX_Point source_point, dest_point;
    int32_t row, row_max, col, col_max;
    GFX_Buffer source_ptr, dest_ptr;
    GFX_Color color;

    // calculate minimums
    row_max = source_rect->height;// - source_rect->y;
    col_max = source_rect->width;// - source_rect->x;
    
    // simple memcpy blit operation
    if(source->mode == dest->mode)
    {
        // calculate x offsets
        source_point.x = source_rect->x;
        dest_point.x = pnt->x;
        
        for(row = 0; row < row_max; row++)
        {
            // calculate y offsets
            source_point.y = source_rect->y + row;
            dest_point.y = pnt->y + row;
            
            source_ptr = GFX_PixelBufferOffsetGet_Unsafe(source, &source_point);
            dest_ptr = GFX_PixelBufferOffsetGet_Unsafe(dest, &dest_point);
            
            mem_intf->memcpy(dest_ptr,source_ptr, col_max * (GFX_ColorInfo[dest->mode].bpp >> 3));
        }
    }
    // per-pixel blit operation
    else
    {
        for(row = 0; row < row_max; row++)
        {
            for(col = 0; col < col_max; col++)
            {
                // calculate source offset
                source_point.x = source_rect->x + col;
                source_point.y = source_rect->y + row;

                // get source color
                color = GFX_PixelBufferGet(source, &source_point);

                // calculate dest offset
                dest_point.x = pnt->x + col;
                dest_point.y = pnt->y + row;

                // set dest pixel
                GFX_PixelBufferSet_Unsafe(dest, &dest_point, color);
            }
        }
    }

    return GFX_SUCCESS;
}

GFX_Result GFX_PixelBufferAreaFill(const GFX_PixelBuffer* const buffer,
                                   const GFX_Rect* const rect,
                                   const GFX_Color color)
{
    GFX_Rect clip_rect;

    if(buffer == NULL || rect == NULL)
        return GFX_FAILURE;

    if(GFX_PixelBufferClipRect(buffer,
                               rect,
                               &clip_rect) == GFX_FAILURE)
        return GFX_FAILURE;

    // fill the buffer
    return GFX_PixelBufferAreaFill_Unsafe(buffer, &clip_rect, color);
}

GFX_Result GFX_PixelBufferAreaFill_Unsafe(const GFX_PixelBuffer* const buffer,
                                          const GFX_Rect* const rect,
                                          const GFX_Color color)
{
    int32_t row, row_max, col, col_max;
    GFX_Point pnt;

    // calculate minimums
    row_max = rect->height;
    col_max = rect->width;
    
    for(row = 0; row < row_max; row++)
    {
        pnt.y = rect->y + row;
        
        for(col = 0; col < col_max; col++)
        {
            pnt.x = rect->x + col;
            
            // set dest pixel
            GFX_PixelBufferSet_Unsafe(buffer, &pnt, color);
        }
    }

    return GFX_SUCCESS;
}

#endif // GFX_DRAW_PIPELINE_ENABLED