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


#include "gfx/hal/inc/gfx_driver_interface.h"

GFX_DriverIntf GFX_DriverInterfaces[GFX_DRIVER_COUNT] = {{0}};

GFX_Result GFX_InitializeDriverList();

GFX_Result GFX_DriverIntfInitialize(void)
{   
    return GFX_InitializeDriverList();
}

GFX_Result GFX_DriverIntfGet(GFX_Driver idx, GFX_DriverIntf* intf)
{
    if(idx >= GFX_DRIVER_COUNT || intf == NULL)
        return GFX_FAILURE;
        
    intf->infoGet = GFX_DriverInterfaces[idx].infoGet;
    intf->contextInitialize = GFX_DriverInterfaces[idx].contextInitialize;
        
    return GFX_SUCCESS;
}

GFX_DriverIntf* GFX_DriverIntfPtrGet(GFX_Driver idx)
{
    if(idx >= GFX_DRIVER_COUNT)
        return NULL;
        
    return &GFX_DriverInterfaces[idx];
}