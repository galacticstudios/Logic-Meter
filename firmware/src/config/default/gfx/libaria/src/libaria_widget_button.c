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


#include "gfx/libaria/inc/libaria_widget_button.h"

#if LA_BUTTON_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          25

#define DEFAULT_IMAGE_MARGIN    10

#define DEFAULT_PRESSED_OFFSET  1

static void touchDown(laButtonWidget* btn, laInput_TouchDownEvent* evt);
static void touchUp(laButtonWidget* btn, laInput_TouchUpEvent* evt);
static void touchMoved(laButtonWidget* btn, laInput_TouchMovedEvent* evt);

static void invalidateImageRect(laButtonWidget* btn)
{
    GFX_Rect imgRect, imgSrcRect;
    
    _laButtonWidget_GetImageRect(btn, &imgRect, &imgSrcRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)btn),
		                  &imgRect,
						  LA_FALSE);
}

static void invalidateTextRect(laButtonWidget* btn)
{
    GFX_Rect textRect, drawRect;
    
    _laButtonWidget_GetTextRect(btn, &textRect, &drawRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)btn),
		                  &drawRect,
					      LA_FALSE);
}

static void invalidateContents(laButtonWidget* btn)
{
    if(laString_IsEmpty(&btn->text) == LA_FALSE)
        invalidateTextRect(btn);
        
    if((btn->state != LA_BUTTON_STATE_UP && btn->pressedImage != NULL) ||
       (btn->state == LA_BUTTON_STATE_UP && btn->releasedImage != NULL))
        invalidateImageRect(btn);
}

static void languageChanging(laButtonWidget* btn)
{
    if(laString_IsEmpty(&btn->text) == LA_FALSE)
        invalidateContents(btn);
}

void _laButtonWidget_Constructor(laButtonWidget* btn)
{
    _laWidget_Constructor((laWidget*)btn);

    btn->widget.destructor = (laWidget_Destructor_FnPtr)&_laButtonWidget_Destructor;

    btn->widget.languageChangeEvent = (laWidget_LanguageChangingEvent_FnPtr)&languageChanging;

    btn->widget.type = LA_WIDGET_BUTTON;

    // override base class methods
    btn->widget.paint = (laWidget_Paint_FnPtr)&_laButtonWidget_Paint;

    btn->widget.rect.width = DEFAULT_WIDTH;
    btn->widget.rect.height = DEFAULT_HEIGHT;

    btn->state = LA_BUTTON_STATE_UP;
    btn->toggleable = LA_FALSE;

    btn->widget.borderType = LA_WIDGET_BORDER_BEVEL;

    laString_Initialize(&btn->text);
    btn->textLineSpace = -1;
    
    btn->imageMargin = DEFAULT_IMAGE_MARGIN;
    btn->pressedOffset = DEFAULT_PRESSED_OFFSET;

    btn->halign = LA_HALIGN_CENTER;
    btn->valign = LA_VALIGN_MIDDLE;
    
    btn->pressedImage = NULL;
    btn->releasedImage = NULL;

    btn->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&touchDown;
    btn->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&touchUp;
    btn->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&touchMoved;
    
    btn->widget.invalidateBorderAreas = (laWidget_InvalidateBorderAreas_FnPtr)&_laButtonWidget_InvalidateBorderAreas;
}

void _laButtonWidget_Destructor(laButtonWidget* btn)
{
    laString_Destroy(&btn->text);
    
    // free any existing memory reader
    if(btn->reader != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(btn->reader);
        
        btn->reader = NULL;
    }
    
    if (btn->drawObj.lineRect != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(btn->drawObj.lineRect);
        btn->drawObj.lineRect = NULL;
    }
        
    if (btn->drawObj.offset != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(btn->drawObj.offset);
        btn->drawObj.offset = NULL;
    }    

    _laWidget_Destructor((laWidget*)btn);
}

laButtonWidget* laButtonWidget_New()
{
    laButtonWidget* btn = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    btn = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laButtonWidget));
    
    _laButtonWidget_Constructor(btn);

    return btn;
}

laBool laButtonWidget_GetToggleable(laButtonWidget* btn)
{
    if(btn == NULL)
        return LA_FALSE;

    return btn->toggleable;
}

laResult laButtonWidget_SetToggleable(laButtonWidget* btn, laBool toggleable)
{
    if(btn == NULL)
        return LA_FAILURE;

    btn->toggleable = toggleable == 0 ? LA_FALSE : LA_TRUE;
    
    return LA_SUCCESS;
}

laBool laButtonWidget_GetPressed(laButtonWidget* btn)
{
    if(btn == NULL)
        return LA_FALSE;

    return btn->state == LA_BUTTON_STATE_DOWN || btn->state == LA_BUTTON_STATE_TOGGLED;
}

laResult laButtonWidget_SetPressed(laButtonWidget* btn, laBool pressed)
{
    laBool dirty = LA_FALSE;

    if(btn == NULL || (pressed == LA_TRUE && btn->state == LA_BUTTON_STATE_TOGGLED))
        return LA_FAILURE;
        
    if(pressed == LA_TRUE)
    {
        if(btn->pressedEvent != NULL)
            btn->pressedEvent(btn);
            
        // move to toggled state
        if(btn->toggleable == LA_TRUE)
        {
            btn->state = LA_BUTTON_STATE_TOGGLED;
        }
        // throw released event immediately
        else
        {
            if(btn->releasedEvent != NULL)
                btn->releasedEvent(btn);
        }
    }
    else
    {
        btn->state = LA_BUTTON_STATE_UP;
        
        if(btn->releasedEvent != NULL)
                btn->releasedEvent(btn);
    }
    
    // try to find a reason to redraw
    if(btn->pressedImage != NULL && btn->pressedImage != btn->releasedImage)
        dirty = LA_TRUE;
        
    if(btn->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        dirty = LA_TRUE;
        
    if(btn->pressedOffset != 0 &&
       (btn->pressedImage != NULL ||
        laString_IsEmpty(&btn->text) == LA_FALSE))
        dirty = LA_TRUE;
        
    if(dirty == LA_TRUE)
        laWidget_Invalidate((laWidget*)btn);
        
    return LA_SUCCESS;
}

laHAlignment laButtonWidget_GetHAlignment(laButtonWidget* btn)
{
    if(btn == NULL)
        return 0;

    return btn->halign;
}

laResult laButtonWidget_SetHAlignment(laButtonWidget* btn,
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

laVAlignment laButtonWidget_GetVAlignment(laButtonWidget* btn)
{
    if(btn == NULL)
        return 0;

    return btn->valign;
}

laResult laButtonWidget_SetVAlignment(laButtonWidget* btn,
                                      laVAlignment align)
{
    if(btn == NULL)
        return LA_FAILURE;

    if(btn->valign == align)
        return LA_SUCCESS;

    invalidateContents(btn);

    btn->valign = align;

    invalidateContents(btn);
    
    return LA_SUCCESS;
}

laResult laButtonWidget_GetText(laButtonWidget* btn, laString* str)
{
    if(btn == NULL)
        return LA_FAILURE;
    
    return laString_Copy(str, &btn->text);
}

laResult laButtonWidget_SetText(laButtonWidget* btn, laString str)
{
    if(btn == NULL)
        return LA_FAILURE;
        
    if(laString_Compare(&btn->text, &str) == 0)
        return LA_FAILURE;
        
    invalidateContents(btn);
        
    if(laString_Copy(&btn->text, &str) == LA_FAILURE)
        return LA_FAILURE;
    
    invalidateContents(btn);
    
    return LA_SUCCESS;
}


int32_t laButtonWidget_GetTextLineSpace(laButtonWidget* btn)
{
    if(btn == NULL)
        return LA_FAILURE;
    
    return btn->textLineSpace;
}

laResult laButtonWidget_SetTextLineSpace(laButtonWidget* btn, int32_t pixels)
{
    if(btn == NULL)
        return LA_FAILURE;
        
    if(btn->textLineSpace == pixels)
        return LA_SUCCESS;

    invalidateTextRect(btn);
    
    btn->textLineSpace = pixels;
    
    invalidateTextRect(btn);
    
    return LA_SUCCESS;
}

GFXU_ImageAsset* laButtonWidget_GetPressedImage(laButtonWidget* btn)
{
    if(btn == NULL)
        return NULL;

    return btn->pressedImage;
}

laResult laButtonWidget_SetPressedImage(laButtonWidget* btn,
                                        GFXU_ImageAsset* img)
{
    if(btn == NULL || btn->pressedImage == img)
        return LA_FAILURE;

    if(btn->pressedImage == img)
        return LA_SUCCESS;
        
    invalidateContents(btn);

    btn->pressedImage = img;

    invalidateContents(btn);
    
    return LA_SUCCESS;
}

GFXU_ImageAsset* laButtonWidget_GetReleasedImage(laButtonWidget* btn)
{
    if(btn == NULL)
        return NULL;

    return btn->releasedImage;
}

laResult laButtonWidget_SetReleasedImage(laButtonWidget* btn,
                                         GFXU_ImageAsset* img)
{
    if(btn == NULL || btn->releasedImage == img)
        return LA_FAILURE;

    if(btn->releasedImage == img)
        return LA_SUCCESS;

    invalidateContents(btn);

    btn->releasedImage = img;

    invalidateContents(btn);
    
    return LA_SUCCESS;
}

laRelativePosition laButtonWidget_GetImagePosition(laButtonWidget* btn)
{
    if(btn == NULL)
        return LA_RELATIVE_POSITION_LEFTOF;
        
    return btn->imagePosition;
}

laResult laButtonWidget_SetImagePosition(laButtonWidget* btn,
                                         laRelativePosition pos)
{
    if(btn == NULL || btn->imagePosition == pos)
        return LA_FAILURE;
    
    if(btn->imagePosition == pos)
        return LA_SUCCESS;
    
    invalidateContents(btn);
    
    btn->imagePosition = pos;
    
    invalidateContents(btn);
    
    return LA_SUCCESS;
}

uint32_t laButtonWidget_GetImageMargin(laButtonWidget* btn)
{
    if(btn == NULL)
        return LA_RELATIVE_POSITION_LEFTOF;
        
    return btn->imageMargin;
}

laResult laButtonWidget_SetImageMargin(laButtonWidget* btn, uint32_t mg)
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

int32_t laButtonWidget_GetPressedOffset(laButtonWidget* btn)
{
    if(btn == NULL)
        return 0;
        
    return btn->pressedOffset;
}

laResult laButtonWidget_SetPressedOffset(laButtonWidget* btn, int32_t offs)
{
    if(btn == NULL || btn->pressedOffset == offs)
        return LA_FAILURE;
        
    if(btn->pressedOffset == offs)
        return LA_SUCCESS;
        
    if(btn->state != LA_BUTTON_STATE_UP)
        invalidateContents(btn);
        
    btn->pressedOffset = offs;
    
    if(btn->state != LA_BUTTON_STATE_UP)
        invalidateContents(btn);
        
    return LA_SUCCESS;
}

laButtonWidget_PressedEvent laButtonWidget_GetPressedEventCallback(laButtonWidget* btn)
{
    if(btn == NULL)
        return NULL;

    return btn->pressedEvent;
}

laResult laButtonWidget_SetPressedEventCallback(laButtonWidget* btn,
                                                laButtonWidget_PressedEvent cb)
{
    if(btn == NULL || btn->pressedEvent == cb)
        return LA_FAILURE;

    btn->pressedEvent = cb;
    
    return LA_SUCCESS;
}

laButtonWidget_ReleasedEvent laButtonWidget_GetReleasedEventCallback(laButtonWidget* btn)
{
    if(btn == NULL)
        return NULL;

    return btn->releasedEvent;
}

laResult laButtonWidget_SetReleasedEventCallback(laButtonWidget* btn,
                                                 laButtonWidget_ReleasedEvent cb)
{
    if(btn == NULL || btn->releasedEvent == cb)
        return LA_FAILURE;

    btn->releasedEvent = cb;
    
    return LA_SUCCESS;
}

static void touchDown(laButtonWidget* btn, laInput_TouchDownEvent* evt)
{
    laBool dirty = LA_FALSE;
    
    evt->event.accepted = LA_TRUE;

    if(btn->toggleable == LA_TRUE)
    {
        if(btn->state != LA_BUTTON_STATE_TOGGLED)
        {
            btn->state = LA_BUTTON_STATE_DOWN;

            if(btn->pressedEvent != NULL)
                btn->pressedEvent(btn);
        }
    }
    else
    {
        btn->state = LA_BUTTON_STATE_DOWN;

        if(btn->pressedEvent != NULL)
            btn->pressedEvent(btn);
    }

    // try to find a reason to redraw
    if(btn->pressedImage != NULL && btn->pressedImage != btn->releasedImage)
        dirty = LA_TRUE;
        
    if(btn->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        dirty = LA_TRUE;
        
    if(btn->pressedOffset != 0 &&
       (btn->pressedImage != NULL ||
        laString_IsEmpty(&btn->text) == LA_FALSE))
        dirty = LA_TRUE;
        
    if(dirty == LA_TRUE)
        laWidget_Invalidate((laWidget*)btn);
        
    //printf("btn touch down\n");
}

static void touchUp(laButtonWidget* btn, laInput_TouchUpEvent* evt)
{
    GFX_Rect rect = laUtils_WidgetLocalRect((laWidget*)btn);
    GFX_Point pnt;
    
    laBool dirty = LA_FALSE;

    pnt.x = evt->x;
    pnt.y = evt->y;

    evt->event.accepted = LA_TRUE;

    laUtils_ClipRectToParent(btn->widget.parent, &rect);
    laUtils_RectToLayerSpace((laWidget*)btn, &rect);
    
    //Toggle buttons will not respond to touch outside the button area
    if ((GFX_RectContainsPoint(&rect, &pnt) == GFX_FALSE) && 
        (btn->toggleable == LA_TRUE))
    {
        return;
    }

    if(GFX_RectContainsPoint(&rect, &pnt) == GFX_TRUE)
    {
        if(btn->toggleable == LA_TRUE)
        {
            if(btn->state == LA_BUTTON_STATE_TOGGLED)
            {
                btn->state = LA_BUTTON_STATE_UP;

                if(btn->releasedEvent != NULL)
                    btn->releasedEvent(btn);

                //printf("btn released\n");
            }
            else
                btn->state = LA_BUTTON_STATE_TOGGLED;
        }
        else
        {
            btn->state = LA_BUTTON_STATE_UP;

            if(btn->releasedEvent != NULL)
                btn->releasedEvent(btn);

            //printf("btn released\n");
        }
    }
    else
    {
        if(btn->toggleable == LA_TRUE)
        {
            if(btn->state != LA_BUTTON_STATE_TOGGLED)
                btn->state = LA_BUTTON_STATE_UP;
        }
        else
        {
            btn->state = LA_BUTTON_STATE_UP;
        }
    }
    
    // try to find a reason to redraw
    if(btn->pressedImage != NULL && btn->pressedImage != btn->releasedImage)
        dirty = LA_TRUE;
        
    if(btn->widget.borderType == LA_WIDGET_BORDER_BEVEL)
        dirty = LA_TRUE;
        
    if(btn->pressedOffset != 0 &&
       (btn->pressedImage != NULL ||
        laString_IsEmpty(&btn->text) == LA_FALSE))
        dirty = LA_TRUE;
        
    if(dirty == LA_TRUE)
        laWidget_Invalidate((laWidget*)btn);
    
    //printf("btn touch up\n");
}

static void touchMoved(laButtonWidget* btn, laInput_TouchMovedEvent* evt)
{
    GFX_Rect rect = laUtils_WidgetLocalRect((laWidget*)btn);
    GFX_Point pnt;
    
    laBool dirty = LA_FALSE;

    pnt.x = evt->x;
    pnt.y = evt->y;

    evt->event.accepted = LA_TRUE;
    
    //Toggle buttons will not respond to touch outside the button area
    if ((GFX_RectContainsPoint(&rect, &pnt) == GFX_FALSE) && 
        (btn->toggleable == LA_TRUE))
    {
        return;
    }

    if(btn->state == LA_BUTTON_STATE_DOWN)
    {
        laUtils_ClipRectToParent(btn->widget.parent, &rect);
        laUtils_RectToLayerSpace((laWidget*)btn, &rect);

        if(GFX_RectContainsPoint(&rect, &pnt) == GFX_FALSE)
        {
            btn->state = LA_BUTTON_STATE_UP;
            
            // try to find a reason to redraw
            if(btn->pressedImage != NULL && btn->pressedImage != btn->releasedImage)
                dirty = LA_TRUE;
                
            if(btn->widget.borderType == LA_WIDGET_BORDER_BEVEL)
                dirty = LA_TRUE;
                
            if(btn->pressedOffset != 0 &&
               (btn->pressedImage != NULL ||
                laString_IsEmpty(&btn->text) == LA_FALSE))
                dirty = LA_TRUE;
                
            if(dirty == LA_TRUE)
                laWidget_Invalidate((laWidget*)btn);
        }
    }
    else if(btn->state == LA_BUTTON_STATE_UP)
    {
        laUtils_ClipRectToParent(btn->widget.parent, &rect);
        laUtils_RectToLayerSpace((laWidget*)btn, &rect);

        if(GFX_RectContainsPoint(&rect, &pnt) == GFX_TRUE)
        {
            btn->state = LA_BUTTON_STATE_DOWN;

            // try to find a reason to redraw
            if(btn->pressedImage != NULL && btn->pressedImage != btn->releasedImage)
                dirty = LA_TRUE;
                
            if(btn->widget.borderType == LA_WIDGET_BORDER_BEVEL)
                dirty = LA_TRUE;
                
            if(btn->pressedOffset != 0 &&
               (btn->pressedImage != NULL ||
                laString_IsEmpty(&btn->text) == LA_FALSE))
                dirty = LA_TRUE;
                
            if(dirty == LA_TRUE)
                laWidget_Invalidate((laWidget*)btn);
        }
    }
}

#endif // LA_BUTTON_WIDGET_ENABLED