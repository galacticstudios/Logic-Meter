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
    libaria_widget_circular_gauge.h

  Summary:


  Description:
    This module implements circular gauge drawing widget functions.
*******************************************************************************/


#ifndef LIBARIA_WIDGET_CIRCULAR_GAUGE_H
#define LIBARIA_WIDGET_CIRCULAR_GAUGE_H

#include "gfx/libaria/inc/libaria_common.h"

#if LA_CIRCULAR_GAUGE_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

typedef struct laCircularGaugeWidget_t laCircularGaugeWidget;

// *****************************************************************************
/* Structure:
    laCircularGaugeWidgetDir_t

  Summary:
    Direction of the gauge

  Description:
    Direction of the gauge

  Remarks:
    None.
*/
typedef enum laCircularGaugeWidgetDir_t
{
    CIRCULAR_GAUGE_DIR_CLOCKWISE,
    CIRCULAR_GAUGE_DIR_COUNTER_CLOCKWISE,
} laCircularGaugeWidgetDir;

// *****************************************************************************
/* Structure:
    laCircularGaugeWidgetDir_t

  Summary:
    Direction of the gauge

  Description:
    Direction of the gauge

  Remarks:
    None.
*/
typedef enum laCircularGaugeWidgetLabelPosition_t
{
    CIRCULAR_GAUGE_LABEL_OUTSIDE,
    CIRCULAR_GAUGE_LABEL_INSIDE,
} laCircularGaugeWidgetLabelPosition;

// *****************************************************************************
/* Structure:
    laCircularGaugeWidgetArcType

  Summary:
    Type of arc

  Description:
    Type of arc

  Remarks:
    None.
*/
typedef enum laCircularGaugeWidgetArcType_t
{
    ANGLE_ARC,
    VALUE_ARC,
} laCircularGaugeWidgetArcType;

// *****************************************************************************
/* Function Pointer:
    laCircularGaugeWidget_ValueChangedEvent

  Summary:
    Value changed event function callback type
*/
typedef void (*laCircularGaugeWidget_ValueChangedEvent)(laCircularGaugeWidget*,
                                                        int32_t value);


// *****************************************************************************
/* Structure:
    laCircularGaugeWidget_t

  Summary:
    Implementation of a circular gauge widget.

  Description:
    A circular gauge widget draws a circular gauge of the specified properties
    inside the widget bounds.  All coordinates are expressed in local widget space.

  Remarks:
    None.
*/
typedef struct laCircularGaugeWidget_t
{
    laWidget widget; // base widget header

    //Widget properties
    int32_t value;
    int32_t startValue;
    int32_t endValue;
    uint32_t radius; // the radius of the circular gauge
    uint32_t startAngle; //the start angle of the outer arc
    int32_t centerAngle; //the center angle of the outer arc
    laCircularGaugeWidgetDir dir; //the turn direction of the gauge

    //Tick properties
    GFX_Bool ticksVisible; // are ticks visible
    uint32_t tickLength; //length of ticks (towards center)
    int32_t tickValue; // tick value (delta)

    //Tick label properties
    laBool tickLabelsVisible; // are tick labels visible
    GFXU_StringTableAsset * stringTable; //string table
    uint32_t ticksLabelsStringID; //ID of Superset string containing numbers

    //hand properties
    laBool handVisible;
    uint32_t handRadius;
    laBool centerCircleVisible;
    uint32_t centerCircleRadius;
    uint32_t centerCircleThickness;

    //Advanced config arrays
    laArray arcsArray; //ArcsArray list
    laArray ticksArray; //ArcsArray list
    laArray labelsArray; //ArcsArray list


    laCircularGaugeWidget_ValueChangedEvent cb; //value changed event callback

} laCircularGaugeWidget;

// *****************************************************************************
/* Structure:
    laCircularGaugeArc_t

  Summary:
    Internal structure for the arcs in the circular gauge widget

  Description:
    Describs the arc instances in the circular gauge widget

  Remarks:
    None.
*/
typedef struct laCircularGaugeArc_t
{
    laCircularGaugeWidgetArcType type;
    int32_t startAngle;
    int32_t endAngle;
    int32_t startValue;
    int32_t endValue;
    uint32_t radius;
    uint32_t thickness;
    laScheme* scheme;
} laCircularGaugeArc;

// *****************************************************************************
/* Structure:
    laCircularGaugeTick_t

  Summary:
    Tick object for the circular gauge

  Description:
    Contains properties of the ticks in the gauge

  Remarks:
    None.
*/
typedef struct laCircularGaugeTick_t
{
    int32_t startValue;
    int32_t endValue;
    uint32_t interval;
    uint32_t radius;
    uint32_t length;
    laScheme* scheme;
} laCircularGaugeTick;

// *****************************************************************************
/* Structure:
    typedef struct laCircularGaugeLabel_t


  Summary:
    Label object for the circular gauge

  Description:
    Contains properties of the labels in the gauge

  Remarks:
    None.
*/
typedef struct laCircularGaugeLabel_t
{
    int32_t startValue;
    int32_t endValue;
    uint32_t interval;
    uint32_t radius;
    laCircularGaugeWidgetLabelPosition position;
    laScheme* scheme;
} laCircularGaugeLabel;

// DOM-IGNORE-BEGIN
// internal use only
void _laCircularGaugeWidget_Constructor(laCircularGaugeWidget* gauge);
void _laCircularGaugeWidget_Destructor(laCircularGaugeWidget* gauge);
void _laCircularGaugeWidget_Paint(laCircularGaugeWidget* gauge);

// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laCircularGaugeWidget* laCircularGaugeWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the management of this memory until the
    widget is added to a widget tree.

  Description:


  Parameters:
    void

  Returns:
    laCircularGaugeWidget*

  Remarks:

*/
LIB_EXPORT laCircularGaugeWidget* laCircularGaugeWidget_New();

// *****************************************************************************
/* Function:
    uint32_t laCircularGaugeWidget_GetRadius(laCircularGaugeWidget* gauge)

  Summary:
    Gets the radius of a gauge widget

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularGaugeWidget_GetRadius(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetRadius(laCircularGaugeWidget* gauge, uint32_t rad)

  Summary:
    Sets the radius of a gauge widget

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    uint32_t red - the desired radius value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetRadius(laCircularGaugeWidget* gauge, uint32_t rad);

// *****************************************************************************
/* Function:
    int32_t laCircularGaugeWidget_GetStartAngle(laCircularGaugeWidget* gauge)

  Summary:
    Returns the start angle of the circular gauge

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    int32_t

  Remarks:

*/
LIB_EXPORT int32_t laCircularGaugeWidget_GetStartAngle(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetStartAngle(laCircularGaugeWidget* gauge, int32_t angle)

  Summary:
    Sets the start angle of the gauge.

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    int32_t angle - start angle of the gauge

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetStartAngle(laCircularGaugeWidget* gauge, int32_t angle);

// *****************************************************************************
/* Function:
    int32_t laCircularGaugeWidget_GetCenterAngle(laCircularGaugeWidget* gauge)

  Summary:
    Returns the center angle of the circular gauge

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    int32_t

  Remarks:

*/
LIB_EXPORT int32_t laCircularGaugeWidget_GetCenterAngle(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetCenterAngle(laCircularGaugeWidget* gauge, int32_t angle)

  Summary:
    Sets the center angle of the gauge.

  Description:
    A positive center angle draws the gauge, ticks, and hand in CCW, while a negative center
    angle draws in CW.

  Parameters:
    laCircularGaugeWidget* gauge - the widget
    int32_t angle - center angle of the gauge

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetCenterAngle(laCircularGaugeWidget* gauge, int32_t angle);

// *****************************************************************************
/* Function:
    laCircularGaugeWidgetDir laCircularGaugeWidget_GetDirection(laCircularGaugeWidget* gauge)

  Summary:
    Returns the direction of the gauge.

  Description:
    The direction is automatically set to CW if the center angle is negative, and CCW if the
    center angle is positive.


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    laCircularGaugeWidgetDir

  Remarks:

*/
LIB_EXPORT laCircularGaugeWidgetDir laCircularGaugeWidget_GetDirection(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_AddValueArc(laCircularGaugeWidget* gauge,
                                               int32_t startValue,
                                               int32_t endValue,
                                               uint32_t radius,
                                               uint32_t thickness,
                                               laScheme* scheme)

  Summary:
    Adds a 'value arc' in the gauge.

  Description:
    Value arcs are drawn inside the gauge from the same origin/center but are
    bound by the start and end value of the gauge. A value arc that exceeds the start or
    end value of the gauge will not be drawn.

  Parameters:
    laCircularGaugeWidget* gauge - the widget
    int32_t startAngle - the start angle of the arc (relative to gauge starting angle)
    int32_t endAngle - the end angle of the arc (relative to gauge starting angle)
    uint32_t radius - the radius of the arc
    uint32_t thickness - the fill thickness of the arc
    laScheme* scheme - scheme used for drawing the arc


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_AddValueArc(laCircularGaugeWidget* gauge,
                                                      int32_t startValue,
                                                      int32_t endValue,
                                                      uint32_t radius,
                                                      uint32_t thickness,
                                                      laScheme* scheme);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_AddAngularArc(laCircularGaugeWidget* gauge,
                                                 int32_t startAngle,
                                                 int32_t endAngle,
                                                 uint32_t radius,
                                                 uint32_t thickness,
                                                 laScheme* scheme)

  Summary:
    Adds an 'angular arc' in the gauge.

  Description:
    Angular arcs are drawn inside the gauge from the same origin/center but are
    not bound by the start/end angle/value of the gauge.

  Parameters:
    laCircularGaugeWidget* gauge - the widget
    int32_t startAngle - the start angle of the arc (relative to gauge starting angle)
    int32_t endAngle - the end angle of the arc (relative to gauge starting angle)
    uint32_t radius - the radius of the arc
    uint32_t thickness - the fill thickness of the arc
    laScheme* scheme - scheme used for drawing the arc


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_AddAngularArc(laCircularGaugeWidget* gauge,
                                                        int32_t startAngle,
                                                        int32_t endAngle,
                                                        uint32_t radius,
                                                        uint32_t thickness,
                                                        laScheme* scheme);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_DeleteArcs(laCircularGaugeWidget* gauge)

  Summary:
    Deletes all arcs in the gauge widget

  Description:
    Deletes all arcs in the gauge widget

  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_DeleteArcs(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laCircularGaugeWidget_AddMinorTicks(laCircularGaugeWidget* gauge,
                                      int32_t startValue,
                                      int32_t endValue,
                                      uint32_t radius,
                                      uint32_t length,
                                      uint32_t interval,
                                      laScheme* scheme)

  Summary:
    Adds minor ticks in the gauge.

  Description:

  Parameters:
    laCircularGaugeWidget* gauge - the widget
    int32_t startValue - the start value of the ticks (must be within gauge range)
    int32_t endValue - the end value of the ticks (must be within gauge range)
    uint32_t radius - the radius of the ticks
    uint32_t length - the length of the ticks (drawn inward towards center)
    uint32_t interval - the interval between ticks
    laScheme* scheme - scheme used for drawing the tick (uses foreground)


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_AddMinorTicks(laCircularGaugeWidget* gauge,
                                                        int32_t startValue,
                                                        int32_t endValue,
                                                        uint32_t radius,
                                                        uint32_t length,
                                                        uint32_t interval,
                                                        laScheme* scheme);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_DeleteMinorTicks(laCircularGaugeWidget* gauge)

  Summary:
    Deletes all the minor ticks in the gauge widget

  Description:
    Deletes all the minor ticks in the gauge widget

  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_DeleteMinorTicks(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    llaResult laCircularGaugeWidget_AddMinorTickLabels(laCircularGaugeWidget* gauge,
                                      int32_t startValue,
                                      int32_t endValue,
                                      uint32_t radius,
                                      laCircularGaugeWidgetLabelPosition position,
                                      uint32_t interval,
                                      laScheme* scheme)

  Summary:
    Adds minor tick labels in the gauge.

  Description:

  Parameters:
    laCircularGaugeWidget* gauge - the widget
    int32_t startValue - the start value of the reference tick points (must be within gauge range)
    int32_t endValue - the end value of the reference tick points (must be within gauge range)
    uint32_t radius - the radius of the reference tick points
    uint32_t position - the position of the label relative to the tick points
    uint32_t interval - the interval between ticks
    laScheme* scheme - scheme used for drawing the tick (uses foreground)


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_AddMinorTickLabels(laCircularGaugeWidget* gauge,
                                                             int32_t startValue,
                                                             int32_t endValue,
                                                             uint32_t radius,
                                                             laCircularGaugeWidgetLabelPosition position,
                                                             uint32_t interval,
                                                             laScheme* scheme);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_DeleteMinorTickLabels(laCircularGaugeWidget* gauge)

  Summary:
    Deletes all the minor tick labels in the gauge widget

  Description:
    Deletes all the tick labels in the gauge widget

  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_DeleteMinorTickLabels(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    int32_t laCircularGaugeWidget_GetValue(laCircularGaugeWidget* gauge)

  Summary:
    Returns the value of the gauge hand

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    int32_t

  Remarks:

*/
LIB_EXPORT int32_t laCircularGaugeWidget_GetValue(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetValue(laCircularGaugeWidget* gauge,
                                            int32_t value)

  Summary:
    Sets the value of the gauge hand

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    int32_t value - value of the gauge hand


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetValue(laCircularGaugeWidget* gauge,
                                                   int32_t value);

// *****************************************************************************
/* Function:
    int32_t laCircularGaugeWidget_GetStartValue(laCircularGaugeWidget* gauge)

  Summary:
    Returns the start value of the gauge

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    int32_t

  Remarks:

*/
LIB_EXPORT int32_t laCircularGaugeWidget_GetStartValue(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetStartValue(laCircularGaugeWidget* gauge,
                                               int32_t value)

  Summary:
    Sets the start value of the gauge

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    int32_t value - start value of the gauge


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetStartValue(laCircularGaugeWidget* gauge,
                                                        int32_t value);

// *****************************************************************************
/* Function:
    int32_t laCircularGaugeWidget_GetEndValue(laCircularGaugeWidget* gauge)

  Summary:
    Returns the end value of the gauge

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    int32_t

  Remarks:

*/
LIB_EXPORT int32_t laCircularGaugeWidget_GetEndValue(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetEndValue(laCircularGaugeWidget* gauge,
                                               int32_t value)

  Summary:
    Sets the end value of the gauge

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    int32_t value - end value of the gauge


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetEndValue(laCircularGaugeWidget* gauge,
                                                      int32_t value);

// *****************************************************************************
/* Function:
    laBool laCircularGaugeWidget_GetTicksVisible(laCircularGaugeWidget* gauge)

  Summary:
    Returns GFX_TRUE if the ticks in the gauge are visible

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    laBool

  Remarks:

*/
LIB_EXPORT laBool laCircularGaugeWidget_GetTicksVisible(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laCircularGaugeWidget_SetTicksVisible(laCircularGaugeWidget* gauge,
                                       laBool visible)

  Summary:
    Sets the increments/distance between ticks

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    int32_t value - the distance between ticks


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetTicksVisible(laCircularGaugeWidget* gauge,
                                                          laBool visible);

// *****************************************************************************
/* Function:
    int32_t laCircularGaugeWidget_GetTickValue(laCircularGaugeWidget* gauge)

  Summary:
    Returns the tick increment value in the gauge

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    int32_t

  Remarks:

*/
LIB_EXPORT int32_t laCircularGaugeWidget_GetTickValue(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetTickValue(laCircularGaugeWidget* gauge,
                                       int32_t value)

  Summary:
    Sets the increments/distance between ticks

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    int32_t value - the distance between ticks


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetTickValue(laCircularGaugeWidget* gauge,
                                                       int32_t value);

// *****************************************************************************
/* Function:
    uint32_t laCircularGaugeWidget_GetTickLength(laCircularGaugeWidget* gauge)

  Summary:
    Returns the length of the ticks in the gauge in pixels

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularGaugeWidget_GetTickLength(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetTickLength(laCircularGaugeWidget* gauge,
                                         uint32_t length)

  Summary:
    Sets the length of the ticks

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    uint32_t length - length of the ticks in pixels


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetTickLength(laCircularGaugeWidget* gauge,
                                                        uint32_t length);

// *****************************************************************************
/* Function:
    laBool laCircularGaugeWidget_GetTickLabelsVisible(laCircularGaugeWidget* gauge)

  Summary:
    Returns GFX_TRUE if the tick labels are visible

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    laBool

  Remarks:

*/
LIB_EXPORT laBool laCircularGaugeWidget_GetTickLabelsVisible(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetTickLabelsVisible(laCircularGaugeWidget* gauge,
                                               laBool visible)

  Summary:
    Sets the tick labels visible/invisible

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    laBool visible - tick labels are visible if GFX_TRUE


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetTickLabelsVisible(laCircularGaugeWidget* gauge,
                                                               laBool visible);

// *****************************************************************************
/* Function:
    laBool laCircularGaugeWidget_GetHandVisible(laCircularGaugeWidget* gauge)

  Summary:
    Returns GFX_TRUE if the gauge hand is visible

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    laBool

  Remarks:

*/
LIB_EXPORT laBool laCircularGaugeWidget_GetHandVisible(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetHandVisible(laCircularGaugeWidget* gauge,
                                         laBool visible)

  Summary:
    Sets the hand visible/invisible

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    laBool visible - hand is visible if GFX_TRUE


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetHandVisible(laCircularGaugeWidget* gauge,
                                                         laBool visible);

// *****************************************************************************
/* Function:
    uint32_t laCircularGaugeWidget_GetHandRadius(laCircularGaugeWidget* gauge)

  Summary:
    Returns the radius/length of the gauge hand in pixels

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularGaugeWidget_GetHandRadius(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetHandRadius(laCircularGaugeWidget* gauge,
                                        uint32_t length)

  Summary:
    Sets the radius/length of the hand

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    uint32_t length - length of the hand in pixels


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetHandRadius(laCircularGaugeWidget* gauge,
                                                        uint32_t rad);

// *****************************************************************************
/* Function:
    laBool laCircularGaugeWidget_GetCenterCircleVisible(laCircularGaugeWidget* gauge)

  Summary:
    Returns GFX_TRUE if the center circle is visible

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    laBool

  Remarks:

*/
LIB_EXPORT laBool laCircularGaugeWidget_GetCenterCircleVisible(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetCenterCircleVisible(laCircularGaugeWidget* gauge,
                                                 laBool visible)

  Summary:
    Sets the center circle visible/invisible

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    laBool visible - sets visible if GFX_TRUE


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetCenterCircleVisible(laCircularGaugeWidget* gauge,
                                                                 laBool visible);

// *****************************************************************************
/* Function:
    uint32_t laCircularGaugeWidget_GetCenterCircleRadius(laCircularGaugeWidget* gauge)

  Summary:
    Returns radius of the center circle

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularGaugeWidget_GetCenterCircleRadius(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetCenterCircleRadius(laCircularGaugeWidget* gauge,
                                                            uint32_t rad)

  Summary:
    Sets the center radius of the center circle

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    uint32_t rad - radius of the center circle


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetCenterCircleRadius(laCircularGaugeWidget* gauge,
                                                                uint32_t rad);

// *****************************************************************************
/* Function:
    uint32_t laCircularGaugeWidget_GetCenterCircleThickness(laCircularGaugeWidget* gauge)

  Summary:
    Returns thickness of the center circle

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget

  Returns:
    uint32_t

  Remarks:

*/
LIB_EXPORT uint32_t laCircularGaugeWidget_GetCenterCircleThickness(laCircularGaugeWidget* gauge);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetCenterCircleThickness(laCircularGaugeWidget* gauge,
                                                            uint32_t thickness)

  Summary:
    Sets the thickness of the center circle

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    uint32_t thickness - thickness of the center circle


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetCenterCircleThickness(laCircularGaugeWidget* gauge,
                                                                   uint32_t thickness);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetStringTable(laCircularGaugeWidget* gauge,
                                                  GFXU_StringTableAsset * stringTable)

  Summary:
    Sets the string table to be used for the tick labels

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    GFXU_StringTableAsset * stringTable - pointer to the string table


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetStringTable(laCircularGaugeWidget* gauge,
                                                         GFXU_StringTableAsset * stringTable);

// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetTicksLabelsStringID(laCircularGaugeWidget* gauge,
                                                          uint32_t stringID)

  Summary:
    Sets the ID of the string character superset to be used for the tick labels

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    uint32_t stringID - string ID


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetTicksLabelsStringID(laCircularGaugeWidget* gauge,
                                                                 uint32_t stringID);


// *****************************************************************************
/* Function:
    laResult laCircularGaugeWidget_SetValueChangedEventCallback(laCircularGaugeWidget* gauge,
                                                                 laCircularGaugeWidget_ValueChangedEvent cb)

  Summary:
    Sets the function to be called back when the gauge value changes.

  Description:


  Parameters:
    laCircularGaugeWidget* gauge - the widget
    laCircularGaugeWidget_ValueChangedEvent cb - callback function


  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laCircularGaugeWidget_SetValueChangedEventCallback(laCircularGaugeWidget* gauge,
                                                                        laCircularGaugeWidget_ValueChangedEvent cb);

#endif // LA_CIRCULAR_GAUGE_WIDGET_ENABLED
#endif /* LIBARIA_WIDGET_CIRCULAR_GAUGE_H */