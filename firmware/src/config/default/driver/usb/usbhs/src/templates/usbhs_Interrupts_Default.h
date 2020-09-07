/*******************************************************************************
  USBHS Peripheral Library Template Implementation

  File Name:
    usbhs_Interrupts_Default.h

  Summary:
    USBHS PLIB Template Implementation

  Description:
    This header file contains template implementations
    For Feature : Interrupts
    and its Variant : Default
    For following APIs :
        PLIB_USBHS_InterruptEnableSet
        PLIB_USBHS_TxInterruptFlagsGet
        PLIB_USBHS_RxInterruptFlagsGet
        PLIB_USBHS_GenInterruptFlagsGet
        PLIB_USBHS_ExistsInterrupts

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

#ifndef _USBHS_INTERRUPTS_DEFAULT_H
#define _USBHS_INTERRUPTS_DEFAULT_H

#include "usbhs_registers.h"

//******************************************************************************
/* Function :  USBHS_InterruptEnableSet_Default

  Summary:
    Implements Default variant of PLIB_USBHS_InterruptEnableSet 

  Description:
    This template implements the Default variant of the PLIB_USBHS_InterruptEnableSet function.
*/

PLIB_TEMPLATE void USBHS_InterruptEnableSet_Default
( 
    USBHS_MODULE_ID index , 
    USBHS_GEN_INTERRUPT generalInterrupts , 
    USBHS_EPTXRX_INTERRUPT transmitInterrupts , 
    USBHS_EPTXRX_INTERRUPT receiveInterrupts 
)
{
    /* This function will update the general interrupt enable
     * endpoint interrupts enable registers */
    
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);

    /* Update the transmit interrupts */
    usbhs->INTRTXEbits.w = transmitInterrupts;
    usbhs->INTRRXEbits.w = receiveInterrupts;
    usbhs->INTRUSBEbits.w = generalInterrupts;
}

//******************************************************************************
/* Function :  USBHS_TxInterruptFlagsGet_Default

  Summary:
    Implements Default variant of PLIB_USBHS_TxInterruptFlagsGet 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_TxInterruptFlagsGet function.
*/

PLIB_TEMPLATE USBHS_EPTXRX_INTERRUPT USBHS_TxInterruptFlagsGet_Default
( 
    USBHS_MODULE_ID index
)
{
    /* Returns the status of the Transmit endpoint interrupts. The entire
     * register must read at once because this is a clear on read register. */
    
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    return((USBHS_EPTXRX_INTERRUPT)(usbhs->INTRTX));
}

//******************************************************************************
/* Function :  USBHS_RxInterruptFlagsGet_Default

  Summary:
    Implements Default variant of PLIB_USBHS_RxInterruptFlagsGet 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_RxInterruptFlagsGet function.
*/

PLIB_TEMPLATE USBHS_EPTXRX_INTERRUPT USBHS_RxInterruptFlagsGet_Default
( 
    USBHS_MODULE_ID index 
)
{
    /* Returns the status of the Receive endpoint interrupts. The entire
     * register must read at once because this is a clear on read register. */
    
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    return((USBHS_EPTXRX_INTERRUPT)(usbhs->INTRRX));
}

//******************************************************************************
/* Function :  USBHS_GenInterruptFlagsGet_Default

  Summary:
    Implements Default variant of PLIB_USBHS_GenInterruptFlagsGet 

  Description:
    This template implements the Default variant of the PLIB_USBHS_GenInterruptFlagsGet function.
*/

PLIB_TEMPLATE USBHS_GEN_INTERRUPT USBHS_GenInterruptFlagsGet_Default
( 
    USBHS_MODULE_ID index 
)
{
    /* Returns the status of the General endpoint interrupts. The entire
     * register must read at once because this is a clear on read register. */
    
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);
    return((USBHS_GEN_INTERRUPT)(usbhs->INTRUSB));
}

//******************************************************************************
/* Function :  USBHS_TxInterruptEnable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_TxInterruptEnable 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_TxInterruptEnable function.
*/

PLIB_TEMPLATE void USBHS_TxInterruptEnable_Default
(  
    USBHS_MODULE_ID index , 
    USBHS_EPTXRX_INTERRUPT interrupt
)
{
   volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);

    /* Update the Transmit endpoint interrupts */
    usbhs->INTRTXEbits.w = interrupt;
}


//******************************************************************************
/* Function :  USBHS_RxInterruptEnable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_RxInterruptEnable 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_RxInterruptEnable function.
*/

PLIB_TEMPLATE void USBHS_RxInterruptEnable_Default
( 
    USBHS_MODULE_ID index , 
    USBHS_EPTXRX_INTERRUPT interrupt
)
{
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);

    /* Update the Receive endpoint interrupts */
    usbhs->INTRRXEbits.w = interrupt;
}

//******************************************************************************
/* Function :  USBHS_TxInterruptDisable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_TxInterruptDisable 

  Description:
    This template implements the Default variant of the 
    PLIB_USBHS_TxInterruptDisable function.
*/

PLIB_TEMPLATE void USBHS_TxInterruptDisable_Default
( 
    USBHS_MODULE_ID index , 
    USBHS_EPTXRX_INTERRUPT interrupt
)
{
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);

    /* Disable the Transmit endpoint interrupts */
    usbhs->INTRTXEbits.w &= (~(interrupt));
}


//******************************************************************************
/* Function :  USBHS_RxInterruptDisable_Default

  Summary:
    Implements Default variant of PLIB_USBHS_RxInterruptDisable 

  Description:
    This template implements the Default variant of the PLIB_USBHS_RxInterruptDisable function.
*/

PLIB_TEMPLATE void USBHS_RxInterruptDisable_Default
( 
    USBHS_MODULE_ID index , 
    USBHS_EPTXRX_INTERRUPT interrupt
)
{
    volatile usbhs_registers_t * usbhs = (usbhs_registers_t *)(index);

    /* Disable the Receive endpoint interrupts */
    usbhs->INTRRXEbits.w &= (~(interrupt));
}

//******************************************************************************
/* Function :  USBHS_ExistsInterrupts_Default

  Summary:
    Implements Default variant of PLIB_USBHS_ExistsInterrupts

  Description:
    This template implements the Default variant of the PLIB_USBHS_ExistsInterrupts function.
*/

#define PLIB_USBHS_ExistsInterrupts PLIB_USBHS_ExistsInterrupts
PLIB_TEMPLATE bool USBHS_ExistsInterrupts_Default( USBHS_MODULE_ID index )
{
    return true;
}


#endif /*_USBHS_INTERRUPTS_DEFAULT_H*/

/******************************************************************************
 End of File
*/

