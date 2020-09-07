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
    libaria_widget_drawsurface.h

  Summary:


  Description:
    This module implements surface container drawing functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_DRAWSURFACE_H
#define LIBARIA_DRAWSURFACE_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_DRAWSURFACE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

typedef struct laDrawSurfaceWidget_t laDrawSurfaceWidget;

// *****************************************************************************
/* Function Pointer:
    laDrawSurfaceWidget_DrawCallback

  Summary:
    Draw surface draw event function callback type
*/
typedef laBool (*laDrawSurfaceWidget_DrawCallback)(laDrawSurfaceWidget* sfc,
                                                   GFX_Rect* bounds);

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Structure:
    laDrawSurfaceWidget_t

  Summary:
    Implementation of a Drawsurface widget.

  Description:
    A draw surface widget is a special widget that allows an application to
    perform custom HAL draw calls during Aria's paint loop.  To use, create
    and add a draw surface widget to the desired place in the widget tree.
    Then register for the callback through the API
    'laDrawSurfaceWidget_SetDrawCallback'.  This callback occurs during the
    paint loop.  The application should then be free to adjust the HAL
    draw state and issue draw calls as desired.  The HAL layer, buffer, or
    context state must not be adjusted in any way.

    It is also important to not stall for too long during the draw callback.

  Remarks:
    None.
*/
typedef struct laDrawSurfaceWidget_t
{
    laWidget widget; // the widget base class

    laDrawSurfaceWidget_DrawCallback cb; // the draw callback
} laDrawSurfaceWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laDrawSurfaceWidget_Constructor(laDrawSurfaceWidget* sfc);
void _laDrawSurfaceWidget_Destructor(laDrawSurfaceWidget* sfc);

void _laDrawSurfaceWidget_Paint(laDrawSurfaceWidget* sfc);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laDrawSurfaceWidget* laDrawSurfaceWidget_New()

  Summary:
    Allocates memory for a new DrawSurface widget.

  Description:
    Allocates memory for a new DrawSurface widget.  The application is
    responsible for the managment of this memory until the widget is added to
    a widget tree.

  Parameters:
    void

  Returns:
    laDrawSurfaceWidget*

  Remarks:

*/
LIB_EXPORT laDrawSurfaceWidget* laDrawSurfaceWidget_New();

// *****************************************************************************
/* Function:
    laDrawSurfaceWidget_DrawCallback laDrawSurfaceWidget_GetDrawCallback(laDrawSurfaceWidget* sfc)

  Summary:
    Returns the pointer to the currently set draw callback.

  Description:


  Parameters:
    laDrawSurfaceWidget* sfc - the widget

  Returns:
    laDrawSurfaceWidget_DrawCallback - a valid callback pointer or NULL

  Remarks:

*/
LIB_EXPORT laDrawSurfaceWidget_DrawCallback laDrawSurfaceWidget_GetDrawCallback(laDrawSurfaceWidget* sfc);

// *****************************************************************************
/* Function:
    laResult laDrawSurfaceWidget_SetDrawCallback(laDrawSurfaceWidget* sfc,
                                                    laDrawSurfaceWidget_DrawCallback cb)

  Summary:
    Sets the draw callback pointer for the draw surface widget.

  Description:
    Sets the draw callback pointer for the draw surface widget.  This callback will
    be called during Aria's paint loop and allows the application to perform
    HAL draw calls.  The application should not adjust HAL layer, buffer, or context
    options in any way during this phase.

    The callback should return GFX_TRUE if it has completed drawing.  Returning
    GFX_FALSE will indicate to the renderer that the DrawSurface requires more
    time to draw and will call it again during the next paint loop.

  Parameters:
    laDrawSurfaceWidget* sfc - the widget
    laDrawSurfaceWidget_DrawCallback - a valid callback pointer or NULL

  Returns:
    laResult - the result of the operation
*/
LIB_EXPORT laResult laDrawSurfaceWidget_SetDrawCallback(laDrawSurfaceWidget* sfc,
                                                           laDrawSurfaceWidget_DrawCallback cb);

#endif // LA_DRAWSURFACE_WIDGET_ENABLED
#endif /* LIBARIA_DRAWSURFACE_H */
