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


#include "gfx/libaria/inc/libaria_widget_checkbox.h"

#if LA_CHECKBOX_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH    100
#define DEFAULT_HEIGHT   23

#define DEFAULT_IMAGE_MARGIN    10

static void invalidateImageRect(laCheckBoxWidget* cbox)
{
    GFX_Rect imgRect, imgSrcRect;
    
    _laCheckBoxWidget_GetImageRect(cbox, &imgRect, &imgSrcRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)cbox),
		                  &imgRect,
					      LA_FALSE);
}

static void invalidateTextRect(laCheckBoxWidget* cbox)
{
    GFX_Rect textRect, drawRect;
    
    _laCheckBoxWidget_GetTextRect(cbox, &textRect, &drawRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)cbox),
		                                   &drawRect,
								           LA_FALSE);
}

static void invalidateContents(laCheckBoxWidget* cbox)
{
    if(laString_IsEmpty(&cbox->text) == LA_FALSE)
        invalidateTextRect(cbox);
        
    invalidateImageRect(cbox);
}

static void languageChanging(laCheckBoxWidget* cbox)
{
    if(laString_IsEmpty(&cbox->text) == LA_FALSE)
        invalidateTextRect(cbox);
}

void _laCheckBoxWidget_Constructor(laCheckBoxWidget* cbox)
{
    _laWidget_Constructor((laWidget*)cbox);

    cbox->widget.destructor = (laWidget_Destructor_FnPtr)&_laCheckBoxWidget_Destructor;

    cbox->widget.type = LA_WIDGET_CHECKBOX;

    // override base class methods
    cbox->widget.paint = (laWidget_Paint_FnPtr)&_laCheckBoxWidget_Paint;

    cbox->widget.languageChangeEvent = (laWidget_LanguageChangingEvent_FnPtr)&languageChanging;

    cbox->widget.rect.width = DEFAULT_WIDTH;
    cbox->widget.rect.height = DEFAULT_HEIGHT;

    cbox->checked = LA_FALSE;

    cbox->widget.borderType = LA_WIDGET_BORDER_NONE;

    laString_Initialize(&cbox->text);

    cbox->imageMargin = DEFAULT_IMAGE_MARGIN;

    cbox->halign = LA_HALIGN_LEFT;
    cbox->valign = LA_VALIGN_MIDDLE;
    
    cbox->checkedImage = NULL;
    cbox->uncheckedImage = NULL;
    
    cbox->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&_laCheckBoxWidget_TouchDownEvent;
    cbox->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&_laCheckBoxWidget_TouchUpEvent;
    cbox->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&_laCheckBoxWidget_TouchMovedEvent;
}

void _laCheckBoxWidget_Destructor(laCheckBoxWidget* cbox)
{
    laString_Destroy(&cbox->text);
    
    // free any existing memory reader
    if(cbox->reader != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(cbox->reader);
        
        cbox->reader = NULL;
    }

    _laWidget_Destructor((laWidget*)cbox);
}

laCheckBoxWidget* laCheckBoxWidget_New()
{
    laCheckBoxWidget* cbox = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    cbox = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laCheckBoxWidget));

    if(cbox == NULL)
        return NULL;
    
    _laCheckBoxWidget_Constructor(cbox);

    return cbox;
}

laBool laCheckBoxWidget_GetChecked(laCheckBoxWidget* cbox)
{
    if(cbox == NULL)
        return LA_FALSE;

    return cbox->checked;
}

laResult laCheckBoxWidget_SetChecked(laCheckBoxWidget* cbox, laBool checked)
{
    if(cbox == NULL)
        return LA_FAILURE;
        
    if(cbox->checked == checked)
        return LA_SUCCESS;
        
    cbox->checked = checked;
    
    if (cbox->checked == LA_TRUE && cbox->checkedEvent != NULL)
        cbox->checkedEvent(cbox);
    else if (cbox->checked == LA_FALSE && cbox->uncheckedEvent != NULL)
        cbox->uncheckedEvent(cbox);
    
    invalidateImageRect(cbox);
    
    return LA_SUCCESS;
}

laHAlignment laCheckBoxWidget_GetHAlignment(laCheckBoxWidget* cbox)
{
    if(cbox == NULL)
        return 0;

    return cbox->halign;
}

laResult laCheckBoxWidget_SetHAlignment(laCheckBoxWidget* cbox,
                                        laHAlignment align)
{
    if(cbox == NULL)
        return LA_FAILURE;

    if(cbox->halign == align)
        return LA_SUCCESS;

    invalidateContents(cbox);

    cbox->halign = align;

    invalidateContents(cbox);
    
    return LA_SUCCESS;
}

laVAlignment laCheckBoxWidget_GetVAlignment(laCheckBoxWidget* cbox)
{
    if(cbox == NULL)
        return 0;

    return cbox->valign;
}

laResult laCheckBoxWidget_SetVAlignment(laCheckBoxWidget* cbox,
                                        laVAlignment align)
{
    if(cbox == NULL || cbox->valign == align)
        return LA_FAILURE;
        
    if(cbox->valign == align)
        return LA_SUCCESS;
    
    invalidateContents(cbox);

    cbox->valign = align;

    invalidateContents(cbox);
    
    return LA_SUCCESS;
}

laResult laCheckBoxWidget_GetText(laCheckBoxWidget* cbox, laString* str)
{
    if(cbox == NULL)
        return LA_FAILURE;
    
    return laString_Copy(str, &cbox->text);
}

laResult laCheckBoxWidget_SetText(laCheckBoxWidget* cbox, laString str)
{
    if(cbox == NULL)
        return LA_FAILURE;
        
    if(laString_Compare(&cbox->text, &str) == 0)
        return LA_SUCCESS;
           
    invalidateContents(cbox);
                
    laString_Copy(&cbox->text, &str);
    
    invalidateContents(cbox);
    
    return LA_SUCCESS;
}

GFXU_ImageAsset* laCheckBoxWidget_GetCheckedImage(laCheckBoxWidget* cbox)
{
    if(cbox == NULL)
        return NULL;

    return cbox->checkedImage;
}

laResult laCheckBoxWidget_SetCheckedImage(laCheckBoxWidget* cbox,
                                          GFXU_ImageAsset* img)
{
    if(cbox == NULL)
        return LA_FAILURE;

    if(cbox->checkedImage == img)
        return LA_SUCCESS;

    invalidateContents(cbox);
    
    cbox->checkedImage = img;

    invalidateContents(cbox);
    
    return LA_SUCCESS;
}

GFXU_ImageAsset* laCheckBoxWidget_GetUncheckedImage(laCheckBoxWidget* cbox)
{
    if(cbox == NULL)
        return NULL;

    return cbox->uncheckedImage;
}

laResult laCheckBoxWidget_SetUncheckedImage(laCheckBoxWidget* cbox,
                                            GFXU_ImageAsset* img)
{
    if(cbox == NULL)
        return LA_FAILURE;

    if(cbox->uncheckedImage == img)
        return LA_SUCCESS;

    invalidateContents(cbox);

    cbox->uncheckedImage = img;

    invalidateContents(cbox);
    
    return LA_SUCCESS;
}

laRelativePosition laCheckBoxWidget_GetImagePosition(laCheckBoxWidget* cbox)
{
    if(cbox == NULL)
        return LA_RELATIVE_POSITION_LEFTOF;
        
    return cbox->imagePosition;
}

laResult laCheckBoxWidget_SetImagePosition(laCheckBoxWidget* cbox,
                                           laRelativePosition pos)
{
    if(cbox == NULL || cbox->imagePosition == pos)
        return LA_FAILURE;
    
    invalidateContents(cbox);
    
    cbox->imagePosition = pos;
    
    invalidateContents(cbox);
    
    return LA_SUCCESS;
}

uint32_t laCheckBoxWidget_GetImageMargin(laCheckBoxWidget* cbox)
{
    if(cbox == NULL)
        return LA_RELATIVE_POSITION_LEFTOF;
        
    return cbox->imageMargin;
}

laResult laCheckBoxWidget_SetImageMargin(laCheckBoxWidget* cbox, uint32_t mg)
{
    if(cbox == NULL || cbox->imageMargin == mg)
        return LA_FAILURE;
    
    invalidateContents(cbox);
    
    cbox->imageMargin = mg;
    
    invalidateContents(cbox);
    
    return LA_SUCCESS;
}

laCheckBoxWidget_CheckedEvent laCheckBoxWidget_GetCheckedEventCallback(laCheckBoxWidget* cbox)
{
    if(cbox == NULL)
        return NULL;

    return cbox->checkedEvent;
}

laResult laCheckBoxWidget_SetCheckedEventCallback(laCheckBoxWidget* cbox,
                                                  laCheckBoxWidget_CheckedEvent cb)
{
    if(cbox == NULL)
        return LA_FAILURE;

    cbox->checkedEvent = cb;
    
    return LA_SUCCESS;
}

laCheckBoxWidget_UncheckedEvent laCheckBoxWidget_GetUncheckedEventCallback(laCheckBoxWidget* cbox)
{
    if(cbox == NULL)
        return NULL;

    return cbox->uncheckedEvent;
}

laResult laCheckBoxWidget_SetUncheckedEventCallback(laCheckBoxWidget* cbox,
                                                    laCheckBoxWidget_UncheckedEvent cb)
{
    if(cbox == NULL)
        return LA_FAILURE;

    cbox->uncheckedEvent = cb;
    
    return LA_SUCCESS;
}

void _laCheckBoxWidget_TouchDownEvent(laCheckBoxWidget* cbox,
                                      laInput_TouchDownEvent* evt)
{
    GFX_Rect imgRect, imgSrcRect;
    GFX_Point pt;
    
    _laCheckBoxWidget_GetImageRect(cbox, &imgRect, &imgSrcRect);
    
    pt.x = evt->x;
    pt.y = evt->y;
    
    if(GFX_RectContainsPoint(&imgRect, &pt) == GFX_TRUE)
    {
        evt->event.accepted = LA_TRUE;

       // printf("cbox touch down\n");
    }
}

void _laCheckBoxWidget_TouchUpEvent(laCheckBoxWidget* cbox,
                                    laInput_TouchUpEvent* evt)
{
    GFX_Rect imgRect, imgSrcRect;
    GFX_Point pnt;
    laLayer* layer;

    pnt.x = evt->x;
    pnt.y = evt->y;

    evt->event.accepted = LA_TRUE;

    _laCheckBoxWidget_GetImageRect(cbox, &imgRect, &imgSrcRect);
    
    if(GFX_RectContainsPoint(&imgRect, &pnt) == GFX_TRUE)
    {
        cbox->checked = !cbox->checked;

        if(cbox->checked == LA_TRUE && cbox->checkedEvent != NULL)
            cbox->checkedEvent(cbox);
        else if(cbox->checked == LA_FALSE && cbox->uncheckedEvent != NULL)
            cbox->uncheckedEvent(cbox);

        //printf("cbox released\n");
        
        layer = laUtils_GetLayer((laWidget*)cbox);
        
        laLayer_AddDamageRect(layer, &imgRect, LA_FALSE);
        
        //laWidget_Invalidate((laWidget*)cbox);
    }
    
    //printf("cbox touch up\n");
}

void _laCheckBoxWidget_TouchMovedEvent(laCheckBoxWidget* cbox,
                                       laInput_TouchMovedEvent* evt)
{
    evt->event.accepted = LA_TRUE;
}

#endif // LA_CHECKBOX_WIDGET_ENABLED