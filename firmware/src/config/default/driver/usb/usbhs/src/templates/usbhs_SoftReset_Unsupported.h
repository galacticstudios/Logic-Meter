/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_SoftReset_Unsupported.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : SoftReset
    and its Variant : Unsupported
    For following APIs :
        PLIB_USBHS_SoftResetEnable
        PLIB_USBHS_SoftResetDisable
        PLIB_USBHS_ExistsSoftReset

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

#ifndef _USBHS_SOFTRESET_UNSUPPORTED_H
#define _USBHS_SOFTRESET_UNSUPPORTED_H

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
/* Function :  USBHS_SoftResetEnable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_SoftResetEnable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_SoftResetEnable function.
*/

PLIB_TEMPLATE int USBHS_SoftResetEnable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_SoftResetEnable");

    return 0;
}


//******************************************************************************
/* Function :  USBHS_SoftResetDisable_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_SoftResetDisable 

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_SoftResetDisable function.
*/

PLIB_TEMPLATE int USBHS_SoftResetDisable_Unsupported( USBHS_MODULE_ID index )
{
    PLIB_ASSERT(false, "The device selected does not implement PLIB_USBHS_SoftResetDisable");

    return 0;
}


//******************************************************************************
/* Function :  USBHS_ExistsSoftReset_Unsupported

  Summary:
    Implements Unsupported variant of PLIB_USBHS_ExistsSoftReset

  Description:
    This template implements the Unsupported variant of the PLIB_USBHS_ExistsSoftReset function.
*/

PLIB_TEMPLATE bool USBHS_ExistsSoftReset_Unsupported( USBHS_MODULE_ID index )
{
    return false;
}


#endif /*_USBHS_SOFTRESET_UNSUPPORTED_H*/

/******************************************************************************
 End of File
*/

