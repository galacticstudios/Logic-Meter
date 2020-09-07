/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_TxEPStatus_Default.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : TxEPStatus
    and its Variant : Default
    For following APIs :
        PLIB_USBHS_TxEPStatusGet
        PLIB_USBHS_TxEPStatusClear
        PLIB_USBHS_TxEPOUTTokenSend
        PLIB_USBHS_ExistsTxEPStatus

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

#ifndef _USBHS_TXEPSTATUS_DEFAULT_H
#define _USBHS_TXEPSTATUS_DEFAULT_H

#include "usbhs_registers.h"

//******************************************************************************
/* Function :  USBHS_TxEPStatusGet_Default

  Summary:
    Implements Default variant of PLIB_USBHS_TxEPStatusGet 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_TxEPStatusGet function.
*/

PLIB_TEMPLATE uint8_t USBHS_TxEPStatusGet_Default
( 
    USBHS_MODULE_ID index , 
    uint8_t endpoint 
)
{
    /* Returns the entire endpoint status register */
    
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    return(usbhs->EPCSR[endpoint].TXCSRL_DEVICEbits.w);
}

//******************************************************************************
/* Function :  USBHS_TxEPStatusClear_Default

  Summary:
    Implements Default variant of PLIB_USBHS_TxEPStatusClear 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_TxEPStatusClear function.
*/

PLIB_TEMPLATE void USBHS_TxEPStatusClear_Default
( 
    USBHS_MODULE_ID index , 
    uint8_t endpoint , 
    USBHS_TXEP_ERROR error 
)
{
    /* Clears the specified set of errors */
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    usbhs->EPCSR[endpoint].TXCSRL_DEVICEbits.w &= (~(error));
}

//******************************************************************************
/* Function :  USBHS_ExistsTxEPStatus_Default

  Summary:
    Implements Default variant of PLIB_USBHS_ExistsTxEPStatus

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_ExistsTxEPStatus function.
*/

#define PLIB_USBHS_ExistsTxEPStatus PLIB_USBHS_ExistsTxEPStatus
PLIB_TEMPLATE bool USBHS_ExistsTxEPStatus_Default( USBHS_MODULE_ID index )
{
    return true;
}


#endif /*_USBHS_TXEPSTATUS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

