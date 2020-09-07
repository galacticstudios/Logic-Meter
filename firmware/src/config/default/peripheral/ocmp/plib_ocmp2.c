/*******************************************************************************
  Output Compare OCMP2 Peripheral Library (PLIB)

  Company:
    Microchip Technology Inc.

  File Name:
    plib_ocmp2.c

  Summary:
    OCMP2 Source File

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
#include "plib_ocmp2.h"

// *****************************************************************************

// *****************************************************************************
// Section: OCMP2 Implementation
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************


void OCMP2_Initialize (void)
{
    /*Setup OC2CON        */
    /*OCM         = 6        */
    /*OCTSEL       = 0        */
    /*OC32         = 0        */
    /*SIDL         = false    */

    OC2CON = 0x6;

    CFGCON |= 0x00010000;
    OC2R = 100;
    OC2RS = 100;

}

void OCMP2_Enable (void)
{
    OC2CONSET = _OC2CON_ON_MASK;
}

void OCMP2_Disable (void)
{
    OC2CONCLR = _OC2CON_ON_MASK;
}



uint16_t OCMP2_CompareValueGet (void)
{
    return (uint16_t)OC2R;
}

void OCMP2_CompareSecondaryValueSet (uint16_t value)
{
    OC2RS = value;
}

uint16_t OCMP2_CompareSecondaryValueGet (void)
{
    return (uint16_t)OC2RS;
}

