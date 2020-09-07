/*******************************************************************************
  Input Capture (ICAP) Peripheral Library Interface Header File

  Company:
    Microchip Technology Inc.

  File Name:
    plib_icap4.h

  Summary:
    ICAP PLIB Header File

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

#ifndef _PLIB_ICAP4_H
#define _PLIB_ICAP4_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "device.h"
#include "plib_icap_common.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END


// *****************************************************************************
// Section: Interface
// *****************************************************************************
// *****************************************************************************

// *************************** ICAP4 API ***************************************/
// *****************************************************************************
/* Function:
   void ICAP4_Initialize (void)

  Summary:
    Initialization function ICAP4 peripheral

  Description:
    This function initializes the ICAP4 peripheral with user input
    from the configurator.

  Parameters:
    none

  Returns:
    void
*/
void ICAP4_Initialize (void);

// *****************************************************************************
/* Function:
   void ICAP4_Enable (void)

  Summary:
    Enable function for the ICAP4 peripheral

  Description:
    This function enables the ICAP4 peripheral.

  Parameters:
    none

  Returns:
    void
*/
void ICAP4_Enable (void);

// *****************************************************************************
/* Function:
   void ICAP4_Disable (void)

  Summary:
    Disable function for the ICAP4 peripheral

  Description:
    This function disables the ICAP4 peripheral.

  Parameters:
    none

  Returns:
    void
*/
void ICAP4_Disable (void);

// *****************************************************************************
/* Function:
   uint32_t ICAP4_CaptureBufferRead (void)

  Summary:
    Read buffer function ICAP4 peripheral

  Description:
    This function will return the value contained in the ICAP4 peripheral
    buffer.

  Parameters:
    none

  Returns:
    uint32_t
*/
uint32_t ICAP4_CaptureBufferRead (void);


// *****************************************************************************
/* Function:
   void ICAP4_CaptureStatusGet (void)

  Summary:
    ICAP4 status

  Description:
    Returns the current state overflow or buffer not empty flags

  Parameters:
    None

  Returns:
    bool
*/
bool ICAP4_CaptureStatusGet (void);


// *****************************************************************************
/* Function:
   void ICAP4_ErrorStatusGet (void)

  Summary:
    ICAP4 status

  Description:
    Returns the current state of overflow

  Parameters:
    None

  Returns:
    bool
*/
bool ICAP4_ErrorStatusGet (void);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
    }
#endif

// DOM-IGNORE-END
#endif // _PLIB_ICAP4_H
