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


#include "gfx/libaria/inc/libaria_widget_drawsurface.h"

#if LA_DRAWSURFACE_WIDGET_ENABLED

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
    DRAW_CALLBACK,
    DRAW_BORDER,
};

static void drawBackground(laDrawSurfaceWidget* sfc);
static void drawCallback(laDrawSurfaceWidget* sfc);
static void drawBorder(laDrawSurfaceWidget* sfc);

static void nextState(laDrawSurfaceWidget* sfc)
{
    switch(sfc->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(sfc->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                sfc->widget.drawState = DRAW_BACKGROUND;
                sfc->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            sfc->widget.drawState = DRAW_CALLBACK;
            sfc->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawCallback;
            
            return;
        }
        case DRAW_CALLBACK:
        {            
            if(sfc->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                sfc->widget.drawState = DRAW_BORDER;
                sfc->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            sfc->widget.drawState = DONE;
            sfc->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laDrawSurfaceWidget* sfc)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)sfc);

    nextState(sfc);
}

static void drawCallback(laDrawSurfaceWidget* sfc)
{
    GFX_Rect rect;
    
    // paint callback
    if(sfc->cb != NULL)
    {
        rect = laUtils_WidgetLayerRect((laWidget*)sfc);
        
        if(sfc->cb(sfc, &rect) == LA_FALSE)
            return;
    }
    
    nextState(sfc);
}

static void drawBorder(laDrawSurfaceWidget* sfc)
{
    if(sfc->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)sfc);
    else if(sfc->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)sfc);
        
    nextState(sfc);
}

void _laDrawSurfaceWidget_Paint(laDrawSurfaceWidget* sfc)
{
    laContext* context = laContext_GetActive();
    
    if(sfc->widget.scheme == NULL)
    {
        sfc->widget.drawState = DONE;
        
        return;
    }
    
    if(sfc->widget.drawState == NOT_STARTED)
        nextState(sfc);
    
    while(sfc->widget.drawState != DONE)
    {
        sfc->widget.drawFunc((laWidget*)sfc);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_DRAWSURFACE_WIDGET_ENABLED