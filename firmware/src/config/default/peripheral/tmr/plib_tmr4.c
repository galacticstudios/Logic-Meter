/*******************************************************************************
  TMR Peripheral Library Interface Source File

  Company
    Microchip Technology Inc.

  File Name
    plib_tmr4.c

  Summary
    TMR4 peripheral library source file.

  Description
    This file implements the interface to the TMR peripheral library.  This
    library provides access to and control of the associated peripheral
    instance.

*******************************************************************************/

// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include "plib_tmr4.h"




void TMR4_Initialize(void)
{
    /* Disable Timer */
    T4CONCLR = _T4CON_ON_MASK;

    /*
    SIDL = 1
    TCKPS =5
    T32   = 0
    TCS = 0
    */
    T4CONSET = 0x2050;

    /* Clear counter */
    TMR4 = 0x0;

    /*Set period */
    PR4 = 1545U;


}


void TMR4_Start(void)
{
    T4CONSET = _T4CON_ON_MASK;
}


void TMR4_Stop (void)
{
    T4CONCLR = _T4CON_ON_MASK;
}

void TMR4_PeriodSet(uint16_t period)
{
    PR4  = period;
}

uint16_t TMR4_PeriodGet(void)
{
    return (uint16_t)PR4;
}

uint16_t TMR4_CounterGet(void)
{
    return (uint16_t)(TMR4);
}


uint32_t TMR4_FrequencyGet(void)
{
    return (3093750);
}


