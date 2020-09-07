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


#include "gfx/libaria/inc/libaria_widget_progressbar.h"

#if LA_PROGRESSBAR_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          25

static void invalidateBar(laProgressBarWidget* bar,
                          int32_t oldVal,
                          int32_t newVal)
{
    GFX_Rect barRect;
    
    _laProgressBarWidget_GetBarRect(bar, oldVal, newVal, &barRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)bar),
                          &barRect,
                          GFX_FALSE);
}

void _laProgressBarWidget_Constructor(laProgressBarWidget* bar)
{
    _laWidget_Constructor((laWidget*)bar);
    
    bar->widget.destructor = (laWidget_Destructor_FnPtr)&_laProgressBarWidget_Destructor;

    bar->widget.type = LA_WIDGET_PROGRESSBAR;

    // override base class methods
    bar->widget.paint = (laWidget_Paint_FnPtr)&_laProgressBarWidget_Paint;

    bar->widget.rect.width = DEFAULT_WIDTH;
    bar->widget.rect.height = DEFAULT_HEIGHT;

    bar->widget.borderType = LA_WIDGET_BORDER_BEVEL;
    bar->widget.backgroundType = LA_WIDGET_BACKGROUND_FILL;
}

void _laProgressBarWidget_Destructor(laProgressBarWidget* bar)
{
    _laWidget_Destructor((laWidget*)bar);
}

laProgressBarWidget* laProgressBarWidget_New()
{
    laProgressBarWidget* bar = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    bar = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laProgressBarWidget));

    if(bar == NULL)
        return NULL;
    
    _laProgressBarWidget_Constructor(bar);

    return bar;
}

laProgressBarDirection laProgressBarWidget_GetDirection(laProgressBarWidget* bar)
{
    if(bar == NULL)
        return 0;
        
    return bar->direction;
}

laResult laProgressBarWidget_SetDirection(laProgressBarWidget* bar, laProgressBarDirection dir)
{
    if(bar == NULL)
        return LA_FAILURE;
        
    if(bar->direction == dir)
        return LA_SUCCESS;
        
    bar->direction = dir;
    
    laWidget_Invalidate((laWidget*)bar);
        
    return LA_SUCCESS;
}

uint32_t laProgressBarWidget_GetValue(laProgressBarWidget* bar)
{
    if(bar == NULL)
        return 0;
        
    return bar->value;
}

laResult laProgressBarWidget_SetValue(laProgressBarWidget* bar, uint32_t value)
{
    uint32_t oldVal;
    
    if(bar == NULL)
        return LA_FAILURE;
        
    if(bar->value == value)
        return LA_SUCCESS;
    
    if(value > 100)
        value = 100;
    
    oldVal = bar->value;
    
    bar->value = value;
    
    if(bar->cb != NULL)
        bar->cb(bar, value);
    
    invalidateBar(bar, oldVal, bar->value);
        
    return LA_SUCCESS;
}

laProgressBar_ValueChangedEventCallback laProgressBarWidget_GetValueChangedEventCallback(laProgressBar* bar)
{
    if(bar == NULL)
        return NULL;
        
    return bar->cb;
}

laResult laProgressBarWidget_SetValueChangedCallback(laProgressBar* bar, laProgressBar_ValueChangedEventCallback cb)
{
    if(bar == NULL)
        return LA_FAILURE;
        
    bar->cb = cb;
        
    return LA_SUCCESS;
}

#endif // LA_PROGRESSBAR_WIDGET_ENABLED