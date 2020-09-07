/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_RxEPStatus_Default.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : RxEPStatus
    and its Variant : Default
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

#ifndef _USBHS_RXEPSTATUS_DEFAULT_H
#define _USBHS_RXEPSTATUS_DEFAULT_H

#include "usbhs_registers.h"

//******************************************************************************
/* Function :  USBHS_RxEPStatusGet_Default

  Summary:
    Implements Default variant of PLIB_USBHS_RxEPStatusGet 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_RxEPStatusGet function.
*/

PLIB_TEMPLATE uint8_t USBHS_RxEPStatusGet_Default
( 
    USBHS_MODULE_ID index, 
    uint8_t endpoint 
)
{
    /* Return the RX endpoint status */
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    return(usbhs->EPCSR[endpoint].RXCSRL_DEVICEbits.w);
}

//******************************************************************************
/* Function :  USBHS_RxEPStatusClear_Default

  Summary:
    Implements Default variant of PLIB_USBHS_RxEPStatusClear 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_RxEPStatusClear function.
*/

PLIB_TEMPLATE void USBHS_RxEPStatusClear_Default
( 
    USBHS_MODULE_ID index, 
    uint8_t endpoint, 
    USBHS_RXEP_ERROR error 
)
{
    /* Clear the error in the status register */
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    usbhs->EPCSR[endpoint].RXCSRL_DEVICEbits.w &= (~(error)); 
}

//******************************************************************************
/* Function :  USBHS_RxEPINTokenSend_Default

  Summary:
    Implements Default variant of PLIB_USBHS_RxEPINTokenSend 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_RxEPINTokenSend function.
*/

PLIB_TEMPLATE void USBHS_RxEPINTokenSend_Default( USBHS_MODULE_ID index, uint8_t endpoint )
{
    /* Causes the module to send an IN Token in host mode */
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    usbhs->EPCSR[endpoint].RXCSRL_HOSTbits.REQPKT = 1; 
}

//******************************************************************************
/* Function :  USBHS_ExistsRxEPStatus_Default

  Summary:
    Implements Default variant of PLIB_USBHS_ExistsRxEPStatus

  Description:
    This template implements the Default variant of the PLIB_USBHS_ExistsRxEPStatus function.
*/

#define PLIB_USBHS_ExistsRxEPStatus PLIB_USBHS_ExistsRxEPStatus
PLIB_TEMPLATE bool USBHS_ExistsRxEPStatus_Default( USBHS_MODULE_ID index )
{
    return true;
}


#endif /*_USBHS_RXEPSTATUS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

