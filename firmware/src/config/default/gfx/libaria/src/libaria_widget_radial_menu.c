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


#include "gfx/libaria/inc/libaria_widget_radial_menu.h"

#if LA_RADIAL_MENU_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/hal/inc/gfx_math.h"

#define DEFAULT_WIDTH           200
#define DEFAULT_HEIGHT          150
#define DEFAULT_A               100
#define DEFAULT_B               80
#define DEFAULT_THETA           0

#define DEFAULT_HIGHLIGHT_MARGIN 2

#define MAX_SIZE_PERCENT         200
#define MIN_SIZE_PERCENT         1
#define DEFAULT_MAX_SIZE_PERCENT 100
#define DEFAULT_MIN_SIZE_PERCENT 30

#define MAX_ALPHA_PERCENT           255
#define MIN_ALPHA_PERCENT           0
#define DEFAULT_MAX_ALPHA_SCALE     255
#define DEFAULT_MIN_ALPHA_SCALE     128

#define PROMINENT_ANGLE             90
#define BACK_ANGLE                  270
#define MIN_THETA                   0
#define MAX_THETA                   90

static void _laRadialMenuWidget_Destructor(laRadialMenuWidget* mn);

void _laRadialMenuWidget_Paint(laRadialMenuWidget* mn);

static void _laRadialMenuWidget_TouchMovedEvent(laWidget* wgt, laInput_TouchMovedEvent* evt);
static void _laRadialMenuWidget_TouchDownEvent(laWidget* wgt, laInput_TouchDownEvent* evt);
static void _laRadialMenuWidget_TouchUpEvent(laWidget* wgt, laInput_TouchUpEvent* evt);
static void _laRadialMenuWidget_Resized(laWidget* wgt);

static void invalidateContents(laRadialMenuWidget* mn)
{
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)mn),
		                  &mn->widget.rect,
					      LA_FALSE);
}

static void languageChanging(laRadialMenuWidget* mn)
{
    invalidateContents(mn);
}

static laResult _laRadialMenuWidget_RotateMenuItems(laRadialMenuWidget* mn, int32_t angle)
{
    uint32_t i; 
    laRadialMenuItemNode* item = NULL;
    laRadialMenuItemNode* itemToHide = NULL;
    laList itemsToHide;
    
    if(mn == NULL || mn->widgetList.size == 0)
        return LA_FAILURE;
    
    if(angle == 0)
        return LA_SUCCESS;
    
    laList_Create(&itemsToHide);
    
    for(i = 0; i < mn->shownList.size; ++i)
    {
        item = laList_Get(&mn->shownList, i);
        
        if (mn->itemsShown < mn->widgetList.size)
        {                
            if (abs(item->t - BACK_ANGLE) < abs(angle)
                    && (item->t - BACK_ANGLE) * angle >= 0)
            {
                if (angle > 0)
                {
                    laList_PushBack(&mn->hiddenList, item);
                }
                else
                {
                    laList_PushFront(&mn->hiddenList, item);                            
                }
                
                laList_PushBack(&itemsToHide, item);
            }            
        }

        item->t -= angle;                
        item->t = GFX_Normalize360(item->t);
    }

    for(i = 0; i < itemsToHide.size; ++i)
    {
        itemToHide = laList_Get(&itemsToHide, i);
        
        if (angle > 0)
        {
            item = laList_Get(&mn->hiddenList, 0);
        }
        else
        {
            item = laList_Get(&mn->hiddenList, mn->hiddenList.size - 1);
        }                    

        if (item != NULL)
        {
            laList_Remove(&mn->hiddenList, item);                            
            laList_PushFront(&mn->shownList, item);
            item->t = itemToHide->t;
            itemToHide->t = BACK_ANGLE;
        };
        
        laList_Remove(&mn->shownList, itemToHide);
    }
    
    laList_Clear(&itemsToHide);

    return LA_SUCCESS;
}

static void _laRadialMenuWidget_Constructor(laRadialMenuWidget* mn)
{
    _laWidget_Constructor((laWidget*)mn);
    
    mn->widget.destructor = (laWidget_Destructor_FnPtr)&_laRadialMenuWidget_Destructor;

    mn->widget.languageChangeEvent = (laWidget_LanguageChangingEvent_FnPtr)&languageChanging;

    mn->widget.type = LA_WIDGET_RADIAL_MENU;

    // override base class methods
    mn->widget.update = (laWidget_Update_FnPtr)&_laRadialMenuWidget_Update;
    mn->widget.paint = (laWidget_Paint_FnPtr)&_laRadialMenuWidget_Paint;

    mn->widget.rect.width = DEFAULT_WIDTH;
    mn->widget.rect.height = DEFAULT_HEIGHT;

    mn->drawEllipse = LA_FALSE;

    mn->widget.borderType = LA_WIDGET_BORDER_NONE;

    mn->halign = LA_HALIGN_CENTER;
    mn->valign = LA_VALIGN_MIDDLE;
    
    mn->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&_laRadialMenuWidget_TouchDownEvent;
    mn->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&_laRadialMenuWidget_TouchUpEvent;
    mn->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&_laRadialMenuWidget_TouchMovedEvent;
    mn->widget.resized = (laWidget_Resized_FnPtr)&_laRadialMenuWidget_Resized;

    mn->highlighter = NULL;

    mn->a = DEFAULT_A;
    mn->b = DEFAULT_B;
    mn->theta = DEFAULT_THETA;
    mn->ellipseChanged = LA_TRUE;
    mn->itemsShown = 0;

    mn->scaleItemSize = LA_RADIAL_MENU_SCALE_OFF;
    mn->scaleItemAlpha = LA_RADIAL_MENU_SCALE_OFF;
    
    mn->maxSizePercent = DEFAULT_MAX_SIZE_PERCENT;
    mn->minSizePercent = DEFAULT_MIN_SIZE_PERCENT;
    
    mn->maxAlphaAmount = DEFAULT_MAX_ALPHA_SCALE;
    mn->minAlphaAmount = DEFAULT_MIN_ALPHA_SCALE;
    
    laList_Create(&mn->widgetList);     
    laList_Create(&mn->shownList);     
    laList_Create(&mn->hiddenList);     
}

static void _laRadialMenuWidget_Destructor(laRadialMenuWidget* mn)
{
    laList_Destroy(&mn->widgetList);

    mn->highlighter = NULL;
    
    // free any existing memory reader
    if(mn->reader != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(mn->reader);
        
        mn->reader = NULL;
    }

    _laWidget_Destructor((laWidget*)mn);
}

static laBool _laRadialMenuWidget_UpdateWidestTallestItem(laRadialMenuWidget* mn)
{
    uint32_t i;
    laRadialMenuItemNode* item = NULL;
    
    if(mn == NULL || mn->widgetList.size == 0)
        return LA_FALSE;
    
    for(i = 0; i < mn->widgetList.size; ++i)
    {
        item = laList_Get(&mn->widgetList, i);
        
        if (item != NULL)
        {
            if (mn->widestWidgetItem == NULL)
            {
                mn->widestWidgetItem = item;
            }
            else
            {
                if (item->widget->rect.width > mn->widestWidgetItem->widget->rect.width)
                {
                    mn->widestWidgetItem = item;
                }
            }

            if (mn->tallestWidgetItem == NULL)
            {
                mn->tallestWidgetItem = item;
            }
            else
            {
                if (item->widget->rect.height > mn->widestWidgetItem->widget->rect.height)
                {
                    mn->tallestWidgetItem = item;
                }
            }        
        }
    }
    
    return LA_FAILURE;    
}

static laBool _laRadialMenuWidget_DeriveWidestTallestItem(laRadialMenuWidget* mn, laRadialMenuItemNode* item, laWidget* widget)
{
    laRadialMenuItemNode* testItem = NULL;
    uint32_t i = 0;
    
    if (mn == NULL)
        return LA_FAILURE;

    //This is a widget addition
    if (item != NULL && widget != NULL)
    {
        if (mn->widestWidgetItem == NULL)
        {
            mn->widestWidgetItem = item;
        }
        else
        {
            if (widget->rect.width > mn->widestWidgetItem->widget->rect.width)
            {
                mn->widestWidgetItem = item;
            }
        }

        if (mn->tallestWidgetItem == NULL)
        {
            mn->tallestWidgetItem = item;
        }
        else
        {
            if (widget->rect.height > mn->widestWidgetItem->widget->rect.height)
            {
                mn->tallestWidgetItem = item;
            }
        }        

        return LA_SUCCESS;    
    }
    else if (widget == NULL) //this is a widget removal, if it happens to be the tallest or the widest widget, 
                             //a new one will have to be assigned
    {
        if (mn->widestWidgetItem == item && mn->widgetList.size > 1)
        {
            mn->widestWidgetItem = laList_Get(&mn->widgetList, 0);
            
            for(i = 1; i < mn->widgetList.size; ++i)
            {
                testItem = laList_Get(&mn->widgetList, i);
                
                if (testItem != NULL && mn->widestWidgetItem != NULL 
                        && testItem->widget->rect.width > mn->widestWidgetItem->widget->rect.width)
                {
                    mn->widestWidgetItem = testItem;
                }
            }
        }

        if (mn->tallestWidgetItem == item && mn->widgetList.size > 1)
        {
            mn->tallestWidgetItem = laList_Get(&mn->widgetList, 0);
            
            for(i = 1; i < mn->widgetList.size; ++i)
            {
                testItem = laList_Get(&mn->widgetList, i);
                
                if (testItem != NULL && mn->tallestWidgetItem != NULL 
                        && testItem->widget->rect.height > mn->widestWidgetItem->widget->rect.height)
                {
                    mn->tallestWidgetItem = testItem;
                }
            }            
        }
        
        return LA_SUCCESS;        
    }
    
    return LA_FAILURE;    
}

static int32_t _laRadialMenuWidget_GetSliceAngle(laRadialMenuWidget* mn)
{
    if(mn == NULL || mn->itemsShown == 0)
        return 0;

    return (360 / mn->itemsShown);
}

static void _laRadialMenuWidget_Resized(laWidget* wgt)
{
    laRadialMenuWidget* mn = (laRadialMenuWidget*)wgt;
    
    if (mn == NULL)
        return;
    
    mn->ellipseChanged = LA_TRUE;
}

static laBool _laRadialMenuWidget_UpdateEllipse(laRadialMenuWidget* mn)
{
    if (mn == NULL || mn->widestWidgetItem == NULL || mn->tallestWidgetItem == NULL || mn->ellipseChanged == LA_FALSE)
        return LA_FAILURE;
    
    int16_t sine, cosine = 0;
    int32_t widest, tallest, scale = 0;
    
    _laRadialMenuWidget_UpdateWidestTallestItem(mn);
    
    sine = GFX_SineCosineGet(mn->theta, GFX_TRIG_SINE_TYPE);
    cosine = GFX_SineCosineGet(mn->theta, GFX_TRIG_COSINE_TYPE);

    scale = mn->scaleItemSize != LA_RADIAL_MENU_SCALE_OFF ? mn->maxSizePercent : 100;
    widest = mn->widestWidgetItem->widget->rect.width;
    tallest = mn->tallestWidgetItem->widget->rect.height * scale / 100;
    
    if (mn->highlightProminent == LA_TRUE)
    {
        widest += DEFAULT_HIGHLIGHT_MARGIN * 2;
        tallest += DEFAULT_HIGHLIGHT_MARGIN * 2;
    }
    
    switch(mn->ellipseType)
    {
        case LA_RADIAL_MENU_ELLIPSE_TYPE_DEFAULT:
            mn->b = mn->widget.rect.height / 2 - tallest / 2;
            mn->a = mn->widget.rect.width / 2 - widest / 2;

            mn->a = GFX_Clampi(0, mn->widget.rect.width / 2,  mn->a);
            mn->b = GFX_Clampi(0, mn->widget.rect.height / 2, mn->b);    
            break;
        case LA_RADIAL_MENU_ELLIPSE_TYPE_ORBITAL:
            mn->b = abs((mn->widget.rect.height / 2 - tallest / 2) * sine / 256 - (widest / 2) * sine / 256);
            mn->a = abs((mn->widget.rect.width / 2 - widest / 2) * cosine / 256 - (tallest / 2) * sine / 256);        

            mn->a = GFX_Clampi(0, mn->widget.rect.width / 2 * cosine / 256,  mn->a);
            mn->b = GFX_Clampi(0, mn->widget.rect.height / 2 * sine / 256, mn->b);
            break;
        case LA_RADIAL_MENU_ELLIPSE_TYPE_ROLLODEX:
            mn->b = 0;
            mn->a = mn->widget.rect.height / 2 - tallest / 2;
            mn->theta = 90;
            break;
        default:
            break;
    }

    //Shave some calculations
    mn->ellipseChanged = LA_FALSE;
    
    return LA_SUCCESS;
}

laWidgetUpdateState _laRadialMenuWidget_Update(laRadialMenuWidget* mn)
{
    laRadialMenuItemNode* prominentItem = NULL;

    if (mn != NULL)
    {
        switch(mn->state)
        {
            case LA_RADIAL_MENU_INIT:
            {
                invalidateContents(mn);
                mn->state = LA_RADIAL_MENU_RESET_TO_INPUT_POS;
                break;
            }
            case LA_RADIAL_MENU_INPUT_READY:
            {
                break;
            }
            case LA_RADIAL_MENU_HANDLE_USER_MOVE_REQUEST:
            {
                //Rotate the menu to target angle
                if (mn->userRequestedAngleDiff != 0)
                {
                    mn->userRequestedAngleDiff = mn->userRequestedAngleDiff * 2 / 3;

                    _laRadialMenuWidget_RotateMenuItems(mn, mn->userRequestedAngleDiff);

                    invalidateContents(mn);
                }
                else
                {
                    // If the user still has his finger pressed in the widget touch area,
                    // do not interfere
                    if (mn->touchPressed == LA_FALSE)
                    {
                        mn->prominentIndex = laRadialMenuWidget_GetProminentIndex(mn);
                        prominentItem = laList_Get(&mn->widgetList, mn->prominentIndex);
                        
                        mn->state = LA_RADIAL_MENU_RESET_TO_INPUT_POS;                        
                    }
                    else
                    {
                        mn->touchPressed = LA_TRUE;
                    }
                }
                break;
            }
            case LA_RADIAL_MENU_RESET_TO_INPUT_POS:
            {
                prominentItem = laList_Get(&mn->widgetList, mn->prominentIndex);

                if (prominentItem != NULL)
                {
                    mn->targetAngleDiff = (PROMINENT_ANGLE - prominentItem->t) * -1;
                }

                if (abs(mn->targetAngleDiff) >= 2)
                {
                    mn->targetAngleDiff /= 2;                    
                    _laRadialMenuWidget_RotateMenuItems(mn, mn->targetAngleDiff);
                }
                else //1 or -1
                {
                    _laRadialMenuWidget_RotateMenuItems(mn, mn->targetAngleDiff);
                    mn->targetAngleDiff = 0;
                }
                
                invalidateContents(mn);
                
                if (mn->targetAngleDiff == 0)
                {
                    if (mn->prominentIndex != mn->lastProminentIndex)
                    {
                        if (mn->itemProminenceChangedEvent != NULL)
                        {
                            mn->itemProminenceChangedEvent(mn, prominentItem->widget, mn->prominentIndex);
                        }

                        mn->lastProminentIndex = mn->prominentIndex;
                    }
                    
                    if (mn != NULL && mn->highlightProminent && mn->highlighter != NULL)
                    {
                        laWidget_SetPosition((laWidget*)mn->highlighter, 
                                prominentItem->widget->rect.x - DEFAULT_HIGHLIGHT_MARGIN, 
                                prominentItem->widget->rect.y - DEFAULT_HIGHLIGHT_MARGIN);
                        laWidget_SetSize((laWidget*)mn->highlighter, 
                                prominentItem->widget->rect.width + DEFAULT_HIGHLIGHT_MARGIN * 2, 
                                prominentItem->widget->rect.height + DEFAULT_HIGHLIGHT_MARGIN * 2);
                        laWidget_SetVisible((laWidget*)mn->highlighter, true);        
                    }
 
                    mn->userRequestedDirection = 0;
                    mn->state = LA_RADIAL_MENU_INPUT_READY;
                }
                break;
            }   
        }
        
        _laRadialMenuWidget_UpdateEllipse(mn);
    }
    
    return LA_WIDGET_UPDATE_STATE_PENDING;
}

laRadialMenuWidget* laRadialMenuWidget_New()
{
    laRadialMenuWidget* mn = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    mn = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laRadialMenuWidget));

    if(mn == NULL)
        return mn;
        
    _laRadialMenuWidget_Constructor(mn);

    return mn;
}

static laRadialMenuItemNode* _laRadialMenuWidget_GetWidgetItem(laRadialMenuWidget* mn, laWidget* widget)
{
    uint32_t i;
    laRadialMenuItemNode* item = NULL;
    
    if(mn == NULL || mn->widgetList.size == 0)
        return NULL;
    
    for(i = 0; i < mn->widgetList.size; ++i)
    {
        item = laList_Get(&mn->widgetList, i);
        
        if (item != NULL)
        {
            if(item->widget == widget)
            {
                return item;
            }
        }
    }
    
    return NULL;
}

laResult laRadialMenuWidget_SetHighlightProminent(laRadialMenuWidget* mn, laBool enable)
{
    if(mn == NULL)
        return LA_FALSE;
    
    mn->highlightProminent = enable;

    invalidateContents(mn);
    return LA_SUCCESS;
}

laBool laRadialMenuWidget_IsProminent(laRadialMenuWidget* mn, laWidget* widget)
{
    laRadialMenuItemNode* item = NULL;
    int32_t angle = 0;
    
    if(mn == NULL)
        return LA_FALSE;

    item = _laRadialMenuWidget_GetWidgetItem(mn, widget);
    
    if(item == NULL)
        return LA_FALSE;
    
    angle = _laRadialMenuWidget_GetSliceAngle(mn) / 2;

    //Check if the widget's current t is close to the prominent angle
    if (abs(item->t - PROMINENT_ANGLE) <= angle)
    {
        return LA_TRUE;
    }
    
    return LA_FALSE;
}

laResult laRadialMenuWidget_SetProminent(laRadialMenuWidget* mn, laWidget* widget)
{
    laRadialMenuItemNode* item = NULL;
    
    if(mn == NULL)
        return LA_FAILURE;

    item = _laRadialMenuWidget_GetWidgetItem(mn, widget);
    
    if(item == NULL)
        return LA_FALSE;
    
    mn->targetAngleDiff = PROMINENT_ANGLE - item->t;
    mn->state =  LA_RADIAL_MENU_HANDLE_USER_MOVE_REQUEST;
    
    invalidateContents(mn);
    
    return LA_SUCCESS;
}

int32_t laRadialMenuWidget_GetProminentIndex(laRadialMenuWidget* mn)
{
    uint32_t i;
    laRadialMenuItemNode* item = NULL;
    
    if(mn == NULL || mn->widgetList.size == 0)
        return -1;
    
    for(i = 0; i < mn->widgetList.size; ++i)
    {
        item = laList_Get(&mn->widgetList, i);
        
        if (item != NULL)
        {
            if(laRadialMenuWidget_IsProminent(mn, item->widget) == LA_TRUE)
            {
                return (int32_t)i;
            }
        }
    }
    
    return -1;
}

laResult laRadialMenuWidget_SetProminentIndex(laRadialMenuWidget* mn, uint32_t index)
{
    laRadialMenuItemNode* item = NULL;

    if(mn == NULL || mn->widgetList.size == 0)
        return LA_FAILURE;

    item = laList_Get(&mn->widgetList, index);
    
    if(item == NULL)
        return LA_FALSE;
    
    mn->targetAngleDiff = PROMINENT_ANGLE - item->t;
    mn->state =  LA_RADIAL_MENU_HANDLE_USER_MOVE_REQUEST;
    
    invalidateContents(mn);
    
    return LA_SUCCESS;
}

laWidget* laRadialMenuWidget_GetWidget(laRadialMenuWidget* mn, int32_t index)
{
    laRadialMenuItemNode* item = NULL;

    if(mn == NULL || mn->widgetList.size == 0 || mn->widgetList.size < index - 1)
        return NULL;
    
    item = laList_Get(&mn->widgetList, index);
    
    return item->widget;
}

static laResult _laRadialMenuWidget_RespaceItems(laRadialMenuWidget* mn)
{
    uint32_t i;
    laRadialMenuItemNode* item = NULL;
    int32_t angle = 0;
    int32_t frontLastIndex = 0;
    int32_t backFirstIndex = 0;
    int32_t sliceCount = 0;
    
    if(mn == NULL || mn->widgetList.size == 0 || mn->itemsShown == 0)
        return LA_FAILURE;
    
    angle = _laRadialMenuWidget_GetSliceAngle(mn);

    frontLastIndex = mn->itemsShown % 2 == 0 ? mn->itemsShown / 2 : mn->itemsShown / 2 + 1;
    backFirstIndex = (mn->widgetList.size - 1) - mn->itemsShown / 2;
    
    laList_Clear(&mn->shownList);
    laList_Clear(&mn->hiddenList);        
    
    for(i = 0; i < mn->widgetList.size; ++i)
    {
        item = laList_Get(&mn->widgetList, i);
        
        if (item != NULL)
        {
            if (i < frontLastIndex || i > backFirstIndex 
                    || mn->itemsShown == mn->widgetList.size)
            {
                item->t = sliceCount++ * angle;
                laList_PushFront(&mn->shownList, item);
            }
            else
            {
                //Keep the hidden items at 270
                item->t = BACK_ANGLE;
                laList_PushFront(&mn->hiddenList, item);
            }
        }
    }

    mn->state = LA_RADIAL_MENU_RESET_TO_INPUT_POS;
    
    return LA_SUCCESS;
}

laResult laRadialMenuWidget_SetNumberOfItemsShown(laRadialMenuWidget* mn, int32_t num)
{
    if(mn == NULL)
        return LA_FAILURE;

    if (mn->itemsShown == num)
        return LA_SUCCESS;
    
    mn->itemsShown = GFX_Clampi(0, mn->widgetList.size, num);
    
    _laRadialMenuWidget_RespaceItems(mn);
    
    invalidateContents(mn);
    
    return LA_SUCCESS;
}

laResult laRadialMenuWidget_SetWidgetAt(laRadialMenuWidget* mn, laWidget* widget, int32_t index)
{
    laRadialMenuItemNode* item = NULL;

    if(mn == NULL || widget->parent == (laWidget*)mn)
        return LA_FAILURE;
    
    if (index < mn->widgetList.size)
    {
        item = laList_Get(&mn->widgetList, index);
    }
    else
    {
        item = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laRadialMenuItemNode));
        laList_InsertAt(&mn->widgetList, item, index);
    }    
    
    if (item == NULL)
        return LA_FAILURE;
    
    item->widget = widget;

    item->origTouchDown = widget->touchDown;
    item->origTouchUp   = widget->touchUp;
    item->origTouchMoved = widget->touchMoved;
    
    item->origAlphaAmount = widget->alphaAmount;
    item->origSize = widget->rect;
    
    laWidget_OverrideTouchDownEvent(widget, &_laRadialMenuWidget_TouchDownEvent);            
    laWidget_OverrideTouchMovedEvent(widget, &_laRadialMenuWidget_TouchMovedEvent);                
    laWidget_OverrideTouchUpEvent(widget, &_laRadialMenuWidget_TouchUpEvent);
    laWidget_SetParent(widget,(laWidget*)mn);
    
    _laRadialMenuWidget_DeriveWidestTallestItem(mn, item, widget);
    
    //Make sure items shown is valid
    if (mn->itemsShown == 0 || mn->itemsShown > mn->widgetList.size)
    {
        mn->itemsShown = mn->widgetList.size;
    }

    _laRadialMenuWidget_RespaceItems(mn);

    invalidateContents(mn);

    return LA_SUCCESS;
}

laResult laRadialMenuWidget_AddWidget(laRadialMenuWidget* mn, laWidget* widget)
{
    laRadialMenuItemNode* item = NULL;
    
    if(mn == NULL || widget->parent == (laWidget*)mn)
        return LA_FAILURE;
    
    item = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laRadialMenuItemNode));
    
    if (item == NULL)
        return LA_FAILURE;
    
    item->widget = widget;

    item->origTouchDown = widget->touchDown;
    item->origTouchUp   = widget->touchUp;
    item->origTouchMoved = widget->touchMoved;
    
    item->origAlphaAmount = widget->alphaAmount;
    item->origSize = widget->rect;
    
    laList_PushBack(&mn->widgetList, item);
    
    laWidget_OverrideTouchDownEvent(widget, &_laRadialMenuWidget_TouchDownEvent);            
    laWidget_OverrideTouchMovedEvent(widget, &_laRadialMenuWidget_TouchMovedEvent);                
    laWidget_OverrideTouchUpEvent(widget, &_laRadialMenuWidget_TouchUpEvent);
    laWidget_SetParent(widget,(laWidget*)mn);
    
    _laRadialMenuWidget_DeriveWidestTallestItem(mn, item, widget);

    //Make sure items shown is valid
    if (mn->itemsShown == 0 || mn->itemsShown > mn->widgetList.size)
    {
        mn->itemsShown = mn->widgetList.size;
    }
    
    _laRadialMenuWidget_RespaceItems(mn);
    
    invalidateContents(mn);

    return LA_SUCCESS;
}

laResult laRadialMenuWidget_RemoveWidget(laRadialMenuWidget* mn, laWidget* widget)
{
    laRadialMenuItemNode* item = NULL;
    
    if (mn == NULL || mn->widgetList.size == 0 || widget == NULL)
        return LA_FAILURE;
    
    item = _laRadialMenuWidget_GetWidgetItem(mn, widget);
    
    if (item == NULL)
        return LA_FAILURE;
    
    laWidget_OverrideTouchDownEvent(widget, item->origTouchDown);            
    laWidget_OverrideTouchMovedEvent(widget, item->origTouchMoved);                
    laWidget_OverrideTouchUpEvent(widget, item->origTouchUp);
    laList_Remove(&mn->widgetList, item);
    
    _laRadialMenuWidget_DeriveWidestTallestItem(mn, item, widget);
    
    laContext_GetActive()->memIntf.heap.free(&item);
    item = NULL;
    
    //Make sure items shown is valid
    if (mn->itemsShown == 0 || mn->itemsShown > mn->widgetList.size)
    {
        mn->itemsShown = mn->widgetList.size;
    }

    _laRadialMenuWidget_RespaceItems(mn);
    
    invalidateContents(mn);
    
    return LA_SUCCESS;    
}

laResult laRadialMenuWidget_ClearItems(laRadialMenuWidget* mn)
{
    uint32_t i = 0;
    laRadialMenuItemNode* item = NULL;
    
    if (mn == NULL)
        return LA_FAILURE;
    
    mn->tallestWidgetItem = NULL;
    mn->widestWidgetItem = NULL;
    mn->itemsShown = 0;
    
    for(i = 0; i < mn->widgetList.size; ++i)
    {
        item = laList_Get(&mn->widgetList, i);
        
        if (item != NULL)
        {
            laWidget_OverrideTouchDownEvent(item->widget, item->origTouchDown);            
            laWidget_OverrideTouchMovedEvent(item->widget, item->origTouchMoved);                
            laWidget_OverrideTouchUpEvent(item->widget, item->origTouchUp);                    
 
            laContext_GetActive()->memIntf.heap.free(&item);
            item = NULL;
        }
    }
    
    laList_Clear(&mn->shownList);
    laList_Clear(&mn->hiddenList);
    laList_Clear(&mn->widgetList);
    invalidateContents(mn);
    
    return LA_SUCCESS;    
}

laResult laRadialMenuWidget_SetSizeScaling(laRadialMenuWidget* mn, laRadialMenuWidgetScaleType setting)
{
    if (mn == NULL)
        return LA_FAILURE;

    mn->ellipseChanged = LA_TRUE;
    mn->scaleItemSize = setting;
    invalidateContents(mn);

    mn->state = LA_RADIAL_MENU_RESET_TO_INPUT_POS;
    
    return LA_SUCCESS;    
}

laResult laRadialMenuWidget_SetAlphaScaling(laRadialMenuWidget* mn, laRadialMenuWidgetScaleType setting)
{
    uint32_t i = 0;
    laRadialMenuItemNode* item = NULL;

    if (mn == NULL)
        return LA_FAILURE;

    mn->scaleItemAlpha = setting;
    
    for(i = 0; i < mn->widgetList.size; ++i)
    {
        item = laList_Get(&mn->widgetList, i);

        if (item != NULL)
        {
            if (setting != LA_RADIAL_MENU_SCALE_OFF)
            {
                item->widget->alphaEnabled = LA_TRUE;
            }
            else
            {
                item->widget->alphaEnabled = LA_FALSE;
            }
        }
    }
    
    invalidateContents(mn);

    mn->state = LA_RADIAL_MENU_RESET_TO_INPUT_POS;
    
    return LA_SUCCESS;    
}

laResult laRadialMenuWidget_SetSizeScaleMinMax(laRadialMenuWidget* mn, int32_t min, int32_t max)
{
    if (mn == NULL)
        return LA_FAILURE;
    
    mn->maxSizePercent = GFX_Clampi(MIN_SIZE_PERCENT, MAX_SIZE_PERCENT, max);
    mn->minSizePercent = GFX_Clampi(MIN_SIZE_PERCENT, MAX_SIZE_PERCENT, min);
    mn->ellipseChanged = LA_TRUE;

    invalidateContents(mn);

    return LA_SUCCESS;    
}

laResult laRadialMenuWidget_SetAlphaScaleMinMax(laRadialMenuWidget* mn, int32_t min, int32_t max)
{
    if (mn == NULL)
        return LA_FAILURE;
    
    mn->maxAlphaAmount = GFX_Clampi(MIN_ALPHA_PERCENT, MAX_ALPHA_PERCENT, max);
    mn->minAlphaAmount = GFX_Clampi(MIN_ALPHA_PERCENT, MAX_ALPHA_PERCENT, min);
    invalidateContents(mn);

    return LA_SUCCESS;    
}

laResult laRadialMenuWidget_SetDrawEllipse(laRadialMenuWidget* mn, laBool enable)
{
    if (mn == NULL)
        return LA_FAILURE;
    
    mn->drawEllipse = enable;
    invalidateContents(mn);
    
    return LA_SUCCESS;
}

laResult laRadialMenuWidget_SetTouchArea(laRadialMenuWidget* mn, 
                                int32_t x, int32_t y, int32_t width, int32_t height)
{
    GFX_Rect rect;
    
    if (mn == NULL)
        return LA_FAILURE;

    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    
    mn->touchArea = rect;
    
    return LA_SUCCESS;    
}

laResult laRadialMenuWidget_SetEllipseType(laRadialMenuWidget* mn, laRadialMenuEllipseType type)
{
    if(mn == NULL)
        return LA_FAILURE;

    mn->ellipseType = type;

    invalidateContents(mn);
    return LA_SUCCESS;
}

int32_t laRadialMenuWidget_GetTheta(laRadialMenuWidget* mn)
{
    if(mn == NULL)
        return 0;
    
    return mn->theta;
}

laResult laRadialMenuWidget_SetTheta(laRadialMenuWidget* mn, int32_t theta)
{
    if(mn == NULL)
        return LA_FAILURE;
    
    mn->theta = GFX_Clampi(MIN_THETA, MAX_THETA, theta);
    mn->ellipseChanged = LA_TRUE;
    invalidateContents(mn);

    return LA_SUCCESS;
}

laRadialMenuWidget_ItemSelectedEvent laRadialMenuWidget_GetItemSelectedEventCallback(laRadialMenuWidget* mn)
{
    if(mn == NULL)
        return NULL;

    return mn->itemSelectedEvent;
}

laResult laRadialMenuWidget_SetItemSelectedEventCallback(laRadialMenuWidget* mn,
                                                      laRadialMenuWidget_ItemSelectedEvent cb)
{
    if(mn == NULL || mn->itemSelectedEvent == cb)
        return LA_FAILURE;

    mn->itemSelectedEvent = cb;
    
    return LA_SUCCESS;
}

laResult laRadialMenuWidget_SetItemProminenceChangedEventCallback(laRadialMenuWidget* mn,
                                                      laRadialMenuWidget_ItemProminenceChangedEvent cb)
{
    if(mn == NULL || mn->itemProminenceChangedEvent == cb)
        return LA_FAILURE;

    mn->itemProminenceChangedEvent = cb;
    
    return LA_SUCCESS;
}

laRadialMenuWidget_ItemProminenceChangedEvent laRadialMenuWidget_GetItemProminenceChangeddEventCallback(laRadialMenuWidget* mn)
{
    if(mn == NULL)
        return NULL;

    return mn->itemProminenceChangedEvent;
}

laResult laRadialMenuWidget_SetDeselectedEventCallback(laRadialMenuWidget* mn,
                                                        laRadialMenuWidget_ItemProminenceChangedEvent cb)
{
    if(mn == NULL || mn->itemProminenceChangedEvent == cb)
        return LA_FAILURE;

    mn->itemProminenceChangedEvent = cb;
    
    return LA_SUCCESS;
}

static void _laRadialMenuWidget_TouchDownEvent(laWidget* widget, laInput_TouchDownEvent* evt)
{
    laRadialMenuItemNode* widgetItem;
    laRadialMenuWidget* mn = NULL;
    GFX_Rect clipRect;
    GFX_Point touchPoint;
    
    if (widget == NULL || widget->parent == NULL)
        return;

    if (widget->type == LA_WIDGET_RADIAL_MENU)
    {
        mn = (laRadialMenuWidget*)widget;

        if (mn->state != LA_RADIAL_MENU_INPUT_READY)
            return;
    }
    else if (widget->parent->type == LA_WIDGET_RADIAL_MENU)
    {
        mn = (laRadialMenuWidget*)widget->parent;
        
        if (mn->state != LA_RADIAL_MENU_INPUT_READY)
            return;
    
        clipRect = mn->touchArea;
        
        laUtils_RectToScreenSpace((laWidget*)mn, &clipRect);
        
        touchPoint.x = evt->x;
        touchPoint.y = evt->y;
        
        //Only response if the widget is in the touch allowed area and is 
        //the prominent widget
        if (GFX_RectContainsPoint(&clipRect, &touchPoint) == GFX_TRUE &&
                laRadialMenuWidget_IsProminent(mn, widget) == LA_TRUE)
        {
            widgetItem = _laRadialMenuWidget_GetWidgetItem( mn, widget );    

            if (widgetItem != NULL)
            {
                widgetItem->origTouchDown(widget, evt);
            }            
        }        
    }
        
    mn->touchPressed = LA_TRUE;
    
    if (mn != NULL && mn->highlighter != NULL)
    {
        laWidget_SetVisible((laWidget*)mn->highlighter, false);        
    }

    evt->event.accepted = LA_TRUE;
}

static void _laRadialMenuWidget_TouchUpEvent(laWidget* widget, laInput_TouchUpEvent* evt)
{
    laRadialMenuItemNode* widgetItem;
    laRadialMenuWidget* mn = NULL;
    GFX_Rect clipRect;
    GFX_Point touchPoint;
    
    if (widget == NULL || widget->parent == NULL)
        return;

    if (widget->type == LA_WIDGET_RADIAL_MENU)
    {
        mn = (laRadialMenuWidget*)widget;
    }
    else if (widget->parent->type == LA_WIDGET_RADIAL_MENU)
    {
        mn = (laRadialMenuWidget*)widget->parent;
        
        clipRect = mn->touchArea;
        
        laUtils_RectToScreenSpace((laWidget*)mn, &clipRect);
        
        touchPoint.x = evt->x;
        touchPoint.y = evt->y;
        
        //Only response if the widget is in the touch allowed area
        if (GFX_RectContainsPoint(&clipRect, &touchPoint) == GFX_TRUE  &&
                laRadialMenuWidget_IsProminent(mn, widget) == LA_TRUE &&
                mn->state == LA_RADIAL_MENU_INPUT_READY)
        {
            widgetItem = _laRadialMenuWidget_GetWidgetItem( mn, widget );    

            if (widgetItem != NULL)
            {
                widgetItem->origTouchUp(widget, evt);
            }

            if (mn->itemSelectedEvent != NULL)
            {
                mn->itemSelectedEvent(mn, widget, mn->prominentIndex);
            }
        }        
    }
        
    mn->touchPressed = LA_FALSE;
    
    if (mn != NULL && mn->highlighter != NULL)
    {
        laWidget_SetVisible((laWidget*)mn->highlighter, false);        
    }

    evt->event.accepted = LA_TRUE;
}

static void _laRadialMenuWidget_TouchMovedEvent(laWidget* widget,
                       laInput_TouchMovedEvent* evt)
{
    laRadialMenuWidget* mn = NULL;
    GFX_Rect clipRect;
    GFX_Point touchPoint;
	int32_t dx, dy;
    
    if (widget == NULL || widget->parent == NULL)
        return;

    // Do not continue to trigger any redraw if any layer hasn't been completely drawn
    if (laContext_IsDrawing())
        return;

    if (widget->type == LA_WIDGET_RADIAL_MENU)
    {
        mn = (laRadialMenuWidget*)widget;
    }
    else if (widget->parent->type == LA_WIDGET_RADIAL_MENU)
    {
        mn = (laRadialMenuWidget*)widget->parent;
    }    

    if (mn == NULL)
        return;
    
    clipRect = mn->touchArea;

    laUtils_RectToScreenSpace((laWidget*)mn, &clipRect);

    touchPoint.x = evt->x;
    touchPoint.y = evt->y;

    //Only response if the widget is in the touch allowed area
    if (GFX_RectContainsPoint(&clipRect, &touchPoint) == GFX_TRUE)
    {
        dy = evt->y - evt->prevY;    
        dx = evt->x - evt->prevX;    
        
        mn->touchPressed = LA_TRUE;
        
        if (mn->theta <= 45)
        {
            mn->userRequestedAngleDiff += dx * GFX_SineCosineGet(mn->theta, GFX_TRIG_COSINE_TYPE) / 256 / 4;
        }
        else
        {
            mn->userRequestedAngleDiff += dy * GFX_SineCosineGet(mn->theta, GFX_TRIG_SINE_TYPE) / 256 / 2;
        }
        
        if (mn->userRequestedAngleDiff != 0)
        {
            mn->state = LA_RADIAL_MENU_HANDLE_USER_MOVE_REQUEST;
            
            if (mn->userRequestedAngleDiff > 0)
            {
                mn->userRequestedDirection = 1;
            }
            else if (mn->userRequestedAngleDiff < 0)
            {
                mn->userRequestedDirection = -1;
            }
        }        
    }
    
    evt->event.accepted = LA_TRUE;
}

#endif // LA_RADIALMENU_WIDGET_ENABLED