// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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

/*******************************************************************************
  Module for Microchip Graphics Library - Aria User Interface Library

  Company:
    Microchip Technology Inc.

  File Name:
    libaria_common.h

  Summary:
    This file defines the common macros and definitions used by the gfx
    definition and implementation headers.

  Description:
    This file defines the common macros and definitions used by the gfx
    definition and the implementation header.

  Remarks:
    The directory in which this file resides should be added to the compiler's
    search path for header files.
 *******************************************************************************/

//DOM-IGNORE-BEGIN

#ifndef LIBARIA_COMMON_H
#define LIBARIA_COMMON_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_global.h"

#include "gfx/hal/gfx.h"
#include "gfx/utils/gfx_utils.h"

#include "gfx/libaria/libaria_config.h"

#ifndef WIN32
//#include "osal/osal.h" FIXME for H3
#else
#include "native_common.h"
#endif

// DOM-IGNORE-BEGIN
#ifndef LA_DEFAULT_SCHEME_COLOR_MODE
#define LA_DEFAULT_SCHEME_COLOR_MODE GFX_COLOR_MODE_RGB_565
#endif
// DOM-IGNORE-END

#ifdef __cplusplus
    extern "C" {
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laResult

  Summary:
    libaria results (success and failure codes).

  Description:
    Various definitions for success and failure codes.

  Remarks:
    None.
*/
typedef enum laResult_t
{
    LA_FAILURE = -1,
    LA_SUCCESS = 0
} laResult;

// *****************************************************************************
/* Enumeration:
    laBool

  Summary:
    libaria bool values

  Description:
    libaria bool values

  Remarks:
    None.
*/
typedef enum laBool_t
{
    LA_FALSE = 0,
    LA_TRUE
} laBool;

// *****************************************************************************
/* Enumeration:
    laVAlignment

  Summary:
    libaria vertical alignment values

  Description:
    libaria vertical alignment values

  Remarks:
    None.
*/
typedef enum
{
    LA_VALIGN_TOP,
    LA_VALIGN_MIDDLE,
    LA_VALIGN_BOTTOM
} laVAlignment;

// *****************************************************************************
/* Enumeration:
    laHAlignment

  Summary:
    libaria horizontal alignment values

  Description:
    libaria horizontal alignment values

  Remarks:
    None.
*/
typedef enum
{
    LA_HALIGN_LEFT,
    LA_HALIGN_CENTER,
    LA_HALIGN_RIGHT
} laHAlignment;

// *****************************************************************************
/* Enumeration:
    laMargin

  Summary:
    libaria margin values

  Description:
    libaria margin values

  Remarks:
    None.
*/
typedef struct laMargin_t
{
    uint8_t left;
    uint8_t top;
    uint8_t right;
    uint8_t bottom;
} laMargin;

// *****************************************************************************
/* Enumeration:
    laRelativePosition

  Summary:
    libaria relative position values

  Description:
    libaria relative position values

  Remarks:
    None.
*/
typedef enum laRelativePosition
{
    LA_RELATIVE_POSITION_LEFTOF,
    LA_RELATIVE_POSITION_ABOVE,
    LA_RELATIVE_POSITION_RIGHTOF,
    LA_RELATIVE_POSITION_BELOW,
    LA_RELATIVE_POSITION_BEHIND
} laRelativePosition;

// *****************************************************************************
/* Enumeration:
    laPreemptionLevel

  Summary:
    libaria pre-emption level values

  Description:
    libaria pre-emption level values

  Remarks:
    None.
*/
typedef enum laPreemptionLevel
{
    LA_PREEMPTION_LEVEL_0, // draw cycle always completes
    LA_PREEMPTION_LEVEL_1, // preempts after each widget fully draws
    LA_PREEMPTION_LEVEL_2  // preempts after each widget draw step completes
} laPreemptionLevel;

// DOM-IGNORE-END

#ifdef __cplusplus
    }
#endif
     
#endif // LIBARIA_COMMON_H

/*******************************************************************************
 End of File
*/
