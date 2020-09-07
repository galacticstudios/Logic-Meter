/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_EP0Status_Unsupported.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : EP0Status
    and its Variant : Unsupported
    For following APIs :
        PLIB_USBHS_EP0StatusGet
        PLIB_USBHS_EP0StatusClear
        PLIB_USBHS_EP0INHandshakeSend
        PLIB_USBHS_EP0INTokenSend
        PLIB_USBHS_EP0OUTHandshakeSend
        PLIB_USBHS_EP0INHandshakeClear
        PLIB_USBHS_ExistsEP0Status

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

#ifndef _USBHS_EP0STATUS_UNSUPPORTED_H
#define _USBHS_EP0STATUS_UNSUPPORTED_H

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
/* Function :  USBHS_EP0StatusGet_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_EP0StatusGet 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_EP0StatusGet function.
*/

PLIB_TEMPLATE uint8_t USBHS_EP0StatusGet_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_EP0StatusGet");

    return 0;
}


//******************************************************************************
/* Function :  USBHS_EP0StatusClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_EP0StatusClear 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_EP0StatusClear function.
*/

PLIB_TEMPLATE void USBHS_EP0StatusClear_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_EP0StatusClear");
}


//******************************************************************************
/* Function :  USBHS_EP0INHandshakeSend_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_EP0INHandshakeSend 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_EP0INHandshakeSend function.
*/

PLIB_TEMPLATE void USBHS_EP0INHandshakeSend_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_EP0INHandshakeSend");
}


//******************************************************************************
/* Function :  USBHS_EP0INTokenSend_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_EP0INTokenSend 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_EP0INTokenSend function.
*/

PLIB_TEMPLATE void USBHS_EP0INTokenSend_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_EP0INTokenSend");
}


//******************************************************************************
/* Function :  USBHS_EP0OUTHandshakeSend_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_EP0OUTHandshakeSend 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_EP0OUTHandshakeSend function.
*/

PLIB_TEMPLATE void USBHS_EP0OUTHandshakeSend_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_EP0OUTHandshakeSend");
}


//******************************************************************************
/* Function :  USBHS_EP0INHandshakeClear_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_EP0INHandshakeClear 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_EP0INHandshakeClear function.
*/

PLIB_TEMPLATE void USBHS_EP0INHandshakeClear_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_EP0INHandshakeClear");
}


//******************************************************************************
/* Function :  USBHS_ExistsEP0Status_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_ExistsEP0Status

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_ExistsEP0Status function.
*/

PLIB_TEMPLATE bool USBHS_ExistsEP0Status_Unsupported( USBHS_MODULE_ID index )
{
    return false;
}


#endif /*_USBHS_EP0STATUS_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

