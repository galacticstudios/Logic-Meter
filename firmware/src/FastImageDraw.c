
// This overrides Aria's image drawing code. Aria draws one pixel at a time. We
// optimize it by drawing a line at a time.
//
// Enable this code by adding --wrap GFXU_DrawImageRLEInternal to the linker command line.

#include "definitions.h"
#include "gfx/hal/inc/gfx_context.h"
#include "gfx/utils/inc/gfxu_image_utils.h"
#include "gfx/driver/controller/ili9488/drv_gfx_ili9488_common.h"

static GFX_Result drawColorImage(GFXU_ImageAsset* img, int32_t src_x, int32_t src_y,
    int32_t src_width, int32_t src_height,
    int32_t dest_x, int32_t dest_y);

GFX_Result __real_GFXU_DrawImageRLEInternal(GFXU_ImageAsset* img,
                                     int32_t src_x,
                                     int32_t src_y,
                                     int32_t src_width,
                                     int32_t src_height,
                                     int32_t dest_x,
                                     int32_t dest_y);

GFX_Result __wrap_GFXU_DrawImageRLEInternal(GFXU_ImageAsset* img,
                                     int32_t src_x,
                                     int32_t src_y,
                                     int32_t src_width,
                                     int32_t src_height,
                                     int32_t dest_x,
                                     int32_t dest_y)
{
    switch(img->colorMode)
    {
        case GFX_COLOR_MODE_INDEX_1:
        case GFX_COLOR_MODE_INDEX_4:
        case GFX_COLOR_MODE_INDEX_8:
        case GFX_COLOR_MODE_RGBA_5551:
        case GFX_COLOR_MODE_RGBA_8888:
        case GFX_COLOR_MODE_ARGB_8888:
        {
            return __real_GFXU_DrawImageRLEInternal(img,
                                  src_x,
                                  src_y,
                                  src_width,
                                  src_height,
                                  dest_x,
                                  dest_y);
        }

        case GFX_COLOR_MODE_GS_8:
        case GFX_COLOR_MODE_RGB_332:
        case GFX_COLOR_MODE_RGB_565:
        case GFX_COLOR_MODE_RGB_888:
        case GFX_COLOR_MODE_YUV:
        {
            return drawColorImage(img,
                                  src_x,
                                  src_y,
                                  src_width,
                                  src_height,
                                  dest_x,
                                  dest_y);
        }
    };
        
    return GFX_FAILURE;
}


GFX_Result drawColorImage(GFXU_ImageAsset* img,
    int32_t src_x, int32_t src_y, int32_t src_width, int32_t src_height, int32_t dest_x, int32_t dest_y)
{
    GFX_Context* context = GFX_ActiveContext();
    GFX_Point dest_point;
    GFX_Color color;
    uint8_t pixelLine[src_width * 3], *pp;
    uint32_t lastBlock = 0;
    uint32_t lastOffset = 0;
    GFX_ColorMode layerMode;
    
    uint32_t idx;

    int32_t row;
    int32_t col;
    
    // Only support one color mode for now
    if (img->colorMode != GFX_COLOR_MODE_RGB_565) __builtin_software_breakpoint();
    
    dest_point.x = dest_x;
    dest_point.y = dest_y;
    
    if(GFX_Get(GFXF_COLOR_MODE, &layerMode) == GFX_FAILURE)
        return GFX_FAILURE;
       
    // enable mask color if necessary
    if((img->flags & GFXU_IMAGE_USE_MASK) > 0)
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_TRUE);
        GFX_Set(GFXF_DRAW_MASK_VALUE, img->mask);
    }
    else
    {
        GFX_Set(GFXF_DRAW_MASK_ENABLE, GFX_FALSE);
    }
    
    for(row = 0; row < src_height; row++)
    {
        pp = pixelLine;
        for(col = 0; col < src_width; col++)
        {
            idx = src_x + col + ((src_y + row) * img->width);
            
            // get the RLE data of the index
            color = (uint16_t) getRLEDataAtIndex(img->header.dataAddress,
                                      img->width * img->height,
                                      idx,
                                      &lastBlock,
                                      &lastOffset);
            *pp++ = ((color & 0xf800) >> 8) | 0x7; //R
            *pp++ = ((color & 0x07e0) >> 3 ) | 0x3; //G
            *pp++ = ((color & 0x001f) << 3) | 0x7; //B
        }

        ILI9488_Intf_WritePixels((ILI9488_DRV *) context->driver_data,
                dest_point.x, dest_point.y + row, (uint8_t *) pixelLine, src_width);
    }
    
    return GFX_SUCCESS;
}

