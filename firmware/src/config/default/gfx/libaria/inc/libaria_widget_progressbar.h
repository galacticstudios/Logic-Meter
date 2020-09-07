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
    libaria_widget_progressbar.h

  Summary:


  Description:
    This module implements progress bar widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_PROGRESSBAR_H
#define LIBARIA_PROGRESSBAR_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_PROGRESSBAR_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laProgressBarDirection_t

  Summary:
    Defines the valid values for the progress bar widget fill directions.

  Description:


  Remarks:
    None.
*/
typedef enum laProgressBarDirection_t
{
    LA_PROGRESSBAR_DIRECTION_RIGHT,
    LA_PROGRESSBAR_DIRECTION_UP,
    LA_PROGRESSBAR_DIRECTION_LEFT,
    LA_PROGRESSBAR_DIRECTION_DOWN
} laProgressBarDirection;

// DOM-IGNORE-BEGIN
typedef struct laProgressBarWidget_t laProgressBar;
// DOM-IGNORE-END

// *****************************************************************************
/* Function Pointer:
    laProgressBar_ValueChangedEventCallback

  Summary:
    Value changed event function callback type
*/
typedef void (*laProgressBar_ValueChangedEventCallback)(laProgressBar*, uint32_t);

// *****************************************************************************
/* Structure:
    laProgressBarDirection_t

  Summary:
    Implementation of a progressbar widget struct

  Description:
    A progress bar widget is a widget that can fill itself with a color based
    on a given percentage from 0-100.  This is often used to visually illustrate
    the progress of some other activity over time.

  Remarks:
    None.
*/
typedef struct laProgressBarWidget_t
{
    laWidget widget; // base widget class

    laProgressBarDirection direction; // the fill direction of the bar

    uint32_t value; // fill percentage

    laProgressBar_ValueChangedEventCallback cb; // value changed callback
} laProgressBarWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laProgressBarWidget_Constructor(laProgressBarWidget* bar);
void _laProgressBarWidget_Destructor(laProgressBarWidget* bar);

void _laProgressBarWidget_Paint(laProgressBarWidget* bar);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laProgressBarWidget* laProgressBarWidget_New()

  Summary:
    Allocates memory for a new widget of this type.  The application is
    responsible for the managment of this memory until the widget is added to
    a widget tree.

  Description:


  Parameters:

  Returns:
    laProgressBarWidget*

  Remarks:

*/
LIB_EXPORT laProgressBarWidget* laProgressBarWidget_New();

// *****************************************************************************
/* Function:
    laProgressBarDirection laProgressBarWidget_GetDirection(laProgressBarWidget* bar)

  Summary:
    Gets the fill direction value for a progress bar widget

  Description:


  Parameters:
    laProgressBarWidget* bar - the widget

  Returns:
    laProgressBarDirection - the fill direction value

  Remarks:

*/
LIB_EXPORT laProgressBarDirection laProgressBarWidget_GetDirection(laProgressBarWidget* bar);

// *****************************************************************************
/* Function:
    laResult laProgressBarWidget_SetDirection(laProgressBarWidget* bar,
                                              laProgressBarDirection dir)

  Summary:
    Sets the fill direction for a progress bar widget

  Description:


  Parameters:
    laProgressBarWidget* bar - the widget
    laProgressBarDirection dir - the desired fill direction

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laProgressBarWidget_SetDirection(laProgressBarWidget* bar, laProgressBarDirection dir);

// *****************************************************************************
/* Function:
    uint32_t laProgressBarWidget_GetValue(laProgressBarWidget* bar)

  Summary:
    Gets the percentage value for a progress bar.

  Description:

  Parameters:
    laProgressBarWidget* bar - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laProgressBarWidget_GetValue(laProgressBarWidget* bar);

// *****************************************************************************
/* Function:
    laResult laProgressBarWidget_SetValue(laProgressBarWidget* bar, uint32_t value)

  Summary:
    Sets the percentage value for a progress bar.  Valid values are 0 - 100.

  Description:


  Parameters:
    laProgressBarWidget* bar - the widget
    uint32_t value - the desired value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laProgressBarWidget_SetValue(laProgressBarWidget* bar, uint32_t value);

// *****************************************************************************
/* Function:
    laProgressBar_ValueChangedEventCallback laProgressBarWidget_GetValueChangedEventCallback(laProgressBarWidget* bar)

  Summary:
    Gets the currently set value changed event callback.

  Description:


  Parameters:
    laProgressBarWidget* bar - the widget

  Returns:
    laProgressBar_ValueChangedEventCallback - the current callback pointer or NULL

  Remarks:

*/
LIB_EXPORT laProgressBar_ValueChangedEventCallback laProgressBarWidget_GetValueChangedEventCallback(laProgressBarWidget* bar);

// *****************************************************************************
/* Function:
    laResult laProgressBarWidget_SetValueChangedCallback(laProgressBarWidget* bar, laProgressBar_ValueChangedEventCallback cb)

  Summary:
    Sets the desired value changed event callback pointer

  Description:


  Parameters:
    laProgressBarWidget* bar - the widget
    laProgressBar_ValueChangedEventCallback - a valid callback pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laProgressBarWidget_SetValueChangedCallback(laProgressBarWidget* bar, laProgressBar_ValueChangedEventCallback cb);

// DOM-IGNORE-BEGIN
// internal use only
void _laProgressBarWidget_GetBarRect(laProgressBarWidget* bar,
                                     int32_t val1,
                                     int32_t val2,
                                     GFX_Rect* barRect);
// DOM-IGNORE-END

#endif // LA_PROGRESSBAR_WIDGET_ENABLED
#endif /* LIBARIA_PROGRESSBAR_H */
