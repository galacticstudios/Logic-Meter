/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_SoftReset_Default.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : SoftReset
    and its Variant : Default
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

#ifndef _USBHS_SOFTRESET_DEFAULT_H
#define _USBHS_SOFTRESET_DEFAULT_H

#include "usbhs_registers.h"

//******************************************************************************
/* Function :  USBHS_SoftResetEnable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_SoftResetEnable 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_SoftResetEnable function.
*/

PLIB_TEMPLATE void USBHS_SoftResetEnable_Default( USBHS_MODULE_ID index )
{
    /* This function enables the soft reset bits */

    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    usbhs->SOFTRSTbits.w = USBHS_SOFT_RST_NRST_SET|USBHS_SOFT_RST_NRSTX_SET;
}

//******************************************************************************
/* Function :  USBHS_SoftResetDisable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_SoftResetDisable 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_SoftResetDisable function.
*/

PLIB_TEMPLATE void USBHS_SoftResetDisable_Default( USBHS_MODULE_ID index )
{   
    /* This function will clear the NRST and NRSTX bits. */
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    usbhs->SOFTRSTbits.w = 0;
}

//******************************************************************************
/* Function :  USBHS_SoftResetNRSTXEnable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_SoftResetNRSTXEnable 

  Description:
    This template implements the Default variant of the
    PLIB_USBHS_SoftResetNRSTXEnable function.
*/

PLIB_TEMPLATE void USBHS_SoftResetNRSTXEnable_Default( USBHS_MODULE_ID index )
{   
    /* This function sets the NRSTX bits. It is cleared automatically by
     * hardware. */
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    usbhs->SOFTRSTbits.NRSTX = 1;
}


//******************************************************************************
/* Function :  USBHS_ExistsSoftReset_Default

  Summary:
    Implements Default variant of PLIB_USBHS_ExistsSoftReset

  Description:
    This template implements the Default variant of the PLIB_USBHS_ExistsSoftReset function.
*/

#define PLIB_USBHS_ExistsSoftReset PLIB_USBHS_ExistsSoftReset
PLIB_TEMPLATE bool USBHS_ExistsSoftReset_Default( USBHS_MODULE_ID index )
{
    return true;
}

//******************************************************************************
/* Function :  USBHS_SoftResetIsComplete

  Summary:
    Implements Default variant of PLIB_USBHS_SoftResetIsComplete

  Description:
    This template implements the Default variant of the PLIB_USBHS_SoftResetIsComplete function.
*/

PLIB_TEMPLATE bool USBHS_SoftResetIsComplete_Default( USBHS_MODULE_ID index )
{
	bool returnVal = false ;
	volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
	if ( usbhs->SOFTRSTbits.w == 0 )
	{
		returnVal = true;
	}
	return ( returnVal );
}

#endif /*_USBHS_SOFTRESET_DEFAULT_H*/

/******************************************************************************
 End of File
*/

