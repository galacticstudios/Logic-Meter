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


#include "gfx/libaria/inc/libaria_widget_touchtest.h"

#if LA_TOUCHTEST_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          100

void _laTouchTestWidget_TouchDownEvent(laTouchTestWidget* widget, laInput_TouchDownEvent* evt);
void _laTouchTestWidget_TouchUpEvent(laTouchTestWidget* widget, laInput_TouchUpEvent* evt);
void _laTouchTestWidget_TouchMovedEvent(laTouchTestWidget* widget, laInput_TouchMovedEvent* evt);

static void invalidateLineList(laTouchTestWidget* tch)
{
    uint32_t idx;
    GFX_Rect horzRect, vertRect;
    
    for(idx = 0; idx < tch->size; idx++)
    {
        _laTouchTestWidget_GetLineRects(tch, idx, &horzRect, &vertRect);
        
        laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)tch),
                              &horzRect,
                              GFX_TRUE);
                              
        laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)tch),
                              &vertRect,
                              GFX_TRUE);
    }
}

void _laTouchTestWidget_Constructor(laTouchTestWidget* tch)
{
    _laWidget_Constructor((laWidget*)tch);
    
    tch->widget.destructor = (laWidget_Destructor_FnPtr)&_laTouchTestWidget_Destructor;

    tch->widget.type = LA_WIDGET_TOUCHTEST;
    tch->state = LA_TOUCHTEST_STATE_UP;

    // override base class methods
    tch->widget.paint = (laWidget_Paint_FnPtr)&_laTouchTestWidget_Paint;

    tch->widget.rect.width = DEFAULT_WIDTH;
    tch->widget.rect.height = DEFAULT_HEIGHT;

    tch->widget.borderType = LA_WIDGET_BORDER_NONE;
    tch->widget.backgroundType = LA_WIDGET_BACKGROUND_FILL;
    
    tch->size = 0;
    tch->start = 0;
    tch->next = 0;

    tch->widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&_laTouchTestWidget_TouchDownEvent;
    tch->widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&_laTouchTestWidget_TouchUpEvent;
    tch->widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&_laTouchTestWidget_TouchMovedEvent;
}

void _laTouchTestWidget_Destructor(laTouchTestWidget* tch)
{
    _laWidget_Destructor((laWidget*)tch);
}

/*void _laTouchTestWidget_Update(laTouchTestWidget* tch)
{

}*/

laTouchTestWidget* laTouchTestWidget_New()
{
    laTouchTestWidget* tch = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    tch = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laTouchTestWidget));

    if(tch == NULL)
        return NULL;
    
    _laTouchTestWidget_Constructor(tch);

    return tch;
}

laResult laTouchTest_AddPoint(laTouchTestWidget* tch, GFX_Point* pnt)
{
    if(tch == NULL)
        return LA_FAILURE;
        
    invalidateLineList(tch);
        
    if(tch->size > 0 && tch->next == tch->start)
        tch->start++;
        
    if(tch->size < LA_TOUCHTEST_MEMORY_SIZE)
        tch->size++;
        
    tch->pnts[tch->next] = *pnt;
    
    //printf("adding point %i: %i, %i\n", tch->next, pnt->x, pnt->y);
        
    tch->next += 1;
    
    if(tch->next >= LA_TOUCHTEST_MEMORY_SIZE)
        tch->next = 0;
        
    if(tch->cb != NULL)
        tch->cb(tch, pnt);
        
    invalidateLineList(tch);
        
    return LA_SUCCESS;
}

laResult laTouchTest_ClearPoints(laTouchTestWidget* tch)
{
    if(tch == NULL)
        return LA_FAILURE;
    
    invalidateLineList(tch);
    
    tch->start = 0;
    tch->size = 0;
    tch->next = 0;

    invalidateLineList(tch);
        
    return LA_SUCCESS;
}

laTouchTestWidget_PointAddedEventCallback laTouchTestWidget_GetPointAddedEventCallback(laTouchTestWidget* txt)
{
    if(txt == NULL)
        return NULL;

    return txt->cb;
}

laResult laTouchTestWidget_SetPointAddedEventCallback(laTouchTestWidget* txt,
                                                      laTouchTestWidget_PointAddedEventCallback cb)
{
    if(txt == NULL)
        return LA_FAILURE;

    txt->cb = cb;
    
    return LA_SUCCESS;
}

void _laTouchTestWidget_TouchDownEvent(laTouchTestWidget* tch,
                                       laInput_TouchDownEvent* evt)
{
    GFX_Point pnt;
    
    pnt.x = evt->x;
    pnt.y = evt->y;
    
    evt->event.accepted = LA_TRUE;

    tch->state = LA_TOUCHTEST_STATE_DOWN;
    
    laUtils_PointScreenToLocalSpace((laWidget*)tch, &pnt);
    laTouchTest_AddPoint(tch, &pnt);

    //printf("tch touch down\n");
}

void _laTouchTestWidget_TouchUpEvent(laTouchTestWidget* tch,
                                     laInput_TouchUpEvent* evt)
{
    evt->event.accepted = LA_TRUE;
    tch->state = LA_TOUCHTEST_STATE_UP;

    //printf("tch touch up\n");
    
    laTouchTest_ClearPoints(tch);
}

void _laTouchTestWidget_TouchMovedEvent(laTouchTestWidget* tch,
                                        laInput_TouchMovedEvent* evt)
{
    GFX_Rect rect = laUtils_WidgetLayerRect((laWidget*)tch);
    GFX_Point pnt;

    pnt.x = evt->x;
    pnt.y = evt->y;

    evt->event.accepted = LA_TRUE;

    if(GFX_RectContainsPoint(&rect, &pnt) == GFX_TRUE)
    {
        laUtils_PointScreenToLocalSpace((laWidget*)tch, &pnt);
        laTouchTest_AddPoint(tch, &pnt);
    }
}

#endif // LA_TOUCHTEST_WIDGET_ENABLED