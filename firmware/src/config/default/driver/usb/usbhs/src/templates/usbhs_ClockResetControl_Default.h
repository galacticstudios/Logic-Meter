/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_ClockResetControl_Default.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : ClockResetControl
    and its Variant : Default
    For following APIs :
        PLIB_USBHS_ExistsClockResetControl
        PLIB_USBHS_GlobalInterruptEnable
        PLIB_USBHS_GlobalInterruptDisable

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

#ifndef _USBHS_CLOCKRESETCONTROL_DEFAULT_H
#define _USBHS_CLOCKRESETCONTROL_DEFAULT_H

//******************************************************************************
/* Function :  USBHS_ExistsClockResetControl_Default

  Summary:
    Implements Default variant of PLIB_USBHS_ExistsClockResetControl

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_ExistsClockResetControl function.
*/

#define PLIB_USBHS_ExistsClockResetControl PLIB_USBHS_ExistsClockResetControl
PLIB_TEMPLATE bool USBHS_ExistsClockResetControl_Default( USBHS_MODULE_ID index )
{
    return true;
}

//******************************************************************************
/* Function :  USBHS_GlobalInterruptEnable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_GlobalInterruptEnable 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_GlobalInterruptEnable function.
*/

PLIB_TEMPLATE void USBHS_GlobalInterruptEnable_Default( USBHS_MODULE_ID index )
{
    /* Get pointer to the USBCRCON register and enable the global interrupt
     * register */
    volatile __USBCRCONbits_t * usbcrcon = (__USBCRCONbits_t *)(index - 0x5F000);
    usbcrcon->USBIE = 1;
}

//******************************************************************************
/* Function :  USBHS_GlobalInterruptDisable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_GlobalInterruptDisable 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_GlobalInterruptDisable function.
*/

PLIB_TEMPLATE void USBHS_GlobalInterruptDisable_Default( USBHS_MODULE_ID index )
{
    /* Get pointer to the USBCRCON register and disable the global interrupt
     * register */
    volatile __USBCRCONbits_t * usbcrcon = (__USBCRCONbits_t *)(index - 0x5F000);
    usbcrcon->USBIE = 0;
}

#endif /*_USBHS_CLOCKRESETCONTROL_DEFAULT_H*/

/******************************************************************************
 End of File
*/

