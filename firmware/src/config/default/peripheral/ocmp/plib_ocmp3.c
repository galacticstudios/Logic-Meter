/*******************************************************************************
  Output Compare OCMP3 Peripheral Library (PLIB)

  Company:
    Microchip Technology Inc.

  File Name:
    plib_ocmp3.c

  Summary:
    OCMP3 Source File

  Description:
    None

*******************************************************************************/

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
#include "plib_ocmp3.h"

// *****************************************************************************

// *****************************************************************************
// Section: OCMP3 Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************


void OCMP3_Initialize (void)
{
    /*Setup OC3CON        */
    /*OCM         = 5        */
    /*OCTSEL       = 0        */
    /*OC32         = 0        */
    /*SIDL         = false    */

    OC3CON = 0x5;

    CFGCON |= 0x00010000;
    OC3R = 0;
    OC3RS = 4000;

}

void OCMP3_Enable (void)
{
    OC3CONSET = _OC3CON_ON_MASK;
}

void OCMP3_Disable (void)
{
    OC3CONCLR = _OC3CON_ON_MASK;
}


void OCMP3_CompareValueSet (uint16_t value)
{
    OC3R = value;
}

uint16_t OCMP3_CompareValueGet (void)
{
    return (uint16_t)OC3R;
}

void OCMP3_CompareSecondaryValueSet (uint16_t value)
{
    OC3RS = value;
}

uint16_t OCMP3_CompareSecondaryValueGet (void)
{
    return (uint16_t)OC3RS;
}

