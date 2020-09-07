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


#include "gfx/libaria/inc/libaria_widget_list.h"

#if LA_LIST_WIDGET_ENABLED && LA_SCROLLBAR_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          100

#define DEFAULT_SCROLL_WIDTH    20
#define DEFAULT_STEP_SIZE       10

#define DEFAULT_MARGIN          10

static void resized(laListWidget* lst);
static void recalculateScrollBarValues(laListWidget* lst);
static void scrollBarMoved(laScrollBarWidget* bar);
static uint32_t getRowAtPoint(laListWidget* lst, GFX_Point* pnt);

static void touchDown(laListWidget* lst, laInput_TouchDownEvent* evt);
static void touchUp(laListWidget* lst, laInput_TouchUpEvent* evt);
static void touchMoved(laListWidget* lst, laInput_TouchMovedEvent* evt);

static void invalidateListArea(laListWidget* lst)
{
    GFX_Rect listArea;
    
    _laListWidget_GetListRect(lst, &listArea);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)lst),
		                  &listArea,
					      LA_FALSE);
}

static void invalidateRow(laListWidget* lst, int32_t row)
{
    GFX_Rect rowArea, listArea, clipRect;
    
    _laListWidget_GetRowRect(lst, row, &rowArea);
    _laListWidget_GetListRect(lst, &listArea);
    
    if(GFX_RectIntersects(&rowArea, &listArea) == GFX_TRUE)
    {
        GFX_RectClip(&listArea, &rowArea, &clipRect);
        
        laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)lst),
		                      &clipRect,
					          LA_FALSE);  
    }
}

static void languageChanging(laListWidget* lst)
{
    uint32_t i;
    
    for(i = 0; i < lst->items.size; i++)
    {
        laString* item = lst->items.values[i];
        
        if(laString_IsEmpty(item) == LA_FALSE)
        {
            invalidateListArea(lst);
            
            return;
        }
    }
}

void _laListWidget_Constructor(laListWidget* lst)
{
    _laWidget_Constructor((laWidget*)lst);
    
    lst->widget.destructor = (laWidget_Destructor_FnPtr)&_laListWidget_Destructor;

    lst->widget.languageChangeEvent = (laWidget_LanguageChangingEvent_FnPtr)&languageChanging;

    lst->widget.type = LA_WIDGET_LIST;

    // override base class methods
    lst->widget.paint = (laWidget_Paint_FnPtr)&_laListWidget_Paint;
    lst->widget.resized = (laWidget_Resized_FnPtr)&resized;

    lst->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&touchDown;
    lst->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&touchUp;
    lst->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&touchMoved;

    lst->widget.rect.width = DEFAULT_WIDTH;
    lst->widget.rect.height = DEFAULT_HEIGHT;

    lst->widget.borderType = LA_WIDGET_BORDER_BEVEL;
    lst->widget.backgroundType = LA_WIDGET_BACKGROUND_FILL;
    lst->itemDown = -1;
    //lst->allowEmpty = LA_TRUE;
    lst->mode = LA_LIST_WIDGET_SELECTION_MODE_CONTIGUOUS;
    
    laArray_Create(&lst->items);
    
    // create scrollbar
    
    lst->scrollbar = laScrollBarWidget_New();
    laScrollBarWidget_SetValueChangedEventCallback(lst->scrollbar, &scrollBarMoved);
    
    laWidget_AddChild((laWidget*)lst, (laWidget*)lst->scrollbar);
    
    laWidget_SetSize((laWidget*)lst->scrollbar,
                     DEFAULT_SCROLL_WIDTH,
                     lst->widget.rect.height);
                     
    laWidget_SetPosition((laWidget*)lst->scrollbar,
                         lst->widget.rect.width - DEFAULT_SCROLL_WIDTH,
                         0);
                         
    laScrollBarWidget_SetStepSize(lst->scrollbar, DEFAULT_STEP_SIZE);
                         
    recalculateScrollBarValues(lst);
    
    lst->halign = LA_HALIGN_LEFT;
    lst->iconMargin = DEFAULT_MARGIN;
}

void _laListWidget_Destructor(laListWidget* lst)
{
    // free all item memory
    laListWidget_RemoveAllItems(lst);
    laArray_Destroy(&lst->items);
    
    // free any existing memory reader
    if(lst->reader != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(lst->reader);
        
        lst->reader = NULL;
    }

    _laWidget_Destructor((laWidget*)lst);
}

laListWidget* laListWidget_New()
{
    laListWidget* lst = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    lst = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laListWidget));

    _laListWidget_Constructor(lst);

    return lst;
}

uint32_t getRowAtPoint(laListWidget* lst, GFX_Point* pnt)
{
    uint32_t idx;
    GFX_Rect row;
    
    for(idx = 0; idx < lst->items.size; idx++)
    {
        _laListWidget_GetRowRect(lst, idx, &row);
        
        if(GFX_RectContainsPoint(&row, pnt) == GFX_TRUE)
            return idx;
    }
    
    return -1;
}

static void resized(laListWidget* lst)
{
    laWidget_SetSize((laWidget*)lst->scrollbar,
                     lst->scrollbar->widget.rect.width,
                     lst->widget.rect.height);
                     
    laWidget_SetPosition((laWidget*)lst->scrollbar,
                         lst->widget.rect.width - lst->scrollbar->widget.rect.width,
                         0);
}

static void recalculateScrollBarValues(laListWidget* lst)
{
    GFX_Rect totalRect;
    
    _laListWidget_GetLogicalRect(lst, &totalRect);
    
    if(totalRect.height <= lst->widget.rect.height)
    {
        if(laWidget_GetVisible((laWidget*)lst->scrollbar) == GFX_TRUE)
            laWidget_Invalidate((laWidget*)lst);
    
        laWidget_SetVisible((laWidget*)lst->scrollbar, LA_FALSE);
    }
    else
    {
        if(laWidget_GetVisible((laWidget*)lst->scrollbar) == GFX_FALSE)
            laWidget_Invalidate((laWidget*)lst);
            
        laWidget_SetVisible((laWidget*)lst->scrollbar, LA_TRUE);
        laScrollBarWidget_SetMaximumValue(lst->scrollbar, totalRect.height - lst->widget.rect.height);
    }
}

static int32_t getScrollBarRowLocation(laListWidget* lst, int idx)
{
    uint32_t row = 0;
    GFX_Rect rect;
    int32_t height = 0;
    int32_t y = 0;
    
    int32_t max = lst->scrollbar->max;
    
    float percent;
    
    // avoid divide by zero
    if(max == 0)
        return 0;
        
    if(idx == lst->items.size - 1)
        return max;
    
    for(row = 0; row < lst->items.size; row++)
    {
        _laListWidget_GetRowRect(lst, row, &rect);
        
        if(row == idx)
            y = height;
        
        height += rect.height;
    }
    
    if(height == 0)
        return 0;
    
    percent = (float)y / (float)height;
    
    percent *= (float)max;
    
    return (int32_t)percent;
}

static void scrollBarMoved(laScrollBarWidget* bar)
{
    invalidateListArea((laListWidget*)bar->widget.parent);
}

static void touchDown(laListWidget* lst, laInput_TouchDownEvent* evt)
{
    GFX_Point pnt;
    uint32_t idx;
    
    pnt.x = evt->x;
    pnt.y = evt->y;
    
    idx = getRowAtPoint(lst, &pnt);
    
    if(idx >= 0 && idx < lst->items.size)
    {
        lst->itemDown = idx;
        
        invalidateRow(lst, idx);
    }
    else if(lst->itemDown != -1)
    {
        lst->itemDown = -1;
     
        invalidateRow(lst, idx);
    }   
    
    evt->event.accepted = LA_TRUE;
}

static void touchUp(laListWidget* lst, laInput_TouchUpEvent* evt)
{
    GFX_Point pnt;
    uint32_t idx;
    
    pnt.x = evt->x;
    pnt.y = evt->y;
    
    idx = getRowAtPoint(lst, &pnt);
    
    if(idx == lst->itemDown)
        laListWidget_ToggleItemSelected(lst, idx); 
    else if(lst->itemDown != -1)
        invalidateRow(lst, lst->itemDown);
    
    lst->itemDown = -1;

    evt->event.accepted = LA_TRUE;
}

static void touchMoved(laListWidget* lst, laInput_TouchMovedEvent* evt)
{
    GFX_Point pnt;
    uint32_t idx;
    
    pnt.x = evt->x;
    pnt.y = evt->y;
    
    idx = getRowAtPoint(lst, &pnt);
    
    if(idx >= 0 && idx < lst->items.size)
    {
        if(lst->itemDown != idx)
        {
            if (lst->itemDown != -1)
            {
                invalidateRow(lst, lst->itemDown);
            }
            
            lst->itemDown = idx;
        
            invalidateRow(lst, lst->itemDown);
        }
    }
    else if(lst->itemDown != -1)
    {
        invalidateRow(lst, lst->itemDown);
        
        lst->itemDown = -1;
    }   
    
    evt->event.accepted = LA_TRUE;
}

laListWidget_SelectionMode laListWidget_GetSelectionMode(laListWidget* lst)
{
    if(lst == NULL)
        return LA_LIST_WIDGET_SELECTION_MODE_SINGLE;
        
    return lst->mode;
}

laResult laListWidget_SetSelectionMode(laListWidget* lst, 
                                       laListWidget_SelectionMode mode)
{
    if(lst == NULL)
        return LA_FAILURE;
        
    if(lst->mode == mode)
        return LA_SUCCESS;
        
    lst->mode = mode;
    
    laListWidget_DeselectAll(lst);
        
    return LA_SUCCESS;
}

laBool laListWidget_GetAllowEmptySelection(laListWidget* lst)
{
    if(lst == NULL)
        return LA_FALSE;
    
    return lst->allowEmpty;
}

laResult laListWidget_SetAllowEmptySelection(laListWidget* lst, laBool allow)
{
    if(lst == NULL)
        return LA_FAILURE;
        
    if(lst->allowEmpty == allow)
        return LA_SUCCESS;
        
    lst->allowEmpty = allow;
    
    // automatically select if needed
    if(allow == LA_FALSE && laListWidget_GetSelectionCount(lst) == 0)
        laListWidget_SetItemSelected(lst, 0, LA_TRUE);
        
    return LA_SUCCESS;
}

laHAlignment laListWidget_GetAlignment(laListWidget* lst)
{
    if(lst == NULL)
        return 0;

    return lst->halign;
}

laResult laListWidget_SetAlignment(laListWidget* lst, laHAlignment align)
{
    if(lst == NULL)
        return LA_FAILURE;
        
    if(lst->halign == align)
        return LA_SUCCESS;

    lst->halign = align;

    invalidateListArea(lst);
    
    return LA_SUCCESS;
}

laRelativePosition laListWidget_GetIconPosition(laListWidget* lst)
{
    if(lst == NULL)
        return 0;

    return lst->iconPos;
}
                                               
laResult laListWidget_SetIconPosition(laListWidget* lst,
                                      laRelativePosition pos)
{
    if(lst == NULL ||
       (pos != LA_RELATIVE_POSITION_LEFTOF && pos != LA_RELATIVE_POSITION_RIGHTOF))
        return LA_FAILURE;
        
    if(lst->iconPos == pos)
        return LA_SUCCESS;

    lst->iconPos = pos;

    invalidateListArea(lst);
    
    return LA_SUCCESS;
}

uint32_t laListWidget_GetIconMargin(laListWidget* lst)
{
    if(lst == NULL)
        return 0;

    return lst->iconMargin;
}
                                               
laResult laListWidget_SetIconMargin(laListWidget* lst, uint32_t mg)
{
    if(lst == NULL)
        return LA_FAILURE;
        
    if(lst->iconMargin == mg)
        return LA_SUCCESS;

    lst->iconMargin = mg;

    invalidateListArea(lst);
    
    return LA_SUCCESS;
}
                                                        
uint32_t laListWidget_GetItemCount(laListWidget* lst)
{
    if(lst == NULL)
        return 0;

    return lst->items.size;
}

uint32_t laListWidget_AppendItem(laListWidget* lst)
{
    laListItem* item;
    
    if(lst == NULL)
        return -1;
        
    item = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laListItem));
    
    if(item == NULL)
        return -1;
        
    item->enabled = LA_TRUE;
        
    laString_Initialize(&item->string);
    
    laArray_PushBack(&lst->items, item);
    
    if(lst->allowEmpty == LA_FALSE && laListWidget_GetSelectionCount(lst) == 0)
        laListWidget_SetItemSelected(lst, lst->items.size - 1, LA_TRUE);

    _laListWidget_recalculateRowRect(lst, lst->items.size - 1);

    recalculateScrollBarValues(lst);
        
    return lst->items.size - 1;
}

uint32_t laListWidget_InsertItem(laListWidget* lst, uint32_t idx)
{
    laListItem* item;
    
    if(lst == NULL)
        return -1;
        
    item = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laListItem));
    
    if(item == NULL)
        return -1;
        
    item->enabled = LA_TRUE;
        
    laString_Initialize(&item->string);
    
    laArray_InsertAt(&lst->items, idx, item);
    
    if(lst->allowEmpty == LA_FALSE && laListWidget_GetSelectionCount(lst) == 0)
        laListWidget_SetItemSelected(lst, idx, LA_TRUE);

    _laListWidget_recalculateRowRect(lst, idx);

    recalculateScrollBarValues(lst);
        
    return idx;
}

laResult laListWidget_RemoveItem(laListWidget* lst, uint32_t idx)
{
    laResult res;
    laListItem* item;
    
    if(lst == NULL)
        return LA_FAILURE;
        
    item = lst->items.values[idx];
    
    if(item == NULL)
        return LA_FAILURE;
    
    laString_Destroy(&item->string);
        
    res = laArray_RemoveAt(&lst->items, idx);
    
    laContext_GetActive()->memIntf.heap.free(item);

    if(res == LA_SUCCESS)
        invalidateListArea(lst);
        
    recalculateScrollBarValues(lst);
    
    return res;
}

laResult laListWidget_RemoveAllItems(laListWidget* lst)
{
    laListItem* item;
    uint32_t i;

    if(lst == NULL)
        return LA_FAILURE;

    for(i = 0; i < lst->items.size; i++)
    {
        item = lst->items.values[i];
    
        laString_Destroy(&item->string);
    
        if(item != NULL)
            laContext_GetActive()->memIntf.heap.free(item);
        
    }
        
    laArray_Clear(&lst->items);
        
    laWidget_Invalidate((laWidget*)lst);
        
    return LA_SUCCESS;
}

laBool laListWidget_GetItemSelected(laListWidget* lst, uint32_t idx)
{
    laListItem* item;
    
    if(lst == NULL)
        return LA_FALSE;
    
    item = (laListItem*)lst->items.values[idx];
    
    if(item == NULL)
        return LA_FALSE;
        
    return item->selected;
}

laResult laListWidget_SetItemSelected(laListWidget* lst,
                                      uint32_t idx,
                                      laBool selected)
{
    laListItem *item, *old;
    uint32_t firstIdx, lastIdx, count;
    
    if(lst == NULL || idx == -1 || idx >= lst->items.size)
        return LA_FAILURE;
        
    count = laListWidget_GetSelectionCount(lst);
        
    item = (laListItem*)lst->items.values[idx];
        
    if(item == NULL)
        return LA_FAILURE;
        
    //Prevent a disabled item from being selected
    if (item->enabled == LA_FALSE)
    {
        selected = LA_FALSE;
    }
        
    // nothing to do
    if(item->selected == selected)
        return LA_SUCCESS;
        
    if(lst->mode == LA_LIST_WIDGET_SELECTION_MODE_SINGLE)
    {
        if(lst->allowEmpty == LA_FALSE && selected == LA_FALSE)
            return LA_FAILURE;
          
        if(count > 0)
        {
            firstIdx = laListWidget_GetFirstSelectedItem(lst);
            
            old = lst->items.values[firstIdx];
            
            old->selected = LA_FALSE;
            
            invalidateRow(lst, firstIdx);
            //laListWidget_DeselectAll(lst);
        }
        
        item->selected = selected;
        
        if(lst->cb != NULL)
            lst->cb(lst, idx, selected);
            
        invalidateRow(lst, idx);
    }
    else if(lst->mode == LA_LIST_WIDGET_SELECTION_MODE_MULTIPLE)
    {
        if(lst->allowEmpty == LA_FALSE &&
           selected == LA_FALSE &&
           laListWidget_GetSelectionCount(lst) == 1)
            return LA_FAILURE;
            
        item = lst->items.values[idx];
        
        item->selected = selected;
        
        if(lst->cb != NULL)
            lst->cb(lst, idx, selected);
            
        invalidateRow(lst, idx);
    }
    else if(lst->mode == LA_LIST_WIDGET_SELECTION_MODE_CONTIGUOUS)
    {
        if(lst->allowEmpty == LA_FALSE &&
           selected == LA_FALSE &&
           laListWidget_GetSelectionCount(lst) == 1)
            return LA_FAILURE;
            
        // empty selection case
        if(count == 0 && selected == LA_TRUE)
        {
            item->selected = selected;
            
            if(lst->cb != NULL)
                lst->cb(lst, idx, selected);
                
            invalidateRow(lst, idx);
        }
        // test range
        else
        {
            firstIdx = laListWidget_GetFirstSelectedItem(lst);
            lastIdx = laListWidget_GetLastSelectedItem(lst);
        
            if(selected == LA_TRUE)
            {
                // selection is contiguous, add to selection        
                if(idx == firstIdx - 1 || idx == lastIdx + 1)
                {
                    item->selected = selected;
                
                    if(lst->cb != NULL)
                        lst->cb(lst, idx, selected);
                        
                    invalidateRow(lst, idx);
                }
                // outside of range, deselect all and select target
                else
                {
                    if (item->enabled == LA_TRUE)
                    {
                        laListWidget_DeselectAll(lst);

                        item->selected = selected;

                        if(lst->cb != NULL)
                            lst->cb(lst, idx, selected);

                        invalidateListArea(lst);
                    }
                }
            }
            else
            {
                // selection is contiguous, remove from end caps        
                if(idx == firstIdx || idx == lastIdx)
                {
                    item->selected = selected;
                
                    if(lst->cb != NULL)
                        lst->cb(lst, idx, selected);
                        
                    invalidateListArea(lst);
                }
                // outside of range, deselect all and select target
                else
                {
                    if (item->enabled == LA_TRUE)
                    {
                        laListWidget_DeselectAll(lst);

                        if(lst->allowEmpty == LA_FALSE)
                        {
                            item->selected = LA_TRUE;

                            if(lst->cb != NULL)
                                lst->cb(lst, idx, LA_TRUE);

                            invalidateListArea(lst);
                        }
                    }
                }
            }
        }
    }
        
    return LA_SUCCESS;
}
                                                 
laResult laListWidget_ToggleItemSelected(laListWidget* lst,
                                         uint32_t idx)
{
    laListItem* item;
    
    if(lst == NULL || idx >= lst->items.size)
        return LA_FAILURE;
    
    item = (laListItem*)lst->items.values[idx];
    
    if(item == NULL)
        return LA_FAILURE;
        
    return laListWidget_SetItemSelected(lst, idx, !item->selected);
}

laResult laListWidget_SelectAll(laListWidget* lst)
{
    laListItem* item;
    uint32_t i;
    uint32_t count = 0;
    
    if(lst == NULL)
        return LA_FAILURE;
        
    if(lst->mode == LA_LIST_WIDGET_SELECTION_MODE_SINGLE)
    {
        laListWidget_SetItemSelected(lst, 0, LA_TRUE);
        
        return LA_SUCCESS;
    }
    
    for(i = 0; i < lst->items.size; i++)
    {
        item = lst->items.values[i];
        
        if (item->enabled == LA_TRUE)
        {
            if(item->selected == LA_FALSE)
            {
                item->selected = LA_TRUE;

                if(lst->cb != NULL)
                    lst->cb(lst, i, LA_TRUE);

                count++;
            }
        }
    }
    
    if(count > 0)
        invalidateListArea(lst);
        
    return LA_SUCCESS;
}

laResult laListWidget_DeselectAll(laListWidget* lst)
{
    laListItem* item;
    uint32_t i;
    uint32_t count = 0;
    
    if(lst == NULL)
        return LA_FAILURE;
    
    for(i = 0; i < lst->items.size; i++)
    {
        item = lst->items.values[i];
        
        if(item->selected == LA_TRUE)
        {
            item->selected = LA_FALSE;
            
            if(lst->cb != NULL)
                lst->cb(lst, i, LA_FALSE);
                
            count++;
        }
    }
    
    if(count > 0)
        invalidateListArea(lst);
        
    return LA_SUCCESS;
}

uint32_t laListWidget_GetFirstSelectedItem(laListWidget* lst)
{
    uint32_t idx;
    laListItem* item;
    
    for(idx = 0; idx < lst->items.size; idx++)
    {
        item = lst->items.values[idx];
        
        if(item->selected == LA_TRUE)
            return idx;
    }
    
    return -1;
}

uint32_t laListWidget_GetLastSelectedItem(laListWidget* lst)
{
    uint32_t idx;
    uint32_t last = -1;
    laListItem* item;
    
    for(idx = 0; idx < lst->items.size; idx++)
    {
        item = lst->items.values[idx];
        
        if(item->selected == LA_TRUE)
            last = idx;
    }
    
    return last;
}

uint32_t laListWidget_GetSelectionCount(laListWidget* lst)
{
    uint32_t idx, count = 0;
    laListItem* item;
    
    for(idx = 0; idx < lst->items.size; idx++)
    {
        item = lst->items.values[idx];
        
        if(item->selected == LA_TRUE)
            count++;
    }
    
    return count;
}



laResult laListWidget_GetItemText(laListWidget* lst, uint32_t idx, laString* str)
{
    laListItem* item;
    
    if(lst == NULL || idx >= lst->items.size)
        return LA_FAILURE;
        
    item = lst->items.values[idx];
    
    return laString_Copy(str, &item->string);
}
                                               
laResult laListWidget_SetItemText(laListWidget* lst,
                                  uint32_t idx,
                                  laString str)
{
    laListItem* item;
    
    if(lst == NULL || idx >= lst->items.size)
        return LA_FAILURE;
       
    item = lst->items.values[idx];
    
    laString_Copy(&item->string, &str);
    
    _laListWidget_recalculateRowRect(lst, idx);

    invalidateListArea(lst);
    
    recalculateScrollBarValues(lst);
    
    return LA_SUCCESS;
}
                                               
GFXU_ImageAsset* laListWidget_GetItemIcon(laListWidget* lst,
                                          uint32_t idx)
{
    laListItem* item;
    
    if(lst == NULL || idx >= lst->items.size)
        return NULL;
        
    item = lst->items.values[idx];
        
    return item->icon;
}
                                                              
laResult laListWidget_SetItemIcon(laListWidget* lst,
                                  uint32_t idx,
                                  GFXU_ImageAsset* img)
{
    laListItem* item;
    
    if(lst == NULL || idx >= lst->items.size)
        return LA_FAILURE;
        
    item = lst->items.values[idx];
        
    item->icon = img;
    
    _laListWidget_recalculateRowRect(lst, idx);

    recalculateScrollBarValues(lst);
    
    return LA_SUCCESS;
}

laBool laListWidget_GetItemEnable(laListWidget* lst,
                                    uint32_t idx)
{
    laListItem* item;
    
    if(lst == NULL || idx >= lst->items.size)
        return LA_FAILURE;
        
    item = lst->items.values[idx];
    
    return item->enabled;
}

laResult laListWidget_SetItemEnable(laListWidget* lst,
                                    uint32_t idx,
                                    laBool enable)
{
    laListItem* item;
    
    if(lst == NULL || idx >= lst->items.size)
        return LA_FAILURE;
        
    item = lst->items.values[idx];
    
    if (item->enabled == enable)
        return LA_SUCCESS;
        
    item->enabled = enable;
    
    if (enable == LA_FALSE)
    {
        item->selected = LA_FALSE;
    }
    
    invalidateListArea(lst);
    
    return LA_SUCCESS;
}

laResult laListWidget_SetItemVisible(laListWidget* lst,
                                     uint32_t idx)
{
    int32_t y;
    
    if(lst == NULL || idx >= lst->items.size)
        return LA_FAILURE;
        
    y = getScrollBarRowLocation(lst, idx);
    
    laScrollBarWidget_SetScrollValue(lst->scrollbar, y);
    
    return LA_SUCCESS;
}

laListWidget_SelectedItemChangedEvent laListWidget_GetSelectedItemChangedEventCallback(laListWidget* lst)
{
    if(lst == NULL)
        return NULL;

    return lst->cb;
}

laResult laListWidget_SetSelectedItemChangedEventCallback(laListWidget* lst,
                                                          laListWidget_SelectedItemChangedEvent cb)
{
    if(lst == NULL || lst->cb == cb)
        return LA_FAILURE;

    lst->cb = cb;
    
    return LA_SUCCESS;
}

#endif // LA_LIST_WIDGET_ENABLED && LA_SCROLLBAR_WIDGET_ENABLED
