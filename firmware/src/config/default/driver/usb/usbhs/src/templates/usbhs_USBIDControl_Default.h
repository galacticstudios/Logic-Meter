/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_USBIDControl_Default.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : USBIDControl
    and its Variant : Default
    For following APIs :
        PLIB_USBHS_ExistsUSBIDControl
        PLIB_USBHS_USBIDOverrideEnable
        PLIB_USBHS_USBIDOverrideDisable
        PLIB_USBHS_USBIDOverrideValueSet
        PLIB_USBHS_PhyIDMonitoringEnable
        PLIB_USBHS_PhyIDMonitoringDisable

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

#ifndef _USBHS_USBIDCONTROL_DEFAULT_H
#define _USBHS_USBIDCONTROL_DEFAULT_H

#include "usbhs_registers.h"

//******************************************************************************
/* Function :  USBHS_ExistsUSBIDControl_Default

  Summary:
    Implements Default variant of PLIB_USBHS_ExistsUSBIDControl

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_ExistsUSBIDControl function.
*/

#define PLIB_USBHS_ExistsUSBIDControl PLIB_USBHS_ExistsUSBIDControl
PLIB_TEMPLATE bool USBHS_ExistsUSBIDControl_Default( USBHS_MODULE_ID index )
{
    return true;
}

//******************************************************************************
/* Function :  USBHS_USBIDOverrideEnable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_USBIDOverrideEnable 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_USBIDOverrideEnable function.
*/

PLIB_TEMPLATE void USBHS_USBIDOverrideEnable_Default( USBHS_MODULE_ID index )
{
    /* Get a pointer to the USBCRCON register and then set the USBIDOVEN bits.
     * The USBCRCON register does not have SET, CLR registers. */

    volatile __USBCRCONbits_t * usbcrcon = (__USBCRCONbits_t *)(index - 0x5F000);
    usbcrcon->USBIDOVEN = 1;
}

//******************************************************************************
/* Function :  USBHS_USBIDOverrideDisable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_USBIDOverrideDisable 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_USBIDOverrideDisable function.
*/

PLIB_TEMPLATE void USBHS_USBIDOverrideDisable_Default( USBHS_MODULE_ID index )
{
	volatile __USBCRCONbits_t * usbcrcon = (__USBCRCONbits_t *)(index - 0x5F000);
    usbcrcon->USBIDOVEN = 0;
}

//******************************************************************************
/* Function :  USBHS_USBIDOverrideValueSet_Default

  Summary:
    Implements Default variant of PLIB_USBHS_USBIDOverrideValueSet 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_USBIDOverrideValueSet function.
*/

PLIB_TEMPLATE void USBHS_USBIDOverrideValueSet_Default
( 
    USBHS_MODULE_ID index , 
    USBHS_USBID_OVERRIDE_VALUE idValue
)
{
    /* Sets the ID override value. */
    volatile __USBCRCONbits_t * usbcrcon = (__USBCRCONbits_t *)(index - 0x5F000);
    usbcrcon->USBIDVAL = idValue;
}

//******************************************************************************
/* Function :  USBHS_PhyIDMonitoringEnable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_PhyIDMonitoringEnable 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_PhyIDMonitoringEnable function.
*/

PLIB_TEMPLATE void USBHS_PhyIDMonitoringEnable_Default( USBHS_MODULE_ID index )
{
    /* Enables PHY ID monitoring. */
    volatile __USBCRCONbits_t * usbcrcon = (__USBCRCONbits_t *)(index - 0x5F000);
    usbcrcon->PHYIDEN = 1;
}

//******************************************************************************
/* Function :  USBHS_PhyIDMonitoringDisable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_PhyIDMonitoringDisable 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_PhyIDMonitoringDisable function.
*/

PLIB_TEMPLATE void USBHS_PhyIDMonitoringDisable_Default( USBHS_MODULE_ID index )
{
    /* Enables PHY ID monitoring. */
    volatile __USBCRCONbits_t * usbcrcon = (__USBCRCONbits_t *)(index - 0x5F000);
    usbcrcon->PHYIDEN = 0;
}


#endif /*_USBHS_USBIDCONTROL_DEFAULT_H*/

/******************************************************************************
 End of File
*/

