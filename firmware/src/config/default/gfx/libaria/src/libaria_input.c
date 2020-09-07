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


#include "gfx/libaria/inc/libaria_input.h"
#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_screen.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

//#define INPUT_EVENT_DEBUG

laBool laInput_GetEnabled()
{
    laContext* context = laContext_GetActive();
    
    if(context == NULL)
        return LA_FALSE;

    return context->input.enabled;
}

laResult laInput_SetEnabled(laBool enable)
{
    laContext* context = laContext_GetActive();
    
    if(context == NULL)
        return LA_FAILURE;

    context->input.enabled = enable;

    return LA_SUCCESS;
}

GFX_Point transformCoordinate(int32_t x, int32_t y)
{
    GFX_Point pnt;
    GFX_Rect rect;
    GFX_Orientation orientation;
    GFX_Bool mirror;
    
    // reorient touch coordinates if the user interface is rotated
    pnt.x = x;
    pnt.y = y;
    
    rect = laContext_GetScreenRect();
    
    GFX_Get(GFXF_ORIENTATION, &orientation);
    GFX_Get(GFXF_MIRRORED, &mirror);
    
    pnt = laUtils_ScreenToOrientedSpace(&pnt, &rect, orientation);
    
    if(mirror == GFX_TRUE)
        pnt = laUtils_ScreenToMirroredSpace(&pnt, &rect, orientation);
        
    return pnt;
}

laResult laInput_InjectTouchDown(uint32_t id, int32_t x, int32_t y)
{
    laInput_TouchDownEvent* evt;
    laContext* context = laContext_GetActive();
    GFX_Point pnt;

    if(context == NULL ||                          // need a valid context
       context->input.enabled == LA_FALSE ||       // inputs are disabled
       id >= LA_MAX_TOUCH_STATES ||                // don't overrun array
       context->input.touch[id].valid == LA_TRUE)  // touch id must be invalid
        return GFX_FAILURE;

    // reorient touch coordinates if the user interface is rotated
    pnt = transformCoordinate(x, y);

    // dispatch the event
    context->input.touch[id].valid = LA_TRUE;
    context->input.touch[id].x = pnt.x;
    context->input.touch[id].y = pnt.y;

    evt = context->memIntf.heap.calloc(1, sizeof(laInput_TouchDownEvent));

    if(evt == NULL)
        return GFX_FAILURE;
    
    evt->event.id = LA_EVENT_TOUCH_DOWN;
    evt->touchID = id;
    evt->x = pnt.x;
    evt->y = pnt.y;

#ifdef INPUT_EVENT_DEBUG
    printf("adding touch down event %i, %i\n", evt->x, evt->y);
#endif

    if(laEvent_AddEvent((laEvent*)evt) == GFX_FAILURE)
    {
        context->memIntf.heap.free(evt);

        return GFX_FAILURE;
    }    

    return GFX_SUCCESS;
}

laResult laInput_InjectTouchUp(uint32_t id, int32_t x, int32_t y)
{
    laInput_TouchUpEvent* evt;
    laContext* context = laContext_GetActive();
    GFX_Point pnt;

    if(context == NULL ||                           // need a valid context
       id >= LA_MAX_TOUCH_STATES ||                 // don't overrun array
       context->input.touch[id].valid == LA_FALSE)  // touch id must be valid
        return GFX_FAILURE;

    context->input.touch[id].valid = LA_FALSE;

    // reorient touch coordinates if the user interface is rotated
    pnt = transformCoordinate(x, y);

    // dispatch event
    evt = context->memIntf.heap.calloc(1, sizeof(laInput_TouchUpEvent));

    if(evt == NULL)
        return GFX_FAILURE;
    
    evt->event.id = LA_EVENT_TOUCH_UP;
    evt->touchID = id;
    evt->x = pnt.x;
    evt->y = pnt.y;
    
#ifdef INPUT_EVENT_DEBUG
    printf("adding touch up event %i, %i\n", evt->x, evt->y);
#endif

    if(laEvent_AddEvent((laEvent*)evt) == GFX_FAILURE)
    {
        context->memIntf.heap.free(evt);

        return GFX_FAILURE;
    }    

    return GFX_SUCCESS;
}

laResult laInput_InjectTouchMoved(uint32_t id, int32_t x, int32_t y)
{
    laInput_TouchMovedEvent* evt;
    laEvent* evtPtr;
    laContext* context = laContext_GetActive();
    GFX_Point pnt;
    laListNode* node;

    if(context == NULL ||                           // need a valid context
       id >= LA_MAX_TOUCH_STATES ||                 // don't overrun array
       context->input.touch[id].valid == LA_FALSE)  // touch id must be valid
        return GFX_FAILURE;
        
    // find any existing touch moved event and overwrite
    node = context->event.events.head;
    
    while(node != NULL)
    {
        evtPtr = (laEvent*)node->val;
        
        if(evtPtr->id == LA_EVENT_TOUCH_MOVED)
        {
            evt = (laInput_TouchMovedEvent*)evtPtr;
         
            if(evt->touchID == id)
            {   
#ifdef INPUT_EVENT_DEBUG
                printf("overwriting previous move event\n");
#endif                
                
                // reorient touch coordinates if the user interface is rotated
                pnt = transformCoordinate(x, y);
                
                evt->x = x;
                evt->y = y;
                
                evt->x = pnt.x;
                evt->y = pnt.y;

                context->input.touch[id].x = pnt.x;
                context->input.touch[id].y = pnt.y;
                
                return LA_SUCCESS;
            }
        }
        
        node = node->next;
    }

    evt = context->memIntf.heap.calloc(1, sizeof(laInput_TouchMovedEvent));

    if(evt == NULL)
        return GFX_FAILURE;

    // reorient touch coordinates if the user interface is rotated
    pnt = transformCoordinate(x, y);

    evt->event.id = LA_EVENT_TOUCH_MOVED;
    evt->touchID = id;
    evt->prevX = context->input.touch[id].x;
    evt->prevY = context->input.touch[id].y;
    evt->x = pnt.x;
    evt->y = pnt.y;

    context->input.touch[id].x = pnt.x;
    context->input.touch[id].y = pnt.y;

#ifdef INPUT_EVENT_DEBUG
    printf("adding move event %i, %i, %i, %i\n", evt->x, evt->y, evt->prevX, evt->prevY);
#endif

    if(laEvent_AddEvent((laEvent*)evt) == GFX_FAILURE)
    {
        context->memIntf.heap.free(evt);

        return GFX_FAILURE;
    }    

    return GFX_SUCCESS;
}

laEventResult handleTouchDown(laInput_TouchDownEvent* evt)
{
    laContext* context = laContext_GetActive();
    laScreen* screen;
    laLayer* layer;
    laWidget* targetWidget = NULL;
    int32_t i;
    
    // get the active screen
    screen = context->activeScreen;
    
    // find the topmost widget on the topmost layer for the touch event
    for(i = LA_MAX_LAYERS - 1; i >= 0; i--)
    {
        layer = screen->layers[i];
        
        // layer must exist and be enabled
        if(layer == NULL || layer->widget.enabled == LA_FALSE)
           continue;
           
        targetWidget = laUtils_PickFromLayer(layer, evt->x, evt->y);
             
        if(targetWidget != NULL)
        {                                
            if(targetWidget == (laWidget*)layer)
            {
                if(layer->allowInputPassThrough == LA_FALSE)
                    break;
            }
            else
                break;
        }
    }

    // no widget qualified
    if(targetWidget == NULL)
    {
        laContext_SetFocusWidget(NULL);
        return LA_EVENT_HANDLED;
    }

    layer = laUtils_GetLayer(targetWidget);
    
    if(layer->frameState > LA_LAYER_FRAME_PREFRAME)
        return LA_EVENT_DEFERRED;

    while(targetWidget != NULL)
    {
        targetWidget->touchDown(targetWidget, evt);
        
        if(evt->event.accepted == LA_TRUE)
            break;

        targetWidget = targetWidget->parent;
    }

    laContext_SetFocusWidget(targetWidget);
    
#ifdef INPUT_EVENT_DEBUG  
    printf("handling touch down event %i, %i\n", evt->x, evt->y);
#endif
    
    return LA_EVENT_HANDLED;
}

laBool handleTouchUp(laInput_TouchUpEvent* evt)
{
    laContext* context = laContext_GetActive();
    laLayer* layer;
    
    if(context->focus == NULL)
        return LA_EVENT_HANDLED;
        
    layer = laUtils_GetLayer(context->focus);
        
    if(layer->frameState <= LA_LAYER_FRAME_PREFRAME)
    {
        context->focus->touchUp(context->focus, evt);

#ifdef INPUT_EVENT_DEBUG
        printf("handling touch up event %i, %i\n", evt->x, evt->y);
#endif       
        
        return LA_EVENT_HANDLED;
    }
    
    return LA_EVENT_DEFERRED;
}

laBool handleTouchMoved(laInput_TouchMovedEvent* evt)
{
    laContext* context = laContext_GetActive();
    laLayer* layer;

    if(context->focus == NULL)
        return LA_EVENT_HANDLED;
        
    layer = laUtils_GetLayer(context->focus);
        
    if(layer->frameState <= LA_LAYER_FRAME_PREFRAME)
    {
        context->focus->touchMoved(context->focus, evt);

#ifdef INPUT_EVENT_DEBUG        
        printf("handling touch move event %i, %i, %i, %i\n", evt->x, evt->y, evt->prevX, evt->prevY);
#endif
        
        return LA_EVENT_HANDLED;
    }

    return LA_EVENT_DEFERRED;
}

laEventResult _laInput_HandleInputEvent(laEvent* evt)
{
    //if(laContext_GetActive()->input.inputTarget == NULL)
    //    return LA_SUCCESS;

    switch(evt->id)
    {
        case LA_EVENT_TOUCH_DOWN:
        {
            return handleTouchDown((laInput_TouchDownEvent*)evt);

#ifdef INPUT_EVENT_DEBUG                
            printf("handled touch down\n");
#endif                
                
            break;
        }
        case LA_EVENT_TOUCH_UP:
        {
            return handleTouchUp((laInput_TouchUpEvent*)evt);

#ifdef INPUT_EVENT_DEBUG               
            printf("handled touch up\n");
#endif                
                
            break;
        }
        case LA_EVENT_TOUCH_MOVED:
        {
            return handleTouchMoved((laInput_TouchMovedEvent*)evt);
                return LA_FAILURE;

#ifdef INPUT_EVENT_DEBUG                
            printf("handled touch move\n");
#endif                
                
            break;
        }
        default:
            break;
    }

    return LA_SUCCESS;
}