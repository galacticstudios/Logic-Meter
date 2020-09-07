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


#include "gfx/libaria/inc/libaria_widget_keypad.h"

#if LA_KEYPAD_WIDGET_ENABLED && LA_BUTTON_WIDGET_ENABLED

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

void _laKeyPadWidget_InvalidateBorderAreas(laKeyPadWidget* pad)
{
    laWidget_Invalidate((laWidget*)pad);
}

static void drawBackground(laKeyPadWidget* pad);
static void drawBorder(laKeyPadWidget* pad);

static void nextState(laKeyPadWidget* pad)
{
    switch(pad->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(pad->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                pad->widget.drawState = DRAW_BACKGROUND;
                pad->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            if(pad->widget.borderType != LA_WIDGET_BORDER_NONE)
            {
                pad->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBorder;
                pad->widget.drawState = DRAW_BORDER;
                
                return;
            }
        }
        case DRAW_BORDER:
        {
            pad->widget.drawState = DONE;
            pad->widget.drawFunc = NULL;
        }
    }
}

static void drawBackground(laKeyPadWidget* pad)
{
    laWidget_SkinClassic_DrawStandardBackground((laWidget*)pad);
    
    nextState(pad);
}

static void drawBorder(laKeyPadWidget* pad)
{
    if(pad->widget.borderType == LA_WIDGET_BORDER_LINE)
        laWidget_SkinClassic_DrawStandardLineBorder((laWidget*)pad);
    else if(pad->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        laWidget_SkinClassic_DrawStandardRaisedBorder((laWidget*)pad);
    
    nextState(pad);
}

void _laKeyPadWidget_Paint(laKeyPadWidget* pad)
{
    laWidget* child;
    uint32_t i;
    
    laContext* context = laContext_GetActive();
    
    // hack to sync child attributes to key pad style
    for(i = 0; i < pad->widget.children.size; i++)
    {
        child = pad->widget.children.values[i];
        
        child->borderType = pad->widget.borderType;
        child->scheme = pad->widget.scheme;
    }
    
    if(pad->widget.scheme == NULL)
    {
        pad->widget.drawState = DONE;
        
        return;
    }
    
    if(pad->widget.drawState == NOT_STARTED)
        nextState(pad);
    
    while(pad->widget.drawState != DONE)
    {
        pad->widget.drawFunc((laWidget*)pad);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_WIDGET_KEYPAD_ENABLED && LA_WIDGET_BUTTON_ENABLED