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
#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#include "gfx/libaria/inc/libaria_widget_skin_classic_common.h"
#include "gfx/hal/inc/gfx_math.h"

enum
{
    NOT_STARTED = LA_WIDGET_DRAW_STATE_READY,
    DONE = LA_WIDGET_DRAW_STATE_DONE,
    DRAW_BACKGROUND,
	ARRANGE_ITEMS,
	DRAW_ELLIPSE,
};

static void drawBackground(laRadialMenuWidget* mn);
static void drawEllipse(laRadialMenuWidget* mn);
static void arrangeItems(laRadialMenuWidget* mn);

static void nextState(laRadialMenuWidget* mn)
{
    switch(mn->widget.drawState)
    {
        case NOT_STARTED:
        {
            if(mn->widget.backgroundType != LA_WIDGET_BACKGROUND_NONE) 
            {
                mn->widget.drawState = DRAW_BACKGROUND;
                mn->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawBackground;

                return;
            }
        }
        case DRAW_BACKGROUND:
        {
            if(mn->drawEllipse == LA_TRUE)
            {
                mn->widget.drawState = DRAW_ELLIPSE;
                mn->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&drawEllipse;
            }
            else
            {
                mn->widget.drawState = ARRANGE_ITEMS;
                mn->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&arrangeItems;                
            }
            return;
        }
        case DRAW_ELLIPSE:
        {            
            if(mn->widgetList.size > 0)
            {
                mn->widget.drawState = ARRANGE_ITEMS;
                mn->widget.drawFunc = (laWidget_DrawFunction_FnPtr)&arrangeItems;                
            }
            else
            {
                mn->widget.drawState = DONE;
                mn->widget.drawFunc = NULL;                
            }
            return;
        }
        case ARRANGE_ITEMS:
        {
            mn->widget.drawState = DONE;
            mn->widget.drawFunc = NULL;                
            return;
        }
    }
}

static void drawBackground(laRadialMenuWidget* mn)
{
    laWidget_SkinClassic_DrawBackground(&mn->widget, mn->widget.scheme->background);        

    nextState(mn);
}

static void drawEllipse(laRadialMenuWidget* mn)
{
    GFX_Point centerPoint = {0,0};
    
    GFX_Set(GFXF_DRAW_MODE, GFX_DRAW_LINE);
    GFX_Set(GFXF_DRAW_COLOR, mn->widget.scheme->foreground);
    
    laUtils_PointToLayerSpace((laWidget*)mn, &centerPoint);

    centerPoint.x += mn->widget.rect.width / 2;
    centerPoint.y += mn->widget.rect.height / 2;
    
    GFX_Set(GFXF_DRAW_CLIP_ENABLE, LA_TRUE);
    GFX_DrawEllipse(centerPoint.x, centerPoint.y, mn->a, mn->b, mn->theta, 0, 360, mn->widget.rect.width, mn->widget.rect.height);
    
    nextState(mn);
}

//Weigh the angle t between 90 (prominent) and 270 (sunken) and returns a percent, t has to be between 0 to 360
uint32_t _weighAngle(int32_t t)
{
    int32_t angle;
    
    t = GFX_Normalize360(t);
    
    if (t >= 90 && t <= 270)
    {
        angle = t - 90;
    }
    else if (t < 90)
    {
        angle = 90 - t;
    }
    else
    {
        angle = 450 - t;
    }

    //Calculate the percent scale relative to prominent (90) and minimum (270)
    return GFX_PercentWholeRounded(angle, 180);
}

static laResult _insertionSort(laList* list, laRadialMenuItemNode* item)
{
    uint32_t i, percentTest, percentNew; 
    laRadialMenuItemNode* testItem;
    
    if (list->size == 0)
    {
        laList_PushFront(list, item);
        return LA_SUCCESS;
    }
    
    percentNew = _weighAngle(item->t);

    for(i = 0; i < list->size; ++i)
    {
        testItem = laList_Get(list,i);

        percentTest = _weighAngle(testItem->t);

        if (percentNew > percentTest)
        {
            laList_InsertAt(list, item, i);
            return LA_SUCCESS;
        }
    }

    //if we got here without an insertion, item goes to the back
    laList_PushBack(list, item);
    
    return LA_SUCCESS;
}

static void arrangeItems(laRadialMenuWidget* mn)
{
    GFX_Point itemPos;
//    GFX_Rect mnScreenRect;
    laRadialMenuItemNode* item;
    uint32_t i,percent;
    laList drawOrderList;
    
    laList_Create(&drawOrderList);

//    mnScreenRect = laWidget_RectToLayerSpace((laWidget*)mn);
    for (i = 0; i < mn->hiddenList.size; ++i)
    {
        item = laList_Get(&mn->hiddenList, i);

        laWidget_SetVisible(item->widget, LA_FALSE);
    }
    
    for (i = 0; i < mn->shownList.size; ++i)
    {
        item = laList_Get(&mn->shownList, i);
        
        percent = 100 - _weighAngle(item->t);

		item->widget->alphaEnabled = mn->scaleItemAlpha == LA_RADIAL_MENU_SCALE_OFF ? LA_FALSE : LA_TRUE;

		switch (mn->scaleItemAlpha)
        {
            case LA_RADIAL_MENU_SCALE_GRADUAL:
                item->widget->alphaAmount = GFX_Lerp(mn->minAlphaAmount, mn->maxAlphaAmount, percent);                
                break;
            case LA_RADIAL_MENU_SCALE_OFF:
                item->widget->alphaAmount = item->origAlphaAmount;
                break;
            case LA_RADIAL_MENU_SCALE_PROMINENT:
                if (laRadialMenuWidget_IsProminent(mn, item->widget))
                {
                    item->widget->alphaAmount = mn->maxAlphaAmount;
                }
                else
                {
                    item->widget->alphaAmount = mn->minAlphaAmount;
                }                
                break;
        }

        switch(mn->scaleItemSize)
        {
            case LA_RADIAL_MENU_SCALE_GRADUAL:
                item->widget->rect.width = item->origSize.width * GFX_Lerp(mn->minSizePercent, mn->maxSizePercent, percent) / 100;                
                item->widget->rect.height = item->origSize.height * GFX_Lerp(mn->minSizePercent, mn->maxSizePercent, percent) / 100;                
                break;
            case LA_RADIAL_MENU_SCALE_OFF:
                item->widget->rect.width = item->origSize.width;
                item->widget->rect.height = item->origSize.height;        
                break;
            case LA_RADIAL_MENU_SCALE_PROMINENT:

                if (laRadialMenuWidget_IsProminent(mn, item->widget))
                {
                    item->widget->rect.width = item->origSize.width * (int32_t)mn->maxSizePercent / 100;
                    item->widget->rect.height = item->origSize.height * (int32_t)mn->maxSizePercent / 100;                
                }
                else
                {
                    item->widget->rect.width = item->origSize.width * (int32_t)mn->minSizePercent / 100;
                    item->widget->rect.height = item->origSize.height * (int32_t)mn->minSizePercent / 100;        
                }                
                break;
        }

        //Establish widget positions
        GFX_EllipsePoint(item->t, mn->a, mn->b, mn->theta, &itemPos);

        itemPos.x = itemPos.x + mn->widget.rect.width / 2 - item->widget->rect.width / 2;
        itemPos.y = itemPos.y + mn->widget.rect.height / 2 - item->widget->rect.height / 2;
        
        laWidget_SetPosition(item->widget, itemPos.x, itemPos.y);        
        
        //This step is to setup arranging the draw order of each item
        laArray_Remove(&mn->widget.children, item->widget);
        
        laWidget_SetVisible(item->widget, LA_TRUE);
        
        //Sort by inserting into empty list
        _insertionSort(&drawOrderList, item);
    }

    //Re-add each widget item back as child from back to front
    for (i = 0; i < drawOrderList.size; ++i)
    {
        item = laList_Get(&drawOrderList, i);

        laArray_PushBack(&mn->widget.children, item->widget);
    }

    laList_Clear(&drawOrderList);

    nextState(mn);
}

void _laRadialMenuWidget_Paint(laRadialMenuWidget* mn)
{
    laContext* context = laContext_GetActive();
    
    if(mn->widget.scheme == NULL)
    {
        mn->widget.drawState = DONE;
        
        return;
    }
    
    if(mn->widget.drawState == NOT_STARTED)
        nextState(mn);
    
    while(mn->widget.drawState != DONE)
    {
        mn->widget.drawFunc((laWidget*)mn);
        
        if(context->preemptLevel == LA_PREEMPTION_LEVEL_2)
            break;
    }
}

#endif // LA_RADIALMENU_WIDGET_ENABLED