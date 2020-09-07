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


#include "gfx/libaria/inc/libaria_widget_window.h"

#if LA_WINDOW_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          100

#define DEFAULT_ICON_MARGIN     6

static void invalidateTitleBar(laWindowWidget* win)
{
    GFX_Rect barRect;
    
    laWindowWidget_GetTitleBarRect(win, &barRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)win),
                          &barRect,
                          GFX_FALSE);
}

static void invalidateTitleText(laWindowWidget* win)
{
    GFX_Rect textRect, drawRect;
    
    laWindowWidget_GetTextRect(win, &textRect, &drawRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)win),
                          &drawRect,
                          GFX_FALSE);
}

static void languageChanging(laWindowWidget* win)
{
    if(laString_IsEmpty(&win->title) == LA_FALSE)
        invalidateTitleText(win);
}

void _laWindowWidget_Constructor(laWindowWidget* win)
{
    _laWidget_Constructor((laWidget*)win);
    
    win->widget.destructor = (laWidget_Destructor_FnPtr)&_laWindowWidget_Destructor;

    win->widget.languageChangeEvent = (laWidget_LanguageChangingEvent_FnPtr)&languageChanging;

    win->widget.type = LA_WIDGET_WINDOW;

    // override base class methods
    win->widget.paint = (laWidget_Paint_FnPtr)&_laWindowWidget_Paint;

    win->widget.rect.width = DEFAULT_WIDTH;
    win->widget.rect.height = DEFAULT_HEIGHT;

    win->iconMargin = DEFAULT_ICON_MARGIN;

    win->widget.borderType = LA_WIDGET_BORDER_BEVEL;

    laString_Initialize(&win->title);
}

void _laWindowWidget_Destructor(laWindowWidget* win)
{
    laString_Destroy(&win->title);

    // free any existing memory reader
    if(win->reader != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(win->reader);
        
        win->reader = NULL;
    }

    _laWidget_Destructor((laWidget*)win);
}

laWindowWidget* laWindowWidget_New()
{
    laWindowWidget* win = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    win = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laWindowWidget));

    _laWindowWidget_Constructor(win);

    return win;
}

laResult laWindowWidget_GetTitle(laWindowWidget* win, laString* str)
{
    if(win == NULL)
        return LA_FAILURE;
    
    return laString_Copy(str, &win->title);
}

laResult laWindowWidget_SetTitle(laWindowWidget* win, laString str)
{
    if(win == NULL)
        return LA_FAILURE;
        
    if(laString_Compare(&win->title, &str) == 0)
        return LA_FAILURE;
    
    invalidateTitleText(win);
    
    laString_Copy(&win->title, &str);
    
    invalidateTitleText(win);
    
    return LA_SUCCESS;
}

GFXU_ImageAsset* laWindowWidget_GetIcon(laWindowWidget* win)
{
    if(win == NULL)
        return NULL;
        
    return win->icon;
}
                                                              
laResult laWindowWidget_SetIcon(laWindowWidget* win, GFXU_ImageAsset* img)
{
    if(win == NULL)
        return LA_FAILURE;
        
    if(win->icon == img)
        return LA_SUCCESS;
        
    win->icon = img;
    
    invalidateTitleBar(win);
    
    return LA_SUCCESS;
}

uint32_t laWindowWidget_GetIconMargin(laWindowWidget* win)
{
    if(win == NULL)
        return 0;

    return win->iconMargin;
}
                                               
laResult laWindowWidget_SetIconMargin(laWindowWidget* win, uint32_t mg)
{
    if(win == NULL)
        return LA_FAILURE;
        
    if(win->iconMargin == mg)
        return LA_SUCCESS;

    win->iconMargin = mg;

    invalidateTitleBar(win);
    
    return LA_SUCCESS;
}

#endif // LA_WINDOW_WIDGET_ENABLED