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
    libaria_widget_touchtest.h

  Summary:


  Description:
    This module implements graphical touch test (box) widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_TOUCHTEST_H
#define LIBARIA_TOUCHTEST_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_TOUCHTEST_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

// DOM-IGNORE-BEGIN
#define LA_TOUCHTEST_MEMORY_SIZE 20
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laTouchTestState_t

  Summary:
    Touch test states

  Description:


  Remarks:
    None.
*/
typedef enum laTouchTestState_t
{
    LA_TOUCHTEST_STATE_UP,
    LA_TOUCHTEST_STATE_DOWN
} laTouchTestState;

typedef struct laTouchTestWidget_t laTouchTestWidget;

// *****************************************************************************
/* Function Pointer:
    laTouchTestWidget_PointAddedEventCallback

  Summary:
    Point added event function callback type
*/
typedef void (*laTouchTestWidget_PointAddedEventCallback)(laTouchTestWidget*, GFX_Point*);

// *****************************************************************************
/* Structure:
    laTouchTestWidget_t

  Summary:
    Implementation of a touch test widget struct

  Description:
    The touch test widget is a specialized widget that displays intersecting
    lines based on input events.  This can help visualize touch interaction
    and aid determining accurate input coordinates.

  Remarks:
    None.
*/
typedef struct laTouchTestWidget_t
{
    laWidget widget; // widget base class

    laTouchTestState state; // touch test state

    GFX_Point pnts[LA_TOUCHTEST_MEMORY_SIZE]; // touch point array
    uint32_t size; // current number of valid touch points

    uint32_t start; // first valid touch point
    uint32_t next; // next available touch point entry

    laTouchTestWidget_PointAddedEventCallback cb; // point added callback
} laTouchTestWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laTouchTestWidget_Constructor(laTouchTestWidget* tch);
void _laTouchTestWidget_Destructor(laTouchTestWidget* tch);

void _laTouchTestWidget_Paint(laTouchTestWidget* tch);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laTouchTestWidget* laTouchTestWidget_New()

  Summary:
    Allocates memory for a new widget of this type.  The application is
    responsible for the managment of this memory until the widget is added to
    a widget tree.

  Description:


  Parameters:

  Returns:
    laTouchTestWidget*

  Remarks:

*/
LIB_EXPORT laTouchTestWidget* laTouchTestWidget_New();

// *****************************************************************************
/* Function:
    laResult laTouchTest_AddPoint(laTouchTestWidget* tch, GFX_Point* pnt)

  Summary:
    Adds a point to the touch test widget.  The point will then be displayed.

  Description:


  Parameters:
    laTouchTestWidget* tch - the widget
    GFX_Point* pnt - a pointer to the point to add

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTouchTest_AddPoint(laTouchTestWidget* tch, GFX_Point* pnt);

// *****************************************************************************
/* Function:
    laResult laTouchTest_ClearPoints(laTouchTestWidget* tch)

  Summary:
    Clears all of the existing touch points

  Description:


  Parameters:
    laTouchTestWidget* tch - the widget

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTouchTest_ClearPoints(laTouchTestWidget* tch);

// *****************************************************************************
/* Function:
    laTouchTestWidget_PointAddedEventCallback laTouchTestWidget_GetPointAddedEventCallback(laTouchTestWidget* txt)

  Summary:
    Gets the current point added event callback

  Description:


  Parameters:
    laTouchTestWidget* tch - the widget

  Returns:
    laTouchTestWidget_PointAddedEventCallback - a valid pointer or NULL

  Remarks:

*/
LIB_EXPORT laTouchTestWidget_PointAddedEventCallback laTouchTestWidget_GetPointAddedEventCallback(laTouchTestWidget* txt);

// *****************************************************************************
/* Function:
    laResult laTouchTestWidget_SetPointAddedEventCallback(laTouchTestWidget* txt,
                                                          laTouchTestWidget_PointAddedEventCallback cb)

  Summary:
    Sets the point added event callback

  Description:


  Parameters:
    laTouchTestWidget* tch - the widget
    laTouchTestWidget_PointAddedEventCallback cb - a valid pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laTouchTestWidget_SetPointAddedEventCallback(laTouchTestWidget* txt,
                                                                 laTouchTestWidget_PointAddedEventCallback cb);
// DOM-IGNORE-BEGIN
// internal use only
void _laTouchTestWidget_GetLineRects(laTouchTestWidget* tch,
                                     uint32_t idx,
                                     GFX_Rect* horzRect,
                                     GFX_Rect* vertRect);
// DOM-IGNORE-END

#endif // LA_TOUCHTEST_WIDGET_ENABLED
#endif /* LIBARIA_TOUCHTEST_H */
