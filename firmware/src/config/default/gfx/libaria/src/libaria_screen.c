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


#include "gfx/libaria/inc/libaria_screen.h"

#include "gfx/hal/gfx.h"

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"

laScreen* laScreen_New(laBool persistent, laBool createAtStartup, laScreen_CreateCallback_FnPtr cb)
{
    laScreen* scr = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    scr = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laScreen));
    
    laString_Initialize(&scr->name);

    scr->persistent = persistent;
    scr->createCB = cb;
    scr->created = LA_FALSE;
    
    scr->layerSwapSync = LA_TRUE;
    
    if(scr->persistent == LA_TRUE && createAtStartup == LA_TRUE)
    {
        scr->createCB(scr);
    
        scr->created = LA_TRUE;
    }
    
    return scr;
}

void laScreen_Delete(laScreen* scr)
{
    uint32_t i;
    
    if(laContext_GetActive() == NULL ||
       scr == NULL || 
       scr->created == LA_FALSE)
        return;
        
    for(i = 0; i < LA_MAX_LAYERS; i++)
    {
        if(scr->layers[i] == NULL)
            continue;
            
        // if screen is active, turn off all layers
        if(laContext_GetActive()->activeScreen == scr)
        {
            GFX_Set(GFXF_LAYER_ACTIVE, i);
            GFX_Set(GFXF_LAYER_ENABLED, LA_FALSE);
            GFX_Set(GFXF_LAYER_BUFFER_FREE, i);
        }
        
        laLayer_Delete(scr->layers[i]);
            
        laContext_GetActive()->memIntf.heap.free(scr->layers[i]);
            
        scr->layers[i] = NULL;
    }
    
    scr->created = LA_FALSE;
}

static void setLayerAttributes(laLayer* layer, uint32_t idx)
{    
    uint32_t i;
    
    GFX_Set(GFXF_COLOR_MODE, laContext_GetActive()->colorMode);
    
    GFX_Set(GFXF_LAYER_ACTIVE, idx);
    GFX_Set(GFXF_LAYER_ENABLED, layer->widget.enabled);
    GFX_Set(GFXF_LAYER_POSITION, layer->widget.rect.x, layer->widget.rect.y); 
    GFX_Set(GFXF_LAYER_SIZE, layer->widget.rect.width, layer->widget.rect.height); 
    GFX_Set(GFXF_LAYER_BUFFER_COUNT, layer->bufferCount);
    
    // manage buffer settings
    for(i = 0; i < layer->bufferCount; i++)
    {
        if(layer->buffers[i].type == LA_BUFFER_TYPE_AUTO)
        {
            GFX_Set(GFXF_LAYER_BUFFER_COHERENT, i, GFX_TRUE);
            GFX_Set(GFXF_LAYER_BUFFER_ALLOCATE, i);
        }
        else
            GFX_Set(GFXF_LAYER_BUFFER_ADDRESS, layer->buffers[i].address);
    }
    
    GFX_Set(GFXF_LAYER_VSYNC, layer->vsync);
    GFX_Set(GFXF_LAYER_ALPHA_ENABLE, layer->alphaEnable, GFX_TRUE);
    GFX_Set(GFXF_LAYER_ALPHA_AMOUNT, layer->widget.alphaAmount, GFX_TRUE);
    GFX_Set(GFXF_LAYER_MASK_ENABLE, layer->maskEnable, GFX_TRUE);
    GFX_Set(GFXF_LAYER_MASK_COLOR, layer->maskColor, GFX_TRUE);
}

GFX_DEPRECATED laResult laScreen_Show(laScreen* scr) 
{
    int i;

    if(laContext_GetActive() == NULL || scr == NULL)
        return LA_FAILURE;
        
    // screen is already active
    if(laContext_GetActive()->activeScreen == scr)
        return LA_FAILURE;
       
    if(scr->created == LA_FALSE)
    {
        scr->createCB(scr);
        scr->created = LA_TRUE;
    }
    
    GFX_Set(GFXF_COLOR_MODE, laContext_GetActive()->colorMode);
    GFX_Set(GFXF_ORIENTATION, scr->orientation);
    GFX_Set(GFXF_MIRRORED, scr->mirrored);
    GFX_Set(GFXF_LAYER_SWAP_SYNC, scr->layerSwapSync);
    
    laContext_GetActive()->frameState = LA_CONTEXT_FRAME_READY;
    
    for(i = 0; i < LA_MAX_LAYERS; i++)
    {
        if(scr->layers[i] == NULL)
            continue;
            
        GFX_Set(GFXF_LAYER_ACTIVE, i);
        
        _laLayer_Reset(scr->layers[i]);
        
        // turn on existing layers
        if(scr->layers[i] != NULL)
        {
            setLayerAttributes(scr->layers[i], i);
            
            laWidget_Invalidate((laWidget*)scr->layers[i]);
        }
    }

    laContext_GetActive()->activeScreen = scr;

    // call show callback if set
    if(scr->showCB != NULL)
        scr->showCB(scr);

    return LA_SUCCESS;
}

GFX_DEPRECATED laResult laScreen_Hide(laScreen* scr) 
{
    int32_t i;

    if(scr == NULL ||
       laContext_GetActive() == NULL ||
       laContext_GetActive()->activeScreen != scr)
        return LA_FAILURE;

    // call hide callback if set
    if(scr->hideCB != NULL)
        scr->hideCB(scr);
        
    GFX_Set(GFXF_LAYER_SWAP_SYNC, LA_FALSE);

    // TODO change this to use laScreen_Destroy
    for(i = 0; i < LA_MAX_LAYERS; i++)
    {
        if(scr->layers[i] == NULL)
            continue;
            
        // turn off all layers
        GFX_Set(GFXF_LAYER_ACTIVE, i);
        GFX_Set(GFXF_LAYER_ENABLED, LA_FALSE);
        GFX_Set(GFXF_LAYER_BUFFER_COUNT, 0);
        
        if(scr->persistent == LA_FALSE)
        {
            laLayer_Delete(scr->layers[i]);
            
            laContext_GetActive()->memIntf.heap.free(scr->layers[i]);
            
            scr->layers[i] = NULL;
            
            scr->created = LA_FALSE;
        }
    }

    laContext_GetActive()->activeScreen = NULL;

    return LA_SUCCESS;
}

laResult laScreen_SetLayer(laScreen* scr, uint32_t idx, laLayer* layer)
{
    if(idx > LA_MAX_LAYERS - 1)
        return LA_FAILURE;

    if(scr->layers[idx] != NULL)
        scr->layers[idx]->screen = NULL;

    scr->layers[idx] = layer;
    layer->screen = scr;
    
    // don't worry about updating HAL if this screen is not active
    if(laContext_GetActive()->activeScreen == scr)
        setLayerAttributes(layer, idx);

    return LA_SUCCESS;
}

int32_t laScreen_GetLayerIndex(laScreen* scr, laLayer* layer)
{
    int i = 0;
    
    if(layer == NULL)
        return -1;

    for(i = 0; i < LA_MAX_LAYERS; i++)
    {
        if(scr->layers[i] == layer)
            return i;
    }

    return -1;
}

laScreenOrientation laScreen_GetOrientation(laScreen* scr)
{
    if(scr == NULL)
        return 0;
        
    return scr->orientation;
}

laResult laScreen_SetOrientation(laScreen* scr, laScreenOrientation ori)
{
    if(scr == NULL)
        return LA_FAILURE;
        
    scr->orientation = ori;
    
    return LA_SUCCESS;
}

laBool laScreen_GetMirrored(laScreen* scr)
{
    if(scr == NULL)
        return LA_FALSE;
        
    return scr->mirrored;
}

laResult laScreen_SetMirrored(laScreen* scr, laBool mirr)
{
    if(scr == NULL)
        return LA_FAILURE;
        
    scr->mirrored = mirr;
    
    // todo if active screen, update HAL settings and repaint
    
    return LA_SUCCESS;
}

laBool laScreen_GetLayerSwapSync(laScreen* scr)
{
    if(scr == NULL)
        return LA_FALSE;
        
    return scr->layerSwapSync;
}

laResult laScreen_SetLayerSwapSync(laScreen* scr, laBool sync)
{
    if(scr == NULL)
        return LA_FAILURE;
        
    scr->layerSwapSync = sync;
    
    // todo if active screen, update HAL settings and repaint
    
    return LA_SUCCESS;
}

laScreen_ShowHideCallback_FnPtr laScreen_GetShowEventCallback(laScreen* scr)
{
    if(scr == NULL)
        return NULL;
        
    return scr->showCB;
}

laResult laScreen_SetShowEventCallback(laScreen* scr,
                                       laScreen_ShowHideCallback_FnPtr cb)
{
    if(scr == NULL)
        return LA_FAILURE;
        
    scr->showCB = cb;
    
    return LA_SUCCESS;
}

laScreen_ShowHideCallback_FnPtr laScreen_GetHideEventCallback(laScreen* scr)
{
    if(scr == NULL)
        return NULL;
        
    return scr->hideCB;
}

laResult laScreen_SetHideEventCallback(laScreen* scr,
                                       laScreen_ShowHideCallback_FnPtr cb)
{
    if(scr == NULL)
        return LA_FAILURE;
        
    scr->hideCB = cb;
    
    return LA_SUCCESS;
}