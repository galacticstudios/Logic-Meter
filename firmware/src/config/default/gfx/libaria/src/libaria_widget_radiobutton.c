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


#include "gfx/libaria/inc/libaria_widget_radiobutton.h"

#if LA_RADIOBUTTON_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_radiobutton_group.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          25

#define DEFAULT_IMAGE_MARGIN    10

#define DEFAULT_CIRCLE_SIZE     12

static void _laRadioButtonWidget_Destructor(laRadioButtonWidget* btn);

void _laRadioButtonWidget_Paint(laRadioButtonWidget* btn);

static void touchDown(laRadioButtonWidget* btn, laInput_TouchDownEvent* evt);
static void touchUp(laRadioButtonWidget* btn, laInput_TouchUpEvent* evt);
static void touchMoved(laRadioButtonWidget* btn, laInput_TouchMovedEvent* evt);

static void invalidateImageRect(laRadioButtonWidget* btn)
{
    GFX_Rect imgRect, imgSrcRect;
    
    _laRadioButtonWidget_GetImageRect(btn, &imgRect, &imgSrcRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)btn),
		                  &imgRect,
					      LA_FALSE);
}

static void invalidateTextRect(laRadioButtonWidget* btn)
{
    GFX_Rect textRect, drawRect;
    
    _laRadioButtonWidget_GetTextRect(btn, &textRect, &drawRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)btn),
		                                   &drawRect,
								           LA_FALSE);
}

static void invalidateContents(laRadioButtonWidget* btn)
{
    if(laString_IsEmpty(&btn->text) == LA_FALSE)
        invalidateTextRect(btn);
        
    invalidateImageRect(btn);
}

static void languageChanging(laRadioButtonWidget* btn)
{
    if(laString_IsEmpty(&btn->text) == LA_FALSE)
        invalidateContents(btn);
}

static void _laRadioButtonWidget_Constructor(laRadioButtonWidget* btn)
{
    _laWidget_Constructor((laWidget*)btn);
    
    btn->widget.destructor = (laWidget_Destructor_FnPtr)&_laRadioButtonWidget_Destructor;

    btn->widget.languageChangeEvent = (laWidget_LanguageChangingEvent_FnPtr)&languageChanging;

    btn->widget.type = LA_WIDGET_RADIOBUTTON;

    // override base class methods
    //btn->widget.update = &(laWidget_Update_FnPtr)_laRadioButtonWidget_Update;
    btn->widget.paint = (laWidget_Paint_FnPtr)&_laRadioButtonWidget_Paint;

    btn->widget.rect.width = DEFAULT_WIDTH;
    btn->widget.rect.height = DEFAULT_HEIGHT;

    btn->selected = LA_TRUE;

    btn->widget.borderType = LA_WIDGET_BORDER_NONE;

    laString_Initialize(&btn->text);
    
    btn->imageMargin = DEFAULT_IMAGE_MARGIN;

    btn->halign = LA_HALIGN_CENTER;
    btn->valign = LA_VALIGN_MIDDLE;
    
    btn->selectedImage = NULL;
    btn->unselectedImage = NULL;
    btn->circleButtonSize = DEFAULT_CIRCLE_SIZE;

    btn->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&touchDown;
    btn->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&touchUp;
    btn->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&touchMoved;
}

static void _laRadioButtonWidget_Destructor(laRadioButtonWidget* btn)
{
    if(btn->group != NULL)
        laRadioButtonGroup_RemoveButton(btn->group, btn);

    laString_Destroy(&btn->text);
    
    // free any existing memory reader
    if(btn->reader != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(btn->reader);
        
        btn->reader = NULL;
    }

    _laWidget_Destructor((laWidget*)btn);
}

/*laWidgetUpdateState _laRadioButtonWidget_Update(laRadioButtonWidget* btn)
{

}*/

laRadioButtonWidget* laRadioButtonWidget_New()
{
    laRadioButtonWidget* btn = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    btn = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laRadioButtonWidget));

    if(btn == NULL)
        return btn;
        
    _laRadioButtonWidget_Constructor(btn);

    return btn;
}

laRadioButtonGroup* laRadioButtonWidget_GetGroup(laRadioButtonWidget* btn)
{
    if(btn == NULL)
        return NULL;
        
    return btn->group;
}

laBool laRadioButtonWidget_GetSelected(laRadioButtonWidget* btn)
{
    if(btn == NULL)
        return LA_FALSE;

    if(btn->group == NULL)
        return LA_TRUE;

    return btn->group->selected == btn;
}

laResult laRadioButtonWidget_SetSelected(laRadioButtonWidget* btn)
{
    laResult res;
    
    if(btn == NULL || btn->selected == LA_TRUE)
        return LA_FAILURE;

    if(btn->group == NULL)
        return LA_SUCCESS;
   
    invalidateContents(btn);
    
    res = laRadioButtonGroup_SelectButton(btn->group, btn);
    
    if(res == GFX_TRUE)
        invalidateContents(btn);
    
    return res;
}

void _laRadioButtonWidget_Select(laRadioButtonWidget* btn)
{
    if(btn->selected == LA_TRUE)
        return;
    
    invalidateContents(btn);

    btn->selected = LA_TRUE;

    if(btn->selectedEvent != NULL)
        btn->selectedEvent(btn);
        
    invalidateContents(btn);
}

void _laRadioButtonWidget_Deselect(laRadioButtonWidget* btn)
{
    if(btn->selected == LA_FALSE)
        return;

    invalidateContents(btn);
    
    btn->selected = LA_FALSE;

    if(btn->deselectedEvent != NULL)
        btn->deselectedEvent(btn);
        
    invalidateContents(btn);
}

laHAlignment laRadioButtonWidget_GetHAlignment(laRadioButtonWidget* btn)
{
    if(btn == NULL)
        return 0;

    return btn->halign;
}

laResult laRadioButtonWidget_SetHAlignment(laRadioButtonWidget* btn,
                                           laHAlignment align)
{
    if(btn == NULL)
        return LA_FAILURE;

    if(btn->halign == align)
        return LA_SUCCESS;
        
    invalidateContents(btn);

    btn->halign = align;

    invalidateContents(btn);
    
    return LA_SUCCESS;
}

laVAlignment laRadioButtonWidget_GetVAlignment(laRadioButtonWidget* btn)
{
    if(btn == NULL)
        return 0;

    return btn->valign;
}

laResult laRadioButtonWidget_SetVAlignment(laRadioButtonWidget* btn,
                                           laVAlignment align)
{
    if(btn == NULL || btn->valign == align)
        return LA_FAILURE;

    if(btn->valign == align)
        return LA_SUCCESS;

    invalidateContents(btn);

    btn->valign = align;

    invalidateContents(btn);
    
    return LA_SUCCESS;
}

laResult laRadioButtonWidget_GetText(laRadioButtonWidget* btn, laString* str)
{
    if(btn == NULL)
        return LA_FAILURE;
    
    laString_Copy(str, &btn->text);
    
    return LA_SUCCESS;
}

laResult laRadioButtonWidget_SetText(laRadioButtonWidget* btn, laString str)
{
    if(btn == NULL)
        return LA_FAILURE;
        
    if(laString_Compare(&btn->text, &str) == 0)
        return LA_SUCCESS;
        
    invalidateContents(btn);
        
    laString_Copy(&btn->text, &str);
    
    invalidateContents(btn);
    
    return LA_SUCCESS;
}

GFXU_ImageAsset* laRadioButtonWidget_GetSelectedImage(laRadioButtonWidget* btn)
{
    if(btn == NULL)
        return NULL;

    return btn->selectedImage;
}

laResult laRadioButtonWidget_SetSelectedImage(laRadioButtonWidget* btn,
                                              GFXU_ImageAsset* img)
{
    if(btn == NULL)
        return LA_FAILURE;

    if(btn->selectedImage == img)
        return LA_SUCCESS;

    invalidateContents(btn);

    btn->selectedImage = img;

    invalidateContents(btn);
    
    return LA_SUCCESS;
}

GFXU_ImageAsset* laRadioButtonWidget_GetUnselectedImage(laRadioButtonWidget* btn)
{
    if(btn == NULL)
        return NULL;

    return btn->unselectedImage;
}

laResult laRadioButtonWidget_SetUnselectedImage(laRadioButtonWidget* btn,
                                                GFXU_ImageAsset* img)
{
    if(btn == NULL)
        return LA_FAILURE;
        
    if(btn->unselectedImage == img)
        return LA_SUCCESS;
        
    invalidateContents(btn);

    btn->unselectedImage = img;

    invalidateContents(btn);
    
    return LA_SUCCESS;
}

laRelativePosition laRadioButtonWidget_GetImagePosition(laRadioButtonWidget* btn)
{
    if(btn == NULL)
        return LA_RELATIVE_POSITION_LEFTOF;
        
    return btn->imagePosition;
}

laResult laRadioButtonWidget_SetImagePosition(laRadioButtonWidget* btn,
                                              laRelativePosition pos)
{
    if(btn == NULL)
        return LA_FAILURE;
    
    if(btn->imagePosition == pos)
        return LA_SUCCESS;
    
    invalidateContents(btn);
    
    btn->imagePosition = pos;
    
    invalidateContents(btn);
    
    return LA_SUCCESS;
}

uint32_t laRadioButtonWidget_GetImageMargin(laRadioButtonWidget* btn)
{
    if(btn == NULL)
        return LA_RELATIVE_POSITION_LEFTOF;
        
    return btn->imageMargin;
}

laResult laRadioButtonWidget_SetImageMargin(laRadioButtonWidget* btn, uint32_t mg)
{
    if(btn == NULL)
        return LA_FAILURE;
    
    if(btn->imageMargin == mg)
        return LA_SUCCESS;
        
    invalidateContents(btn);
    
    btn->imageMargin = mg;
    
    invalidateContents(btn);
    
    return LA_SUCCESS;
}

laRadioButtonWidget_SelectedEvent laRadioButtonWidget_GetSelectedEventCallback(laRadioButtonWidget* btn)
{
    if(btn == NULL)
        return NULL;

    return btn->selectedEvent;
}

laResult laRadioButtonWidget_SetSelectedEventCallback(laRadioButtonWidget* btn,
                                                      laRadioButtonWidget_SelectedEvent cb)
{
    if(btn == NULL || btn->selectedEvent == cb)
        return LA_FAILURE;

    btn->selectedEvent = cb;
    
    return LA_SUCCESS;
}

laRadioButtonWidget_DeselectedEvent laRadioButtonWidget_GetDeselectedEventCallback(laRadioButtonWidget* btn)
{
    if(btn == NULL)
        return NULL;

    return btn->deselectedEvent;
}

laResult laRadioButtonWidget_SetDeselectedEventCallback(laRadioButtonWidget* btn,
                                                        laRadioButtonWidget_DeselectedEvent cb)
{
    if(btn == NULL || btn->deselectedEvent == cb)
        return LA_FAILURE;

    btn->deselectedEvent = cb;
    
    return LA_SUCCESS;
}

uint32_t laRadioButtonWidget_GetCircleButtonSize(laRadioButtonWidget* btn)
{
    if(btn == NULL)
        return LA_FAILURE;
    
    return btn->circleButtonSize;
}

laResult laRadioButtonWidget_SetCircleButtonSize(laRadioButtonWidget* btn,
                                                 uint32_t size)
{
    if(btn == NULL || size < DEFAULT_CIRCLE_SIZE)
        return LA_FAILURE;
    
    if (size > btn->widget.rect.height)
        return LA_FAILURE;
    
    if (size > btn->widget.rect.width)
        return LA_FAILURE;

    if (size == btn->circleButtonSize)
        return LA_SUCCESS;
    
    btn->circleButtonSize = size;
    
    laWidget_Invalidate((laWidget*) btn);
    
    return LA_SUCCESS;
}

static void touchDown(laRadioButtonWidget* btn,
                      laInput_TouchDownEvent* evt)
{
    GFX_Rect imgRect, imgSrcRect, widgetRect, clipRect;
    GFX_Point pt;
    
    _laRadioButtonWidget_GetImageRect(btn, &imgRect, &imgSrcRect);
    
    widgetRect = laUtils_WidgetLayerRect((laWidget*)btn);
    
    GFX_RectClip(&imgRect, &widgetRect, &clipRect);
    
    pt.x = evt->x;
    pt.y = evt->y;
    
    if(GFX_RectContainsPoint(&clipRect, &pt) == GFX_TRUE &&
       btn->selected == LA_FALSE)
    {
        evt->event.accepted = LA_TRUE;
    }
}

static void touchUp(laRadioButtonWidget* btn,
                    laInput_TouchUpEvent* evt)
{
    GFX_Rect imgRect, imgSrcRect, widgetRect, clipRect;
    GFX_Point pt;
    
    _laRadioButtonWidget_GetImageRect(btn, &imgRect, &imgSrcRect);
    
    widgetRect = laUtils_WidgetLayerRect((laWidget*)btn);
    
    GFX_RectClip(&imgRect, &widgetRect, &clipRect);

    pt.x = evt->x;
    pt.y = evt->y;

    evt->event.accepted = LA_TRUE;

    if(GFX_RectContainsPoint(&clipRect, &pt) == GFX_TRUE)
        laRadioButtonGroup_SelectButton(btn->group, btn);
    
    //printf("cbox touch up\n");
}

static void touchMoved(laRadioButtonWidget* btn,
                       laInput_TouchMovedEvent* evt)
{
    evt->event.accepted = LA_TRUE;
}

#endif // LA_RADIOBUTTON_WIDGET_ENABLED