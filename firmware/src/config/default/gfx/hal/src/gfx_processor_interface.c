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


#include "gfx/hal/inc/gfx_processor_interface.h"

#if GFX_PROCESSOR_COUNT > 0
GFX_ProcessorIntf GFX_ProcessorInterfaces[GFX_PROCESSOR_COUNT] = {{0}};

GFX_Result GFX_InitializeProcessorList();

GFX_Result GFX_ProcessorIntfInitialize(void)
{   
    return GFX_InitializeProcessorList();
}

GFX_Result GFX_ProcessorIntfGet(GFX_Processor idx, GFX_ProcessorIntf* intf)
{
    if(idx >= GFX_PROCESSOR_COUNT || intf == NULL)
        return GFX_FAILURE;
        
    intf->infoGet = GFX_ProcessorInterfaces[idx].infoGet;
    intf->contextInitialize = GFX_ProcessorInterfaces[idx].contextInitialize;
        
    return GFX_SUCCESS;
}

GFX_ProcessorIntf* GFX_ProcessorIntfPtrGet(GFX_Driver idx)
{
    if(idx >= GFX_DRIVER_COUNT)
        return NULL;
        
    return &GFX_ProcessorInterfaces[idx];
}
#endif