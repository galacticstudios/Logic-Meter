/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_HighSpeedSupport_Unsupported.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : HighSpeedSupport
    and its Variant : Unsupported
    For following APIs :
        PLIB_USBHS_HighSpeedEnable
        PLIB_USBHS_HighSpeedDisable
        PLIB_USBHS_HighSpeedIsConnected
        PLIB_USBHS_FullOrHighSpeedIsConnected
        PLIB_USBHS_ExistsHighSpeedSupport
        PLIB_USBHS_DMAErrorGet
        PLIB_USBHS_DMAInterruptGet
        PLIB_USBHS_DMAOperationEnable
        PLIB_USBHS_LoadEPInIndex
        PLIB_USBHS_GetEP0FIFOAddress
        PLIB_USBHS_GetEP0CSRAddress
        PLIB_USBHS_GetReceiveDataCount
        PLIB_USBHS_TestModeEnter
        PLIB_USBHS_TestModeExit

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

#ifndef _USBHS_HIGHSPEEDSUPPORT_UNSUPPORTED_H
#define _USBHS_HIGHSPEEDSUPPORT_UNSUPPORTED_H

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
/* Function :  USBHS_HighSpeedEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_HighSpeedEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_HighSpeedEnable function.
*/

PLIB_TEMPLATE void USBHS_HighSpeedEnable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_HighSpeedEnable");
}


//******************************************************************************
/* Function :  USBHS_HighSpeedDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_HighSpeedDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_HighSpeedDisable function.
*/

PLIB_TEMPLATE void USBHS_HighSpeedDisable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_HighSpeedDisable");
}


//******************************************************************************
/* Function :  USBHS_HighSpeedIsConnected_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_HighSpeedIsConnected 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_HighSpeedIsConnected function.
*/

PLIB_TEMPLATE bool USBHS_HighSpeedIsConnected_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_HighSpeedIsConnected");

    return false;
}


//******************************************************************************
/* Function :  USBHS_FullOrHighSpeedIsConnected_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_FullOrHighSpeedIsConnected 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_FullOrHighSpeedIsConnected function.
*/

PLIB_TEMPLATE bool USBHS_FullOrHighSpeedIsConnected_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_FullOrHighSpeedIsConnected");

    return false;
}


//******************************************************************************
/* Function :  USBHS_ExistsHighSpeedSupport_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_ExistsHighSpeedSupport

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_ExistsHighSpeedSupport function.
*/

PLIB_TEMPLATE bool USBHS_ExistsHighSpeedSupport_Unsupported( USBHS_MODULE_ID index )
{
    return false;
}


//******************************************************************************
/* Function :  USBHS_DMAErrorGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DMAErrorGet 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DMAErrorGet function.
*/

PLIB_TEMPLATE bool USBHS_DMAErrorGet_Unsupported( USBHS_MODULE_ID index , uint8_t dmaChannel )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DMAErrorGet");

    return false;
}


//******************************************************************************
/* Function :  USBHS_DMAInterruptGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DMAInterruptGet 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DMAInterruptGet function.
*/

PLIB_TEMPLATE uint8_t USBHS_DMAInterruptGet_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DMAInterruptGet");

    return 0;
}


//******************************************************************************
/* Function :  USBHS_DMAOperationEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_DMAOperationEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_DMAOperationEnable function.
*/

PLIB_TEMPLATE void USBHS_DMAOperationEnable_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint , uint8_t dmaChannel , void * address , uint32_t count , bool direction )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_DMAOperationEnable");
}


//******************************************************************************
/* Function :  USBHS_LoadEPInIndex_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_LoadEPInIndex 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_LoadEPInIndex function.
*/

PLIB_TEMPLATE void USBHS_LoadEPInIndex_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_LoadEPInIndex");
}


//******************************************************************************
/* Function :  USBHS_GetEP0FIFOAddress_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_GetEP0FIFOAddress 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_GetEP0FIFOAddress function.
*/

PLIB_TEMPLATE uint8_t * USBHS_GetEP0FIFOAddress_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_GetEP0FIFOAddress");

    return 0;
}


//******************************************************************************
/* Function :  USBHS_GetEP0CSRAddress_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_GetEP0CSRAddress 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_GetEP0CSRAddress function.
*/

PLIB_TEMPLATE uint8_t * USBHS_GetEP0CSRAddress_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_GetEP0CSRAddress");

    return 0;
}


//******************************************************************************
/* Function :  USBHS_GetReceiveDataCount_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_GetReceiveDataCount 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_GetReceiveDataCount function.
*/

PLIB_TEMPLATE uint32_t USBHS_GetReceiveDataCount_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_GetReceiveDataCount");

    return 0;
}


//******************************************************************************
/* Function :  USBHS_TestModeEnter_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_TestModeEnter 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_TestModeEnter function.
*/

PLIB_TEMPLATE bool USBHS_TestModeEnter_Unsupported( USBHS_MODULE_ID index , uint8_t testMode )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_TestModeEnter");

    return false;
}


//******************************************************************************
/* Function :  USBHS_TestModeExit_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_TestModeExit 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_TestModeExit function.
*/

PLIB_TEMPLATE bool USBHS_TestModeExit_Unsupported( USBHS_MODULE_ID index , uint8_t testMode )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_TestModeExit");

    return false;
}


#endif /*_USBHS_HIGHSPEEDSUPPORT_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

