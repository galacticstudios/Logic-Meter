/*******************************************************************************
  Output Compare OCMP4 Peripheral Library (PLIB)

  Company:
    Microchip Technology Inc.

  File Name:
    plib_ocmp4.c

  Summary:
    OCMP4 Source File

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
#include "plib_ocmp4.h"

// *****************************************************************************

// *****************************************************************************
// Section: OCMP4 Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************


void OCMP4_Initialize (void)
{
    /*Setup OC4CON        */
    /*OCM         = 5        */
    /*OCTSEL       = 0        */
    /*OC32         = 1        */
    /*SIDL         = false    */

    OC4CON = 0x25;

    OC4R = 2000;
    OC4RS = 4000;

}

void OCMP4_Enable (void)
{
    OC4CONSET = _OC4CON_ON_MASK;
}

void OCMP4_Disable (void)
{
    OC4CONCLR = _OC4CON_ON_MASK;
}


void OCMP4_CompareValueSet (uint32_t value)
{
    OC4R = value;
}

uint32_t OCMP4_CompareValueGet (void)
{
    return OC4R;
}

void OCMP4_CompareSecondaryValueSet (uint32_t value)
{
    OC4RS = value;
}

uint32_t OCMP4_CompareSecondaryValueGet (void)
{
    return OC4RS;
}


