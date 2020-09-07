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
  MPLAB Harmony Graphics Composer Generated Implementation File

  File Name:
    libaria_harmony.c

  Summary:
    Build-time generated implementation from the MPLAB Harmony
    Graphics Composer.

  Description:
    Build-time generated implementation from the MPLAB Harmony
    Graphics Composer.

    Created with MPLAB Harmony Version 3.0
*******************************************************************************/

#include "gfx/libaria/libaria_harmony.h"
#include "gfx/libaria/libaria_init.h"

#include "gfx/libaria/libaria.h"



/*** libaria Object Global ***/
libaria_objects libariaObj;
static LIBARIA_STATES libariaState;

GFXU_MemoryIntf memIntf;

static GFX_Result LibAria_MediaOpenRequest(GFXU_AssetHeader* asset);

static GFX_Result LibAria_MediaReadRequest(GFXU_ExternalAssetReader* reader,
                                           GFXU_AssetHeader* asset,
                                           void* address,
                                           uint32_t readSize,
                                           uint8_t* destBuffer,
                                           GFXU_MediaReadRequestCallback_FnPtr cb);

static void LibAria_MediaCloseRequest(GFXU_AssetHeader* asset);




int32_t LibAria_Initialize(void)
{
    if(laInitialize() == LA_FAILURE)
        return -1;

    memIntf.heap.malloc = (GFX_Malloc_FnPtr) &malloc;
    //memIntf.heap.coherent_alloc = (GFX_Malloc_FnPtr) &__pic32_alloc_coherent; // FIXME for H3
    memIntf.heap.coherent_alloc = (GFX_Malloc_FnPtr) &malloc;
    memIntf.heap.calloc = (GFX_Calloc_FnPtr) &calloc;
    memIntf.heap.free = (GFX_Free_FnPtr) &free;
    memIntf.heap.coherent_free = (GFX_Free_FnPtr) &free;
    //memIntf.heap.coherent_free = (GFX_Free_FnPtr) &__pic32_free_coherent; // FIXME for H3

    memIntf.heap.memcpy = (GFX_Memcpy_FnPtr) &memcpy;
    memIntf.heap.memset = (GFX_Memset_FnPtr) &memset;
    memIntf.heap.realloc = (GFX_Realloc_FnPtr) &realloc;
    memIntf.open = (GFXU_MediaOpenRequest_FnPtr) &LibAria_MediaOpenRequest;
    memIntf.read = (GFXU_MediaReadRequest_FnPtr) &LibAria_MediaReadRequest;
    memIntf.close = (GFXU_MediaCloseRequest_FnPtr) &LibAria_MediaCloseRequest;

    libariaObj.context = laContext_Create(0, 0, 0, GFX_COLOR_MODE_RGB_565, &memIntf);

    if(libariaObj.context == NULL)
        return -1;

    laContext_SetActive(libariaObj.context);


    libaria_initialize(); // use auto-generated initialization functions


    libariaState = LIBARIA_STATE_INIT;

    return 0;
}

void LibAria_Tasks(void)
{
    switch(libariaState)
    {
        case LIBARIA_STATE_INIT:
        {
            LIBARIA_STATES nextState = LIBARIA_STATE_RUNNING;

            libariaState = nextState;
            break;
        }
        case LIBARIA_STATE_RUNNING:
        {
            laContext_SetActive(libariaObj.context);


            laUpdate(0);

            break;
        }

        default:
        {
            break;
        }
    }
}



static GFX_Result LibAria_MediaOpenRequest(GFXU_AssetHeader* asset)
{
    return GFX_FAILURE;
}

static GFX_Result LibAria_MediaReadRequest(GFXU_ExternalAssetReader* reader,
                                           GFXU_AssetHeader* asset,
                                           void* address,
                                           uint32_t readSize,
                                           uint8_t* destBuffer,
                                           GFXU_MediaReadRequestCallback_FnPtr cb)
{
    return GFX_FAILURE;
}

static void LibAria_MediaCloseRequest(GFXU_AssetHeader* asset)
{
}

