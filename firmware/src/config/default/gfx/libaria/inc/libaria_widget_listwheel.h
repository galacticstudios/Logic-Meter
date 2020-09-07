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
    libaria_widget_listwheel.h

  Summary:


  Description:
    This module implements list wheel (drawing-style list box) widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_LISTWHEELWIDGET_H
#define LIBARIA_LISTWHEELWIDGET_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_LISTWHEEL_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_list.h"
#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_widget_scrollbar.h"
#include "gfx/libaria/inc/libaria_string.h"

typedef struct laListWheelWidget_t laListWheelWidget;

// *****************************************************************************
/* Function Pointer:
    laListWheelWidget_SelectedItemChangedEvent

  Summary:
    Selected item changed event function callback type
*/
typedef void (*laListWheelWidget_SelectedItemChangedEvent)(laListWheelWidget*,
                                                           uint32_t idx);

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laListWheelIndicatorFill

  Summary:
    Indicates the fill type for the listwheel indicator area.
*/
typedef enum laListWheelIndicatorFill_t
{
    LA_LISTWHEEL_INDICATOR_FILL_NONE,
    LA_LISTWHEEL_INDICATOR_FILL_SOLID,
    LA_LISTWHEEL_INDICATOR_FILL_GRADIENT,
} laListWheelIndicatorFill;

// *****************************************************************************
/* Enumeration:
    laListWheelEffects

  Summary:
    Indicates the zoom effects for the list wheel items.
*/
typedef enum laListWheelZoomEffects_t
{
    LA_LISTWHEEL_ZOOM_EFFECT_NONE,
    LA_LISTWHEEL_ZOOM_EFFECT_VSCALE,
    LA_LISTWHEEL_ZOOM_EFFECT_FULL_SCALE,
    LA_LISTWHEEL_ZOOM_EFFECT_FIXED_SCALE,
} laListWheelZoomEffects;

// *****************************************************************************
/* Structure:
    laListWheelItem_t

  Summary:
    Implementation of a list wheel widget item struct

  Description:
    A list wheel item contains either a text string, an icon, or both

  Remarks:
    None.
*/
typedef struct laListWheelItem_t
{
    laString string;
    GFXU_ImageAsset* icon;
} laListWheelItem;

// *****************************************************************************
/* Structure:
    laListWheelWidget_t

  Summary:
    Implementation of a list wheel widget struct

  Description:
    A list wheel widget is a widget that is similar to a normal list widget
    but can be dragged up or down to cycle through a single active value.  This
    widget is also capable of momentum and motion over time.

  Remarks:
    None.
*/
typedef struct laListWheelWidget_t
{
    laWidget widget; // widget base class

    laArray items; // list of items for the wheel
    int32_t selectedItem; // currently selected item
    int32_t visibleItems; // number of visible items in the wheel
                          // must be odd and >= 3
    int32_t topItem; // the current top item

    laHAlignment halign; // the horizontal alignment of the items
    laRelativePosition iconPos; // the icon position of the items
    uint32_t iconMargin; // the icon margin of the items

    laBool showIndicators; // controls the visibility of the horizontal
                           // indicator bars in the center of the widget
    uint32_t indicatorArea; // controls the distance between the indicator bars

    uint32_t shaded; // determins if the background of the widget uses gradient
                     // shading to show depth

    int32_t cycleDistance; // determins the amount of drag distance needed
                           // to cycle between items
    int32_t cycleDelta; // tracks the current amount of drag distance

    // these track drag movement over time
    int32_t firstTouchY;
    int32_t touchY;
    int32_t lastTouchY;
    laBool  stillTouching;

    int32_t minFlickDelta; // amount of distance that must be dragged in a single
                           // frame to trigger momentum mode

    int32_t momentum; // current momentum value
    int32_t maxMomentum; // maximum momentum value
    int32_t momentumFalloff; // momentum falloff rate
    int32_t rotation; // determines actual rotation of the wheel
    int32_t rotationCounter; // time-based limiter for rotation calculations
    int32_t rotationTick; // rotation time accumulator

    laBool snapPending;

    laListWheelIndicatorFill indicatorFill; // the indicator's fill type
    laListWheelZoomEffects zoomEffects; // zoomEffects
    laBool autoHideWheel;   //auto hides the wheel
    laBool hideWheel;       //flag to hide/show the wheel

    struct
    {
        int32_t y;
        int32_t per;

        uint32_t nextItem;
    } paintState;

    laListWheelWidget_SelectedItemChangedEvent cb; // item changed callback

    laBorderType borderTypeCache; // Copy of border type, used to restore borders on auto-hide
    laBackgroundType backgroundTypeCache; // Copy of background type, used to restore borders on auto-hide

    GFXU_ExternalAssetReader* reader; // asset reader
} laListWheelWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laListWheelWidget_Constructor(laListWheelWidget* whl);
void _laListWheelWidget_Destructor(laListWheelWidget* whl);

void _laListWheelWidget_Paint(laListWheelWidget* whl);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laListWheelWidget* laListWheelWidget_New()

  Summary:
    Allocates memory for a new widget of this type.  The application is
    responsible for the managment of this memory until the widget is added to
    a widget tree.

  Description:


  Parameters:

  Returns:
    laListWheelWidget*

  Remarks:

*/
LIB_EXPORT laListWheelWidget* laListWheelWidget_New();

// *****************************************************************************
/* Function:
    laHAlignment laListWheelWidget_GetAlignment(laListWheelWidget* whl)

  Summary:
    Gets the horizontal alignment for the list widget

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    laHAlignment - the current list halign mode

  Remarks:

*/
LIB_EXPORT laHAlignment laListWheelWidget_GetAlignment(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetAlignment(laListWheelWidget* whl,
                                                  laHAlignment align)

  Summary:
    Sets the horizontal alignment mode for the list widget.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget
    laHAlignment align - the desired halign mode

  Returns:
    laResult

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetAlignment(laListWheelWidget* whl,
                                                   laHAlignment align);

// *****************************************************************************
/* Function:
    uint32_t laListWheelWidget_GetVisibleItemCount(laListWheelWidget* whl)

  Summary:
    Returns the list's visible item count

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    uint32_t - the number of visible items

  Remarks:

*/
LIB_EXPORT uint32_t laListWheelWidget_GetVisibleItemCount(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetVisibleItemCount(laListWheelWidget* whl,
                                                   uint32_t cnt)

  Summary:
    Sets the number of visible items in the list.  Must be greater than or equal
    to three and must be an odd number.

  Description:


  Parameters:
    laListWidget* lst - the widget
    uint32_t cnt - the desired number of items

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetVisibleItemCount(laListWheelWidget* whl,
                                                          uint32_t cnt);


// *****************************************************************************
/* Function:
    laRelativePosition laListWheelWidget_GetIconPosition(laListWheelWidget* whl)

  Summary:
    Sets the icon position for the list wheel widget.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    laRelativePosition - the current icon position

  Remarks:

*/
LIB_EXPORT laRelativePosition laListWheelWidget_GetIconPosition(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetIconPosition(laListWheelWidget* whl,
                                                      laRelativePosition pos)

  Summary:
    Sets the icon position for the list wheel widget

  Description:


  Parameters:
    laListWheelWidget* whl - the widget
    laRelativePosition pos - the relative position setting

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetIconPosition(laListWheelWidget* whl,
                                                      laRelativePosition pos);

// *****************************************************************************
/* Function:
    uint32_t laListWheelWidget_GetIconMargin(laListWheelWidget* whl)

  Summary:
    Gets the icon margin value for the list wheel widget

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    uint32_t - the icon margin value

  Remarks:

*/
LIB_EXPORT uint32_t laListWheelWidget_GetIconMargin(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetIconMargin(laListWheelWidget* whl, uint32_t mg)

  Summary:
    Sets the icon margin value for the list widget.

  Description:
    The icon margin value is the distance between the icon image and the text.

  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t mg - the margin value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetIconMargin(laListWheelWidget* whl,
                                                    uint32_t mg);

// *****************************************************************************
/* Function:
    laBool laListWheelWidget_GetShowIndicators(laListWheelWidget* whl)

  Summary:
    Returns true if the list is displaying its selected item indicators

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    laBool - true if the indicators are being shown

  Remarks:

*/
LIB_EXPORT laBool laListWheelWidget_GetShowIndicators(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetShowIndicators(laListWheelWidget* whl,
                                                 laBool b)

  Summary:
    Configures the list to display the selected item indicator bars

  Description:


  Parameters:
    laListWheelWidget* whl - the widget
    laBool b - configures the indicator bar display state

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetShowIndicators(laListWheelWidget* whl,
                                                        laBool b);

// *****************************************************************************
/* Function:
    uint32_t laListWheelWidget_GetIndicatorArea(laListWheelWidget* whl)

  Summary:
    Returns the spacing for the selected item indicator bars.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    uint32_t - the display area

  Remarks:

*/
LIB_EXPORT uint32_t laListWheelWidget_GetIndicatorArea(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetIndicatorArea(laListWheelWidget* whl,
                                                uint32_t area)

  Summary:
    Configures the display area for the list selection indicator bars

  Description:
    This space is measured from the middle of the widget outward.

  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t area - the display area for the indicator bars

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetIndicatorArea(laListWheelWidget* whl,
                                                       uint32_t area);

// *****************************************************************************
/* Function:
    laBool laListWheelWidget_GetShaded(laListWheelWidget* whl)

  Summary:
    Returns true if the list is using gradient shading to illustrate depth

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    laBool - true gradient shading is being used

  Remarks:

*/
LIB_EXPORT laBool laListWheelWidget_GetShaded(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetShaded(laListWheelWidget* whl,
                                         laBool b)

  Summary:
    Configures the list to use gradient or flat background shading

  Description:


  Parameters:
    laListWheelWidget* whl - the widget
    laBool b - true if gradient shading should be used

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetShaded(laListWheelWidget* whl,
                                                laBool b);

// *****************************************************************************
/* Function:
    uint32_t laListWheelWidget_GetFlickInitSpeed(laListWheelWidget* whl)

  Summary:
    Returns the flick init speed for the wheel.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    uint32_t - the flick init speed value

  Remarks:

*/
LIB_EXPORT uint32_t laListWheelWidget_GetFlickInitSpeed(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetFlickInitSpeed(laListWheelWidget* whl,
                                                 uint32_t speed)

  Summary:
    Configures the flick init speed for the list wheel

  Description:
    The flick init speed is the drag distance needed to move the wheel into
    momentum mode.  It is the distance that must be covered from one Aria
    update frame to another.

  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t speed - the flick init speed value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetFlickInitSpeed(laListWheelWidget* whl,
                                                        uint32_t speed);

// *****************************************************************************
/* Function:
    uint32_t laListWheelWidget_GetMaxMomentum(laListWheelWidget* whl)

  Summary:
    Returns the maximum momentum value for the wheel.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    uint32_t - the maximum momentum value.

  Remarks:

*/
LIB_EXPORT uint32_t laListWheelWidget_GetMaxMomentum(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetMaxMomentum(laListWheelWidget* whl,
                                              uint32_t max)

  Summary:
    Configures the maximum momentum value for the wheel

  Description:
    When a wheel is in momenum mode addition drag/flick gestures will add
    more momentum to the wheel.  The maximum momentum value governs the maximum
    speed at which the wheel can rotate at any single point in time.

  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t max - the maximum momentum value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetMaxMomentum(laListWheelWidget* whl,
                                                     uint32_t max);

// *****************************************************************************
/* Function:
    uint32_t laListWheelWidget_GetMomentumFalloffRate(laListWheelWidget* whl)

  Summary:
    Returns the momentum falloff rate for the wheel.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    uint32_t - the momentum falloff rate value.

  Remarks:

*/
LIB_EXPORT uint32_t laListWheelWidget_GetMomentumFalloffRate(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetMomentumFalloffRate(laListWheelWidget* whl,
                                                      uint32_t rate)

  Summary:
    Configures the momentum falloff rate for the wheel

  Description:
    When a wheel is in momenum mode and during each rotation update tick the
    wheel will reduce its current momentum value by this falloff percentage.
    The higher this value is the faster a wheel will slow down.  The wheel is
    limited to integer math so the lowest this value can be is one.

  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t max - the momentum falloff value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetMomentumFalloffRate(laListWheelWidget* whl,
                                                             uint32_t rate);

// *****************************************************************************
/* Function:
    uint32_t laListWheelWidget_GetRotationUpdateRate(laListWheelWidget* whl)

  Summary:
    Returns the wheel rotation update rate.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    uint32_t - the rotation update rate value.

  Remarks:

*/
LIB_EXPORT uint32_t laListWheelWidget_GetRotationUpdateRate(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetRotationUpdateRate(laListWheelWidget* whl,
                                                     uint32_t ms)

  Summary:
    Configures the rotation update rate for a wheel

  Description:
    When a wheel is in momenum mode it may be too costly to update with every
    Aria update loop call.  This value can delay a wheel update.  For instance,
    if Aria is updating every 20ms, the wheel can be set to update every 60ms
    and it will update approximately every three to four Aria updates.  This
    can cut down on the number of repaints the wheel needs to perform and can
    also slow the wheel down it if is rotating too fast for the application
    to handle.  This value is typically expressed in milliseconds.

  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t ms - the desired rotation update rate

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetRotationUpdateRate(laListWheelWidget* whl,
                                                            uint32_t ms);

// *****************************************************************************
/* Function:
    uint32_t laListWheelWidget_GetItemCount(laListWheelWidget* whl)

  Summary:
    Gets the number of items currently contained in the list

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    uint32_t - the number of items in the list

  Remarks:

*/
LIB_EXPORT uint32_t laListWheelWidget_GetItemCount(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    uint32_t laListWheelWidget_AppendItem(laListWheelWidget* whl)

  Summary:
    Appends a new item entry to the list.  The initial value of the item
    will be empty.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    uint32_t - the index of the newly appended item

  Remarks:

*/
LIB_EXPORT uint32_t laListWheelWidget_AppendItem(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    uint32_t laListWheelWidget_InsertItem(laListWheelWidget* whl, uint32_t idx)

  Summary:
    Attempts to insert a new item at the desired index.  Existing items at
    idx or greater will be shuffled one index to the right.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t idx - the desired index of the new item

  Returns:
    uint32_t - the index of the inserted item

  Remarks:

*/
LIB_EXPORT uint32_t laListWheelWidget_InsertItem(laListWheelWidget* whl, uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_RemoveItem(laListWheelWidget* whl, uint32_t idx)

  Summary:
    Attempts to remove an item from the list.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t idx - the index to remove from the list

  Returns:
    laResult - the operation result

  Remarks:
    The memory owned by the string item will be freed automatically.
*/
LIB_EXPORT laResult laListWheelWidget_RemoveItem(laListWheelWidget* whl, uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_RemoveAllItems(laListWheelWidget* whl)

  Summary:
    Attempts to remove all items from the list.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    laResult - the operation result

  Remarks:
    All memory owned by each item string will be freed automatically.
*/
LIB_EXPORT laResult laListWheelWidget_RemoveAllItems(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    int32_t laListWheelWidget_GetSelectedItem(laListWheelWidget* whl)

  Summary:
    Returns the index of the currently selected item.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    int32_t - the index of the selected item or -1 if an error occured

  Remarks:

*/
LIB_EXPORT int32_t laListWheelWidget_GetSelectedItem(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetSelectedItem(laListWheelWidget* whl,
                                               uint32_t idx)

  Summary:
    Attempts to set the selectedi tem index

  Description:


  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t idx - the desired selected item index

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetSelectedItem(laListWheelWidget* whl,
                                                      uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SelectPreviousItem(laListWheelWidget* whl)

  Summary:
    Attempts to move the selected item index to the previous item in the list.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SelectPreviousItem(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SelectNextItem(laListWheelWidget* whl)

  Summary:
    Attempts to move the selected item index to the next item in the list.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SelectNextItem(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_GetItemText(laListWheelWidget* whl,
                                           uint32_t idx,
                                           laString* str)

  Summary:
    Gets the text value for an item in the list.

  Description:
    This function allocates memory and initializes the input string pointer.  The
    caller is responsible for managing the memory once this function returns.

  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t idx - the index to consider
    laString* str - a pointer to an laString object

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_GetItemText(laListWheelWidget* whl,
                                                  uint32_t idx,
                                                  laString* str);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetItemText(laListWheelWidget* whl,
                                           uint32_t index,
                                           laString str)

  Summary:
    Sets the text value for an item in the list.

  Description:
    This function copies the contents of the input string into its internal
    string buffer.  The input string can then be freed or altered without
    affecting the label's internal string value.

  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t idx - the index to consider
    laString str - an laString object

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetItemText(laListWheelWidget* whl,
                                                  uint32_t index,
                                                  laString str);


// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laListWheelWidget_GetItemIcon(laListWheelWidget* whl,
                                                   uint32_t index)

  Summary:
    Gets the pointer to the image asset for the icon for the item at the given
    index.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t idx - the index to consider

  Returns:
    GFXU_ImageAsset* - the image asset pointer or NULL

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laListWheelWidget_GetItemIcon(laListWheelWidget* whl,
                                                          uint32_t index);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetItemIcon(laListWheelWidget* whl,
                                           uint32_t index,
                                           GFXU_ImageAsset* img)

  Summary:
    Sets the icon pointer for a given index.

  Description:


  Parameters:
    laListWheelWidget* whl - the widget
    uint32_t idx - the index to configure
    GFXU_ImageAsset* - the image asset pointer to use as the icon

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetItemIcon(laListWheelWidget* whl,
                                                  uint32_t index,
                                                  GFXU_ImageAsset* img);



// *****************************************************************************
/* Function:
    laListWheelWidget_SelectedItemChangedEvent laListWheelWidget_GetSelectedItemChangedEventCallback(laListWheelWidget* whl)

  Summary:
    Gets the callback for the item selected changed event

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    laListWheelWidget_SelectedItemChangedEvent - the current pointer to the callback or NULL

  Remarks:

*/
LIB_EXPORT laListWheelWidget_SelectedItemChangedEvent laListWheelWidget_GetSelectedItemChangedEventCallback(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetSelectedItemChangedEventCallback(laListWheelWidget* whl,
                                                                   laListWheelWidget_SelectedItemChangedEvent cb)

  Summary:


  Description:
    This callback is called whenver the wheel's selected item changes.

  Parameters:
    laListWheelWidget* whl - the widget
    laListWheelWidget_SelectedItemChangedEvent - the desired pointer to callback or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetSelectedItemChangedEventCallback(laListWheelWidget* whl,
                                                                          laListWheelWidget_SelectedItemChangedEvent cb);

// *****************************************************************************
/* Function:
    laListWheelIndicatorFill laListWheelWidget_GetIndicatorFill(laListWheelWidget* whl)

  Summary:
    Gets the indicator area fill type

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    laListWheelIndicatorFill - the indicator area fill type

  Remarks:

*/
LIB_EXPORT laListWheelIndicatorFill laListWheelWidget_GetIndicatorFill(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetIndicatorFill(laListWheelWidget* whl, laListWheelIndicatorFill fill)

  Summary:
    Sets the indicator fill type

  Description:
    Sets the indicator fill type

  Parameters:
    laListWheelWidget* whl - the widget
    laListWheelIndicatorFill fill - fill type

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetIndicatorFill(laListWheelWidget* whl, laListWheelIndicatorFill fill);

// *****************************************************************************
/* Function:
    laListWheelZoomEffects laListWheelWidget_GetZoomEffects(laListWheelWidget* whl)

  Summary:
    Gets the list wheel zoom effect

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    laListWheelZoomEffects - the list wheel zoom effect type

  Remarks:

*/
LIB_EXPORT laListWheelZoomEffects laListWheelWidget_GetZoomEffects(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetZoomEffects(laListWheelWidget* whl, laListWheelZoomEffects zoomEffects)

  Summary:
    Sets the list wheel zoom effect

  Description:
    Sets the list wheel zoom effect

  Parameters:
    laListWheelWidget* whl - the widget
    laListWheelZoomEffects zoomEffects - the zoom effect

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetZoomEffects(laListWheelWidget* whl, laListWheelZoomEffects zoomEffects);

// *****************************************************************************
/* Function:
    laListWheelIndicatorFill laListWheelWidget_GetAutoHideWheel(laListWheelWidget* whl)

  Summary:
    Returns the list wheel auto hide setting

  Description:


  Parameters:
    laListWheelWidget* whl - the widget

  Returns:
    laBool - true if the list wheel is set to auto hide

  Remarks:

*/
LIB_EXPORT laBool laListWheelWidget_GetAutoHideWheel(laListWheelWidget* whl);

// *****************************************************************************
/* Function:
    laResult laListWheelWidget_SetAutoHideWheel(laListWheelWidget* whl, laBool autoHide)

  Summary:
    Sets the list wheel to auto hide when not active

  Description:
    Sets the list wheel to auto hide when not active

  Parameters:
    laListWheelWidget* whl - the widget
    laBool autoHide - sets the list wheel to auto hide

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWheelWidget_SetAutoHideWheel(laListWheelWidget* whl, laBool autoHide);

// DOM-IGNORE-BEGIN
// internal use only
void _laListWheelWidget_GetItemTextRect(laListWheelWidget* whl,
                                        uint32_t idx,
                                        uint32_t pos,
                                        GFX_Rect* textRect,
                                        GFX_Rect* drawRect);

// internal use only
void _laListWheelWidget_GetItemIconRect(laListWheelWidget* whl,
                                        uint32_t idx,
                                        uint32_t pos,
                                        GFX_Rect* imgRect,
                                        GFX_Rect* imgSrcRect);
// DOM-IGNORE-END

#endif // LA_LISTWHEEL_WIDGET_ENABLED
#endif /* LIBARIA_LISTWHEELWIDGET_H */
