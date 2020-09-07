/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_ModuleControl_Unsupported.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ModuleControl
    and its Variant : Unsupported
    For following APIs :
        PLIB_USBHS_ResumeEnable
        PLIB_USBHS_ResumeDisable
        PLIB_USBHS_SuspendEnable
        PLIB_USBHS_SuspendDisable
        PLIB_USBHS_ResetEnable
        PLIB_USBHS_ResetDisable
        PLIB_USBHS_VBUSLevelGet
        PLIB_USBHS_HostModeIsEnabled
        PLIB_USBHS_IsBDevice
        PLIB_USBHS_SessionEnable
        PLIB_USBHS_SessionDisable
        PLIB_USBHS_DeviceAddressSet
        PLIB_USBHS_DeviceAttach
        PLIB_USBHS_DeviceDetach
        PLIB_USBHS_ExistsModuleControl

*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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

//DOM-IGNORE-END

#ifndef _USBHS_MODULECONTROL_UNSUPPORTED_H
#define _USBHS_MODULECONTROL_UNSUPPORTED_H

//******************************************************************************
/* Routines available for accessing VREGS, MASKS, POS, LEN are 

  VREGs: 
    None.

  MASKs: 
    None.

  POSs: 
    None.

  LENs: 
    None.

*/


//******************************************************************************
/* Function :  USBHS_ResumeEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_ResumeEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_ResumeEnable function.
*/

PLIB_TEMPLATE void USBHS_ResumeEnable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_ResumeEnable");
}


//******************************************************************************
/* Function :  USBHS_ResumeDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_ResumeDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_ResumeDisable function.
*/

PLIB_TEMPLATE void USBHS_ResumeDisable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_ResumeDisable");
}


//******************************************************************************
/* Function :  USBHS_SuspendEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_SuspendEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_SuspendEnable function.
*/

PLIB_TEMPLATE void USBHS_SuspendEnable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_SuspendEnable");
}


//******************************************************************************
/* Function :  USBHS_SuspendDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_SuspendDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_SuspendDisable function.
*/

PLIB_TEMPLATE void USBHS_SuspendDisable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_SuspendDisable");
}


//******************************************************************************
/* Function :  USBHS_ResetEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_ResetEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_ResetEnable function.
*/

PLIB_TEMPLATE void USBHS_ResetEnable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_ResetEnable");
}


//******************************************************************************
/* Function :  USBHS_ResetDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_ResetDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_ResetDisable function.
*/

PLIB_TEMPLATE void USBHS_ResetDisable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_ResetDisable");
}


//******************************************************************************
/* Function :  USBHS_VBUSLevelGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_VBUSLevelGet 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_VBUSLevelGet function.
*/

PLIB_TEMPLATE USBHS_VBUS_LEVEL USBHS_VBUSLevelGet_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_VBUSLevelGet");

    return (USBHS_VBUS_LEVEL) 0;
}


//******************************************************************************
/* Function :  USBHS_HostModeIsEnabled_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_HostModeIsEnabled 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_HostModeIsEnabled function.
*/

PLIB_TEMPLATE bool USBHS_HostModeIsEnabled_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_HostModeIsEnabled");

    return false;
}


//******************************************************************************
/* Function :  USBHS_IsBDevice_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_IsBDevice 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_IsBDevice function.
*/

PLIB_TEMPLATE bool USBHS_IsBDevice_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_IsBDevice");

    return false;
}


//******************************************************************************
/* Function :  USBHS_SessionEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_SessionEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_SessionEnable function.
*/

PLIB_TEMPLATE void USBHS_SessionEnable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_SessionEnable");
}


//******************************************************************************
/* Function :  USBHS_SessionDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_SessionDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_SessionDisable function.
*/

PLIB_TEMPLATE void USBHS_SessionDisable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_SessionDisable");
}


//******************************************************************************
/* Function :  USBHS_DeviceAddressSet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DeviceAddressSet 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DeviceAddressSet function.
*/

PLIB_TEMPLATE void USBHS_DeviceAddressSet_Unsupported( USBHS_MODULE_ID index , uint8_t address )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DeviceAddressSet");
}


//******************************************************************************
/* Function :  USBHS_DeviceAttach_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DeviceAttach 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DeviceAttach function.
*/

PLIB_TEMPLATE void USBHS_DeviceAttach_Unsupported( USBHS_MODULE_ID index , uint32_t speed )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DeviceAttach");
}


//******************************************************************************
/* Function :  USBHS_DeviceDetach_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DeviceDetach 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DeviceDetach function.
*/

PLIB_TEMPLATE void USBHS_DeviceDetach_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DeviceDetach");
}


//******************************************************************************
/* Function :  USBHS_ExistsModuleControl_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_ExistsModuleControl

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_ExistsModuleControl function.
*/

PLIB_TEMPLATE bool USBHS_ExistsModuleControl_Unsupported( USBHS_MODULE_ID index )
{
    return false;
}


#endif /*_USBHS_MODULECONTROL_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

