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

/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    libaria.c

  Summary:
    Graphics Object Layer of the Microchip Graphics Library.

  Description:
    This module implements the common routines for the Graphics Object Layer
    of the Microchip Graphics Library. The routines are independent of the
    Display Driver Layer and should be compatible with any Display Driver
    that is compliant with the requirements of the Display Driver
    Layer of the Graphics Library.
    The module utilizes the Graphics Primitive Layer to render the objects.
*******************************************************************************/

#include "gfx/libaria/inc/libaria_context.h"

#include "gfx/hal/gfx.h"
#include "gfx/utils/gfx_utils.h"

#include "gfx/libaria/inc/libaria_draw.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_list.h"
#include "gfx/libaria/inc/libaria_screen.h"
#include "gfx/libaria/inc/libaria_utils.h"

laContext* _activeContext = NULL;

typedef struct laContext_ScreenChangeEvent_t
{
    laEvent event;
    
    uint32_t index;
} laContext_ScreenChangeEvent;

laContext* laContext_Create(GFX_Driver driver,
                            GFX_Display display,
                            GFX_Processor processor,
                            GFX_ColorMode mode,
                            GFXU_MemoryIntf* memoryIntf)
{
    laContext* context;

    if(memoryIntf == NULL)
    {
        context = calloc(1, sizeof(laContext));

        context->memIntf.heap.malloc = (GFX_Malloc_FnPtr) &malloc;
        context->memIntf.heap.calloc = (GFX_Calloc_FnPtr) &calloc;
        context->memIntf.heap.realloc = (GFX_Realloc_FnPtr) &realloc;
        context->memIntf.heap.free = (GFX_Free_FnPtr) &free;
        context->memIntf.heap.memset = (GFX_Memset_FnPtr) &memset;
        context->memIntf.heap.memcpy = (GFX_Memcpy_FnPtr) &memcpy;
    }
    else
    {
        context = memoryIntf->heap.calloc(1, sizeof(laContext));
        
        if(context == NULL)
            return NULL;

        context->memIntf = *memoryIntf;
    }

    context->gfxContext = GFX_Open(driver,
                                   processor,
                                   display,
                                   &memoryIntf->heap);

    if(context->gfxContext == NULL)
        return NULL;

    context->displayIndex = display;

    laArray_Create(&context->screenList);
    
    context->colorMode = mode;
    
    context->preprocessStateChangedCB = NULL;

    if(_activeContext == NULL)
        _activeContext = context;
        
    laScheme_Initialize(&_activeContext->defaultScheme, mode);
    laInput_SetEnabled(LA_TRUE);

    return context;
}

laResult laContext_Destroy(laContext* context)
{
    uint32_t i;
    
    if(context == NULL)
        return LA_FAILURE;

    for(i = 0; i < context->screenList.size; i++)
        laScreen_Delete(context->screenList.values[i]);
    
    laArray_Destroy(&context->screenList);
    
    context->activeScreen = NULL;
    
    laList_Destroy(&context->event.events);
    
    GFX_Close();
    
    if(_activeContext == context)
        _activeContext = NULL;
    
    context->memIntf.heap.free(context);

    return LA_SUCCESS;
}

void laContext_Update(uint32_t dt)
{
    if(_activeContext == NULL)
        return;

    laEvent_ProcessEvents();
    
    _laContext_Update(dt);
    _laContext_Paint();
}

laContext* laContext_GetActive()
{
    return _activeContext;
}

laResult laContext_SetActive(laContext* context)
{
    _activeContext = context;

    return LA_SUCCESS;
}

GFX_ColorMode laContext_GetColorMode()
{
    if(_activeContext == NULL)
        return GFX_COLOR_MODE_GS_8;

    return _activeContext->colorMode;
}

laResult laContext_AddScreen(laScreen* screen)
{
    if(_activeContext == NULL || screen == NULL)
        return LA_FAILURE;
        
    screen->id = _activeContext->screenList.size;

    laArray_PushBack(&_activeContext->screenList, screen);

    return LA_SUCCESS;
}

laResult laContext_RemoveScreen(laScreen* screen)
{
    if(_activeContext == NULL || screen == NULL)
        return LA_FAILURE;

    if(laArray_Remove(&_activeContext->screenList, screen) == LA_FAILURE)
        return LA_FAILURE;
        
    return LA_SUCCESS;
}

GFX_Rect laContext_GetScreenRect()
{
    GFX_Rect rect = {0};

    if(_activeContext == NULL)
        return rect;

    rect = GFX_DisplayInfoPtrGet(_activeContext->displayIndex)->rect;

    return rect;
}

laScreen* laContext_GetActiveScreen()
{
    if(_activeContext == NULL)
        return NULL;
        
    return _activeContext->activeScreen;
}

int32_t laContext_GetActiveScreenIndex()
{
    if(_activeContext == NULL || _activeContext->activeScreen == NULL)
        return -1;
    
    return laArray_Find(&_activeContext->screenList, _activeContext->activeScreen);
}

laResult laContext_SetActiveScreen(uint32_t idx)
{
    laContext_ScreenChangeEvent* evt;
    
    if(_activeContext == NULL || idx < 0 || idx >= _activeContext->screenList.size)
        return LA_FAILURE;

    if(laContext_GetActiveScreenIndex() == idx)
        return LA_SUCCESS;
    
    // if no active screen then change immediately    
    if(_activeContext->activeScreen == NULL)
    {
        _laContext_ShowScreen(idx);
    }
    // else execute later to avoid race conditions
    else
    {
        evt = _activeContext->memIntf.heap.malloc(sizeof(laContext_ScreenChangeEvent));
        
        evt->event.id = LA_EVENT_SCREEN_CHANGE;
        evt->index = idx;
        
        laEvent_AddEvent((laEvent*)evt);
    }

    return LA_SUCCESS;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
GFX_DEPRECATED laResult laContext_HideActiveScreen()
{
    if(_activeContext == NULL)
        return LA_FAILURE;

    if(_activeContext->activeScreen != NULL)
        return laScreen_Hide(_activeContext->activeScreen);

    return LA_FAILURE;
}
#pragma GCC diagnostic pop

laPreemptionLevel laContext_GetPreemptionLevel()
{
    if(_activeContext == NULL)
        return 0;
        
    return _activeContext->preemptLevel;
}

laResult laContext_SetPreemptionLevel(laPreemptionLevel level)
{
    if(_activeContext == NULL)
        return LA_FAILURE;

    _activeContext->preemptLevel = level;

    return LA_SUCCESS;
}

// rework this to be non-recursive if necessary
laWidgetUpdateState _laContext_UpdateWidget(laWidget* widget, uint32_t dt)
{
    laWidget* child;
    laWidgetUpdateState state = LA_WIDGET_UPDATE_STATE_DONE;
    laWidgetUpdateState childState = LA_WIDGET_UPDATE_STATE_DONE;
    laWidgetUpdateState retState = LA_WIDGET_UPDATE_STATE_DONE;
    uint32_t i;

    state = widget->update(widget, dt);
    if (state == LA_WIDGET_UPDATE_STATE_PENDING)
    {
        retState = LA_WIDGET_UPDATE_STATE_PENDING;
    }

    // iterate over all existing children for update
    for(i = 0; i < widget->children.size; i++)
    {
        child = widget->children.values[i];

        childState = _laContext_UpdateWidget(child, dt);
        if (childState == LA_WIDGET_UPDATE_STATE_PENDING)
        {
            retState = LA_WIDGET_UPDATE_STATE_PENDING;
        }
    }

    return retState;
}

uint32_t _laContext_Update(uint32_t dt)
{
    laScreen* activeScreen;
    laLayer* layer;
    uint32_t modDelta;
    int32_t i;
    laWidgetUpdateState widgetState;
    laContextUpdateState state = LA_CONTEXT_UPDATE_DONE;;

    activeScreen = laContext_GetActive()->activeScreen;
    
    if(activeScreen == NULL)
        return LA_CONTEXT_UPDATE_DONE;

    // iterate over all existing layers for update
    for(i = 0; i < LA_MAX_LAYERS; i++)
    {
        layer = activeScreen->layers[i];

        if(layer == NULL)
            continue;
            
        if(layer->frameState <= LA_LAYER_FRAME_PREFRAME)
        {
            modDelta = dt;
            
            if(layer->deltaTime != 0)
            {
                modDelta += layer->deltaTime;
                layer->deltaTime = 0;
            }
            
            widgetState = _laContext_UpdateWidget((laWidget*)layer, modDelta);
            if (widgetState != LA_WIDGET_UPDATE_STATE_DONE)
                state = LA_CONTEXT_UPDATE_PENDING;
        }
        else
        {
            layer->deltaTime += dt;
        }
    }

    return state;
}

GFXU_StringTableAsset* laContext_GetStringTable()
{
    if(_activeContext == NULL)
        return NULL;
        
    return _activeContext->stringTable;
}

void laContext_SetStringTable(GFXU_StringTableAsset* table)
{
    if(_activeContext == NULL)
        return;
        
    _activeContext->stringTable = table;    
    
    laContext_RedrawAll();
}

uint32_t laContext_GetStringLanguage()
{
    if(_activeContext == NULL)
        return 0;
        
    return _activeContext->languageID;
}

static void signalLanguageChanging(laWidget* wgt)
{
    uint32_t i;
    laWidget* child;
    
    // signal event
    wgt->languageChangeEvent(wgt);
    
    for(i = 0; i < wgt->children.size; i++)
    {
        child = wgt->children.values[i];

        signalLanguageChanging(child);
    }
}

void laContext_SetStringLanguage(uint32_t id)
{   
    laScreen* activeScreen;
    GFX_Rect screenRect;
    laLayer* layer;
    uint32_t i;
    
    if(_activeContext == NULL || _activeContext->languageID == id)
        return;
        
    activeScreen = laContext_GetActive()->activeScreen;
    
    screenRect = laContext_GetScreenRect();
    
    if(activeScreen == NULL)
        return;
        
    // iterate over all qualifying layers and signal event
    for(i = 0; i < LA_MAX_LAYERS; i++)
    {
        layer = activeScreen->layers[i];

        if(layer == NULL ||
           layer->widget.enabled == LA_FALSE ||
           GFX_RectIntersects(&screenRect, &layer->widget.rect) == LA_FALSE)
            continue;
            
        signalLanguageChanging((laWidget*)layer);
    }
    
    _activeContext->languageID = id;

	// do it again after the change
    for(i = 0; i < LA_MAX_LAYERS; i++)
    {
        layer = activeScreen->layers[i];

        if(layer == NULL ||
           layer->widget.enabled == LA_FALSE ||
           GFX_RectIntersects(&screenRect, &layer->widget.rect) == LA_FALSE)
            continue;
            
        signalLanguageChanging((laWidget*)layer);
    }
}

void laContext_RedrawAll()
{
    laScreen* activeScreen;
    laLayer* layer;
    GFX_Rect screenRect;
    uint32_t i;
    
    if(_activeContext == NULL)
        return;
        
    activeScreen = laContext_GetActive()->activeScreen;
    
    if(activeScreen == NULL)
        return;
        
    screenRect = laContext_GetScreenRect();
        
    // iterate over all qualifying layers and invalidate
    for(i = 0; i < LA_MAX_LAYERS; i++)
    {
        layer = activeScreen->layers[i];

        if(layer == NULL ||
           layer->widget.enabled == LA_FALSE ||
           GFX_RectIntersects(&screenRect, &layer->widget.rect) == LA_FALSE)
            continue;

        laWidget_Invalidate((laWidget*)layer);
    }
}

laScheme* laContext_GetDefaultScheme()
{
    if(_activeContext == NULL)
        return NULL;
        
    return &_activeContext->defaultScheme;
}

laWidget* laContext_GetFocusWidget()
{
    if(_activeContext == NULL)
        return NULL;

    return _activeContext->focus;
}

laResult laContext_SetFocusWidget(laWidget* widget)
{
    if(_activeContext == NULL)
        return LA_FAILURE;

    if(_activeContext->focus == widget)
        return LA_SUCCESS;

    if(_activeContext->focus != NULL)
        _activeContext->focus->focusLost(_activeContext->focus);

    _activeContext->focus = widget;

    if(_activeContext->focus != NULL)
        _activeContext->focus->focusGained(_activeContext->focus);

    return LA_SUCCESS;
}

laEditWidget* laContext_GetEditWidget()
{
    if(_activeContext == NULL)
        return NULL;

    return _activeContext->edit;
}

laResult laContext_SetEditWidget(laWidget* widget)
{
    laEditWidget* edit;
    
    if(_activeContext == NULL)
        return LA_FAILURE;

    if(_activeContext->edit == (laEditWidget*)widget)
        return LA_SUCCESS;

    if(_activeContext->edit != NULL)
        laEditWidget_EndEdit(_activeContext->edit);

    if(widget != NULL && widget->editable == LA_FALSE)
        return LA_FAILURE;
        
    edit = (laEditWidget*)widget;

    _activeContext->edit = (laEditWidget*)widget;

    if(laEditWidget_StartEdit(edit) == LA_FAILURE)
    {
        _activeContext->edit = NULL;
        
        return LA_FAILURE;
    }

    return LA_SUCCESS;
}

laEventResult _laContext_HandleScreenChangeEvent(laEvent* evt)
{
    laContext_ScreenChangeEvent* scrEvt = (laContext_ScreenChangeEvent*)evt;

    _laContext_ShowScreen(scrEvt->index);
    
    // don't process any more events for the active screen
    laList_Destroy(&_activeContext->event.events);
    
    // failure here means the event handler won't try to delete the event
    // the event has already been deleted
    return LA_EVENT_RESET_QUEUE; 
}

void _laContext_ShowScreen(uint32_t idx)
{
    laScreen *oldScr, *newScr;
    uint32_t i, j;
    laLayer* lyr;
    laBool reallocate = LA_FALSE;
    
    if(_activeContext == NULL || idx >= _activeContext->screenList.capacity)
        return;
        
    oldScr = _activeContext->activeScreen;
    newScr = (laScreen*)_activeContext->screenList.values[idx];
        
    // screen is already active
    if(oldScr == newScr)
        return;
    
    // set screen configuration
    laContext_GetActive()->frameState = LA_CONTEXT_FRAME_READY;
    
    GFX_Set(GFXF_ORIENTATION, newScr->orientation);
    GFX_Set(GFXF_MIRRORED, newScr->mirrored);
    GFX_Set(GFXF_LAYER_SWAP_SYNC, newScr->layerSwapSync);
    
    // clear any input state
    laContext_SetFocusWidget(NULL);
    laContext_SetEditWidget(NULL);
    
    // loading a screen for the first time    
    if(oldScr == NULL)
    {
        if(newScr->created == LA_FALSE)
        {
            newScr->createCB(newScr);
            newScr->created = LA_TRUE;
        }
        
        for(i = 0; i < LA_MAX_LAYERS; i++)
        {
            if(newScr->layers[i] == NULL)
                continue;
                
            GFX_Set(GFXF_LAYER_ACTIVE, i);
            
            _laLayer_Reset(newScr->layers[i]);
            
            // turn on layers
            if(newScr->layers[i] != NULL)
            {
                lyr = newScr->layers[i];
                
                GFX_Set(GFXF_LAYER_ACTIVE, i);
                GFX_Set(GFXF_LAYER_ENABLED, lyr->widget.enabled);
                GFX_Set(GFXF_LAYER_POSITION, lyr->widget.rect.x, lyr->widget.rect.y); 
                GFX_Set(GFXF_LAYER_SIZE, lyr->widget.rect.width, lyr->widget.rect.height); 
                GFX_Set(GFXF_LAYER_BUFFER_COUNT, lyr->bufferCount);
                
                //Set colormode on ALL layers in HAL
                GFX_Set(GFXF_COLOR_MODE, laContext_GetActive()->colorMode);
                
                // manage buffer settings
                for(j = 0; j < lyr->bufferCount; j++)
                {
                    if(lyr->buffers[j].type == LA_BUFFER_TYPE_AUTO)
                    {
                        GFX_Set(GFXF_LAYER_BUFFER_COHERENT, j, GFX_TRUE);
                        GFX_Set(GFXF_LAYER_BUFFER_ALLOCATE, j);
                    }
                    else
                        GFX_Set(GFXF_LAYER_BUFFER_ADDRESS, lyr->buffers[j].address);
                }
                
                GFX_Set(GFXF_LAYER_VSYNC, lyr->vsync);
                GFX_Set(GFXF_LAYER_ALPHA_ENABLE, lyr->alphaEnable, GFX_FALSE);
                GFX_Set(GFXF_LAYER_ALPHA_AMOUNT, lyr->widget.alphaAmount, GFX_FALSE);
                GFX_Set(GFXF_LAYER_MASK_ENABLE, lyr->maskEnable, GFX_FALSE);
                GFX_Set(GFXF_LAYER_MASK_COLOR, lyr->maskColor, GFX_FALSE);   
                                
                // force a redraw of the new layer
                laWidget_Invalidate((laWidget*)lyr);
            }
        }
    }
    // switching screens
    else
    {
        // call hide callback if set
        if(oldScr->hideCB != NULL)
            oldScr->hideCB(oldScr);
            
        // free all of the layer child widgets if the screen is not persistent
        if(oldScr->persistent == LA_FALSE)
        {
            for(i = 0; i < LA_MAX_LAYERS; i++)
            {
                // going to delete this layer eventually but need knowledge of its
                // attributes when setting up new screen.  
                // just remove all children instead
                if(oldScr->layers[i] != NULL)
                    laWidget_DeleteAllDescendants((laWidget*)oldScr->layers[i]);
            }
        }
            
        // create the new screen
        if(newScr->created == LA_FALSE)
        {
            newScr->createCB(newScr);
            newScr->created = LA_TRUE;
        }
            
        for(i = 0; i < LA_MAX_LAYERS; i++)
        {
            // both layers are null, nothing to do
            if(oldScr->layers[i] == NULL && newScr->layers[i] == NULL)
            {
                continue;
            }
            // turning off a layer 
            else if(oldScr->layers[i] != NULL && newScr->layers[i] == NULL)
            {
                // configure HAL layer
                GFX_Set(GFXF_LAYER_ACTIVE, i);
                GFX_Set(GFXF_LAYER_ENABLED, LA_FALSE);
                GFX_Set(GFXF_LAYER_BUFFER_COUNT, 0);
            }
            else
            {
                lyr = newScr->layers[i];
                
                GFX_Set(GFXF_LAYER_ACTIVE, i);
                GFX_Set(GFXF_LAYER_ENABLED, lyr->widget.enabled);
                
                reallocate = LA_FALSE;
                
                //Test with layer from old screen
                if (oldScr->layers[i])
                {
                    // only move if necessary
                    if(oldScr->layers[i]->widget.rect.x != lyr->widget.rect.x ||
                       oldScr->layers[i]->widget.rect.y != lyr->widget.rect.y)
                        GFX_Set(GFXF_LAYER_POSITION, lyr->widget.rect.x, lyr->widget.rect.y); 

                    // only resize if necessary
                    if(oldScr->layers[i]->widget.rect.width != lyr->widget.rect.width ||
                       oldScr->layers[i]->widget.rect.height != lyr->widget.rect.height)
                    {
                        GFX_Set(GFXF_LAYER_SIZE, lyr->widget.rect.width, lyr->widget.rect.height);

                        reallocate = LA_TRUE;
                    }

                    // only reallocate if necessary
                    if(oldScr->layers[i]->bufferCount != lyr->bufferCount)
                    {
                        GFX_Set(GFXF_LAYER_BUFFER_COUNT, lyr->bufferCount);

                        reallocate = LA_TRUE;
                    }
                }
                else
                {
                    GFX_Set(GFXF_LAYER_BUFFER_COUNT, lyr->bufferCount);
                    reallocate = LA_TRUE;
                }
                
                // manage buffer settings
                for(j = 0; j < lyr->bufferCount; j++)
                {
                    if(reallocate == LA_FALSE && 
                       oldScr->layers[i] && 
                       oldScr->layers[i]->buffers[j].type == LA_BUFFER_TYPE_AUTO && 
                       oldScr->layers[i]->buffers[j].type == lyr->buffers[j].type)
                    {
                        continue;
                    }
                    
                    if(lyr->buffers[j].type == LA_BUFFER_TYPE_AUTO)
                    {
                        GFX_Set(GFXF_LAYER_BUFFER_COHERENT, j, GFX_TRUE);
                        GFX_Set(GFXF_LAYER_BUFFER_ALLOCATE, j);
                    }
                    else
                        GFX_Set(GFXF_LAYER_BUFFER_ADDRESS, lyr->buffers[i].address);
                }
                
                GFX_Set(GFXF_LAYER_VSYNC, lyr->vsync);
                GFX_Set(GFXF_LAYER_ALPHA_ENABLE, lyr->alphaEnable, GFX_TRUE);
                GFX_Set(GFXF_LAYER_ALPHA_AMOUNT, lyr->widget.alphaAmount, GFX_TRUE);
                GFX_Set(GFXF_LAYER_MASK_ENABLE, lyr->maskEnable, GFX_TRUE);
                GFX_Set(GFXF_LAYER_MASK_COLOR, lyr->maskColor, GFX_TRUE);
                
                //Set colormode on ALL layers in HAL
                GFX_Set(GFXF_COLOR_MODE, laContext_GetActive()->colorMode); 
                                
                // force a redraw of the new layer
                laWidget_Invalidate((laWidget*)newScr->layers[i]);
            }
            
            // delete layer if screen is not marked as persistent
            if(oldScr->persistent == LA_FALSE)
            {
                laLayer_Delete(oldScr->layers[i]);
                
                laContext_GetActive()->memIntf.heap.free(oldScr->layers[i]);
                
                oldScr->layers[i] = NULL;
                
                oldScr->created = LA_FALSE;
            }
        }
    }
    
    // call show callback if set
    if(newScr->showCB != NULL)
        newScr->showCB(newScr);
    
    // finally set the new screen pointer
    laContext_GetActive()->activeScreen = newScr;
}

laBool laContext_IsDrawing()
{
    uint32_t idx;
    
    if(_activeContext == NULL || _activeContext->activeScreen == NULL)
        return LA_FALSE;
        
    for(idx = 0; idx < LA_MAX_LAYERS; idx++)
    {
        if(_activeContext->activeScreen->layers[idx] == NULL)
            continue;
            
        if(laLayer_IsDrawing(_activeContext->activeScreen->layers[idx]))
            return LA_TRUE;
    }
    
    return LA_FALSE;
}

laBool laContext_IsLayerDrawing(uint32_t idx)
{
    if(_activeContext == NULL ||
       _activeContext->activeScreen == NULL ||
       idx >= LA_MAX_LAYERS)
        return LA_FALSE;
        
    return laLayer_IsDrawing(_activeContext->activeScreen->layers[idx]);
}

laResult laContext_SetPreprocessAssetStateChangedCallback(laContext_PreprocessAssetsStateChangedCallback_FnPtr cb)
{
    if(_activeContext == NULL || cb == NULL)
        return LA_FAILURE;
    
    _activeContext->preprocessStateChangedCB = cb;
    
    return LA_SUCCESS;
}
