/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_EndpointOperations_Unsupported.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : EndpointOperations
    and its Variant : Unsupported
    For following APIs :
        PLIB_USBHS_EndpointRxRequestEnable
        PLIB_USBHS_EndpointRxRequestClear
        PLIB_USBHS_HostRxEndpointConfigure
        PLIB_USBHS_HostTxEndpointConfigure
        PLIB_USBHS_HostTxEndpointDataToggleClear
        PLIB_USBHS_HostRxEndpointDataToggleClear
        PLIB_USBHS_DeviceRxEndpointConfigure
        PLIB_USBHS_DeviceTxEndpointConfigure
        PLIB_USBHS_DeviceRxEndpointStallEnable
        PLIB_USBHS_DeviceTxEndpointStallEnable
        PLIB_USBHS_DeviceRxEndpointStallDisable
        PLIB_USBHS_DeviceTxEndpointStallDisable
        PLIB_USBHS_DeviceTxEndpointPacketReady
        PLIB_USBHS_ExistsEndpointOperations

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

#ifndef _USBHS_ENDPOINTOPERATIONS_UNSUPPORTED_H
#define _USBHS_ENDPOINTOPERATIONS_UNSUPPORTED_H

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
/* Function :  USBHS_EndpointRxRequestEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_EndpointRxRequestEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_EndpointRxRequestEnable function.
*/

PLIB_TEMPLATE void USBHS_EndpointRxRequestEnable_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_EndpointRxRequestEnable");
}


//******************************************************************************
/* Function :  USBHS_EndpointRxRequestClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_EndpointRxRequestClear 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_EndpointRxRequestClear function.
*/

PLIB_TEMPLATE void USBHS_EndpointRxRequestClear_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_EndpointRxRequestClear");
}


//******************************************************************************
/* Function :  USBHS_HostRxEndpointConfigure_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_HostRxEndpointConfigure 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_HostRxEndpointConfigure function.
*/

PLIB_TEMPLATE void USBHS_HostRxEndpointConfigure_Unsupported( USBHS_MODULE_ID index , uint8_t hostEndpoint , uint32_t speed , uint32_t pipeType , uint16_t endpointSize , uint16_t receiveFIFOAddress , uint16_t fifoSize , uint8_t  targetEndpoint , uint8_t  targetDevice , uint8_t  targetHub , uint8_t  targetHubPort , uint8_t  nakInterval )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_HostRxEndpointConfigure");
}


//******************************************************************************
/* Function :  USBHS_HostTxEndpointConfigure_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_HostTxEndpointConfigure 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_HostTxEndpointConfigure function.
*/

PLIB_TEMPLATE void USBHS_HostTxEndpointConfigure_Unsupported( USBHS_MODULE_ID index , uint8_t hostEndpoint , uint32_t speed , uint32_t pipeType , uint16_t endpointSize , uint16_t receiveFIFOAddress , uint16_t fifoSize , uint8_t  targetEndpoint , uint8_t  targetDevice , uint8_t  targetHub , uint8_t  targetHubPort , uint8_t  nakInterval )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_HostTxEndpointConfigure");
}


//******************************************************************************
/* Function :  USBHS_HostTxEndpointDataToggleClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_HostTxEndpointDataToggleClear 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_HostTxEndpointDataToggleClear function.
*/

PLIB_TEMPLATE void USBHS_HostTxEndpointDataToggleClear_Unsupported( USBHS_MODULE_ID index , uint8_t hostEndpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_HostTxEndpointDataToggleClear");
}


//******************************************************************************
/* Function :  USBHS_HostRxEndpointDataToggleClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_HostRxEndpointDataToggleClear 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_HostRxEndpointDataToggleClear function.
*/

PLIB_TEMPLATE void USBHS_HostRxEndpointDataToggleClear_Unsupported( USBHS_MODULE_ID index , uint8_t hostEndpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_HostRxEndpointDataToggleClear");
}


//******************************************************************************
/* Function :  USBHS_DeviceRxEndpointConfigure_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DeviceRxEndpointConfigure 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DeviceRxEndpointConfigure function.
*/

PLIB_TEMPLATE void USBHS_DeviceRxEndpointConfigure_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint , uint16_t endpointSize , uint16_t fifoAddress , uint8_t fifoSize , uint32_t transferType )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DeviceRxEndpointConfigure");
}


//******************************************************************************
/* Function :  USBHS_DeviceTxEndpointConfigure_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DeviceTxEndpointConfigure 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DeviceTxEndpointConfigure function.
*/

PLIB_TEMPLATE void USBHS_DeviceTxEndpointConfigure_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint , uint16_t endpointSize , uint16_t fifoAddress , uint8_t fifoSize , uint32_t transferType )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DeviceTxEndpointConfigure");
}


//******************************************************************************
/* Function :  USBHS_DeviceRxEndpointStallEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DeviceRxEndpointStallEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DeviceRxEndpointStallEnable function.
*/

PLIB_TEMPLATE void USBHS_DeviceRxEndpointStallEnable_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DeviceRxEndpointStallEnable");
}


//******************************************************************************
/* Function :  USBHS_DeviceTxEndpointStallEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DeviceTxEndpointStallEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DeviceTxEndpointStallEnable function.
*/

PLIB_TEMPLATE void USBHS_DeviceTxEndpointStallEnable_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DeviceTxEndpointStallEnable");
}


//******************************************************************************
/* Function :  USBHS_DeviceRxEndpointStallDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DeviceRxEndpointStallDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DeviceRxEndpointStallDisable function.
*/

PLIB_TEMPLATE void USBHS_DeviceRxEndpointStallDisable_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DeviceRxEndpointStallDisable");
}


//******************************************************************************
/* Function :  USBHS_DeviceTxEndpointStallDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DeviceTxEndpointStallDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DeviceTxEndpointStallDisable function.
*/

PLIB_TEMPLATE void USBHS_DeviceTxEndpointStallDisable_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DeviceTxEndpointStallDisable");
}


//******************************************************************************
/* Function :  USBHS_DeviceTxEndpointPacketReady_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DeviceTxEndpointPacketReady 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DeviceTxEndpointPacketReady function.
*/

PLIB_TEMPLATE void USBHS_DeviceTxEndpointPacketReady_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DeviceTxEndpointPacketReady");
}


//******************************************************************************
/* Function :  USBHS_ExistsEndpointOperations_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_ExistsEndpointOperations

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_ExistsEndpointOperations function.
*/

PLIB_TEMPLATE bool USBHS_ExistsEndpointOperations_Unsupported( USBHS_MODULE_ID index )
{
    return false;
}


#endif /*_USBHS_ENDPOINTOPERATIONS_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

