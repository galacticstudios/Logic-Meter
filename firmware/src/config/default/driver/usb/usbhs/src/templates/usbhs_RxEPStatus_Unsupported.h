/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_RxEPStatus_Unsupported.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : RxEPStatus
    and its Variant : Unsupported
    For following APIs :
        PLIB_USBHS_RxEPStatusGet
        PLIB_USBHS_RxEPStatusClear
        PLIB_USBHS_ExistsRxEPStatus

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

#ifndef _USBHS_RXEPSTATUS_UNSUPPORTED_H
#define _USBHS_RXEPSTATUS_UNSUPPORTED_H

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
/* Function :  USBHS_RxEPStatusGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_RxEPStatusGet 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_RxEPStatusGet function.
*/

PLIB_TEMPLATE uint8_t USBHS_RxEPStatusGet_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_RxEPStatusGet");

    return 0;
}


//******************************************************************************
/* Function :  USBHS_RxEPStatusClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_RxEPStatusClear 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_RxEPStatusClear function.
*/

PLIB_TEMPLATE void USBHS_RxEPStatusClear_Unsupported( USBHS_MODULE_ID index , uint8_t endpoint , USBHS_RXEP_ERROR error )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_RxEPStatusClear");
}


//******************************************************************************
/* Function :  USBHS_ExistsRxEPStatus_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_ExistsRxEPStatus

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_ExistsRxEPStatus function.
*/

PLIB_TEMPLATE bool USBHS_ExistsRxEPStatus_Unsupported( USBHS_MODULE_ID index )
{
    return false;
}


#endif /*_USBHS_RXEPSTATUS_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

