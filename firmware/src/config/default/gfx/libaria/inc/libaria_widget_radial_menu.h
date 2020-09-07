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
    libaria_widget_radial_menu.h

  Summary:
    

  Description:
    This module implements radial menu widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_RADIALMENU_H
#define LIBARIA_RADIALMENU_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_RADIAL_MENU_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_widget_image.h"
#include "gfx/libaria/inc/libaria_list.h"
#include "gfx/libaria/inc/libaria_string.h"

typedef struct laRadialMenuWidget_t laRadialMenuWidget;

typedef void (*laRadialMenuWidget_ItemSelectedEvent)(laRadialMenuWidget*, laWidget*, int32_t);
typedef void (*laRadialMenuWidget_ItemProminenceChangedEvent)(laRadialMenuWidget*, laWidget*, int32_t);

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

typedef enum laRadialMenuWidgetState_t
{
    LA_RADIAL_MENU_INIT,
	LA_RADIAL_MENU_INPUT_READY,
	LA_RADIAL_MENU_HANDLE_USER_MOVE_REQUEST,
    LA_RADIAL_MENU_RESET_TO_INPUT_POS            
} laRadialMenuWidgetState;

typedef enum laRadialMenuWidgetScaleType_t
{
	LA_RADIAL_MENU_SCALE_OFF,
	LA_RADIAL_MENU_SCALE_GRADUAL,
	LA_RADIAL_MENU_SCALE_PROMINENT
} laRadialMenuWidgetScaleType;

typedef enum laRadialMenuEllipseType_t
{
	LA_RADIAL_MENU_ELLIPSE_TYPE_DEFAULT,
	LA_RADIAL_MENU_ELLIPSE_TYPE_ORBITAL,
	LA_RADIAL_MENU_ELLIPSE_TYPE_ROLLODEX
} laRadialMenuEllipseType;

typedef struct laRadialMenuItemNode_t
{
	laWidget* widget; // point to the widget of the item
	int32_t t; // the angle in degress between 0 - 360, representing the relative position of the item on the track
    
	laWidget_TouchDownEvent_FnPtr origTouchDown; //the widget item's original touch down event, 
                                                 //allows the radial menu to work as a hub to route to the appropriate widget
	laWidget_TouchUpEvent_FnPtr origTouchUp; //the widget item's original touch up event
                                                 //allows the radial menu to work as a hub to route to the appropriate widget
	laWidget_TouchMovedEvent_FnPtr origTouchMoved; //the widget item's original touch move event

    GFX_Rect origSize;  //the original size of the widget, it is a reference point for scaling
    uint32_t origAlphaAmount; //the original alpha value of the widget, it is a reference point for scaling
    
} laRadialMenuItemNode;

// *****************************************************************************
/* Enumeration:
    laRadialMenuWidget_t

  Summary:
    Implementation of a radial menu widget struct

  Description:
    A radial menu is a master widget that manages the movement, in an elliptical 
	track, of a list of widgets.  It also manages the draw order and scaling of 
	each widget item.

	It is essentially a group of widgets which provides a mutually exclusive 
	selection capability so that only one item may be selected at any one time.

  Remarks:
    None.
*/
typedef struct laRadialMenuWidget_t
{
    laWidget widget; // widget base class

	laRadialMenuWidgetState state;

    int32_t prominentIndex;
    int32_t lastProminentIndex;
	int32_t userRequestedAngleDiff; // the angle for the radial menu to rotate as requested by user
	int32_t targetAngleDiff; // the angle for the radial menu to rotate for prominent item to be in front
    int32_t userRequestedDirection; // tracks the direction that the user requested for rotation
    
    laBool drawEllipse; // indicates if the radial menu is selected
    laBool highlightProminent; // highlight the prominent widget

    laHAlignment halign; // horizontal alignment
    laVAlignment valign; // vertical alignment

	laImageWidget* highlighter; // this widget manages the selector art asset
    
	int32_t a;  // the half-length of the 0-180 axis of the ellipse
	int32_t b;  // the half-length of the 90-270 axis of the ellipse
	int32_t theta; // the angle of rotation of the entire ellipse
	
    laBool touchPressed; // keep track of users touch input
    
    laBool ellipseChanged; // keeps track if the elliptical track has changed
    
	laRadialMenuWidgetScaleType scaleItemSize;  // the enable item size scaling within the widget

	int32_t maxSizePercent;  // the maximum size scale between 1 - 200%
	int32_t minSizePercent;  // the minimum size scale between 1 - 200%

	GFX_Rect touchArea; // the area specified within the widget that touch events are detected

	laRadialMenuWidgetScaleType scaleItemAlpha; // the enable item alpha scaling within the widget

	int32_t maxAlphaAmount;  // the maximum alpha between 0 - 255
	int32_t minAlphaAmount;  // the minimum alpha between 0 - 255

    int32_t itemsShown; // keeps count of how many items to visible, this number should be less than or equal to total number of widget items
    laList widgetList; // this is the list of widgets
    laList shownList; // this is the partial list of widgets shown
    laList hiddenList; // this is the partial list of widgets hidden

    laRadialMenuItemNode* widestWidgetItem; // keeps track of which widget is the widgets for major axis calculation
    laRadialMenuItemNode* tallestWidgetItem; // keeps track of which widget is the widgets for minor axis calculation
    
    laRadialMenuWidget_ItemSelectedEvent itemSelectedEvent; // an item is selected event callback
	laRadialMenuWidget_ItemProminenceChangedEvent itemProminenceChangedEvent; // whenever a new item is in prominence event callback

    laRadialMenuEllipseType ellipseType;
    
    GFXU_ExternalAssetReader* reader; // asset reader

} laRadialMenuWidget;


// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laRadialMenuWidget* laRadialMenuWidget_New()

  Summary:
    Allocates memory for a new widget of this type.  The application is
    responsible for the management of this memory until the widget is added to
    a widget tree.

  Description:
    

  Parameters:
    
  Returns:
    laRadialMenuWidget*
    
  Remarks:
    
*/
LIB_EXPORT laRadialMenuWidget* laRadialMenuWidget_New();

// *****************************************************************************
/* Function:
    laBool laRadialMenuWidget_IsProminent(laRadialMenuWidget* mn, laWidget* widget)

  Summary:
    Returns true if this radial menu item is currently in the primary selectable position

  Description:
    

  Parameters:
    laRadialMenuWidget* mn - the radial menu widget
	laWidget* mn - the item widget to inspect

  Returns:
    laBool - true if this widget is currently prominent
    
  Remarks:
    
*/
LIB_EXPORT laBool laRadialMenuWidget_IsProminent(laRadialMenuWidget* mn, laWidget* widget);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetProminent(laRadialMenuWidget* mn, laWidget* widget)

  Summary:
    Sets this widget as prominent.

  Description:
    If this widget belongs to a radial menu then this function will
    cycle the radial menu to show the widget in the primary selectable position.

  Parameters:
  laRadialMenuWidget* mn - the radial menu widget
  laWidget* widget - the item widget to show prominence

  Returns:
    laResult - the operation result
    
  Remarks:
    
*/
LIB_EXPORT laResult laRadialMenuWidget_SetProminent(laRadialMenuWidget* mn, laWidget* widget);

// *****************************************************************************
/* Function:
    int32_t laRadialMenuWidget_GetProminentIndex(laRadialMenuWidget* mn)

  Summary:
    Gets the index of the widget within the radial menu that is prominent

  Description:
    Returns the index of the widget that is in the primary selectable position.

  Parameters:
    laRadialMenuWidget* mn - the radial menu widget

  Returns:
    int32_t - the index of the widget, returns -1 if there is a failure
    
  Remarks:
    
*/
LIB_EXPORT int32_t laRadialMenuWidget_GetProminentIndex(laRadialMenuWidget* mn);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetNumberOfItemsShown(laRadialMenuWidget* mn, int32_t num)

  Summary:
    Sets the number of items visible on the menu

  Description:
    Sets the number of items visible on the menu, this number can be less than or equal to
    the total number of items

  Parameters:
    laRadialMenuWidget* mn - the radial menu widget
    int32_t num - the number of widgets shown

  Returns:
    laResult - the operation result
    
  Remarks:
    
*/
LIB_EXPORT laResult laRadialMenuWidget_SetNumberOfItemsShown(laRadialMenuWidget* mn, int32_t num);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetProminentIndex(laRadialMenuWidget* mn, int32_t index)

  Summary:
    Sets a widget with index within the radial menu as prominent

  Description:
    If the index supplied is within the range of widgets within te radial menu then 
    this function will cycle the radial menu to show the widget 
    in the primary selectable position.

  Parameters:
    laRadialMenuWidget* mn - the radial menu widget
    int32_t index - the item widget to show prominence

  Returns:
    laResult - the operation result
    
  Remarks:
    
*/
LIB_EXPORT laResult laRadialMenuWidget_SetProminentIndex(laRadialMenuWidget* mn, uint32_t index);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetHighlightProminent(laRadialMenuWidget* mn, laBool enable)

  Summary:
    Sets the item widget to highlight when it is at the prominent location

  Description:
    This tells the radial menu widget to enable/disable the ability for the prominent item widget to be
    highlighted when it comes to rest at the prominent location.
 
  Parameters:
    laRadialMenuWidget* mn - the radial menu widget
    laBool enable - the item widget to highlight when prominent

  Returns:
    laResult - the operation result
    
  Remarks:
    
*/
LIB_EXPORT laResult laRadialMenuWidget_SetHighlightProminent(laRadialMenuWidget* mn, laBool enable);

// *****************************************************************************
/* Function:
    int32_t laRadialMenuWidget_GetTheta(laRadialMenuWidget* mn)

  Summary:
    Gets the theta value for the radial menu.

  Description:
    This function returns the current value of theta

  Parameters:
    laRadialMenuWidget* mn - the widget
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/
LIB_EXPORT int32_t laRadialMenuWidget_GetTheta(laRadialMenuWidget* mn);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetTheta(laRadialMenuWidget* mn,
                                         int32_t theta)

  Summary:
    Sets the theta value for the radial menu.

  Description:
    This function sets the theta (angle of rotation of the ellipse)

  Parameters:
    laRadialMenuWidget* mn - the widget
    int32_t theta - angle in degrees of rotation relative to the y-axis of the ellipse
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/                                             
LIB_EXPORT laResult laRadialMenuWidget_SetTheta(laRadialMenuWidget* mn,
                                                int32_t theta);


// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetEllipseType(laRadialMenuWidget* mn,
                                         laRadialMenuEllipseType type)

  Summary:
    Sets the ellipse type for the radial menu track.

  Description:
    This function sets the ellipse type for the radial menu.  There are three types:
    DEFAULT - the elliptical track is best-fit based on the width and height of the 
              radial menu widget and all the item widgets with scaling factored in.
              The theta angle value is used for the ellipse but only in a limited way.
    ORBITAL - the elliptical track is flatter and factors in the theta angle value more.
              Some of the items may be clipped out of the widget area depending on the 
              size of radial widget and the theta angle
    ROLLODEX - the elliptical track is vertical in-nature and visually simular to a rollodex

  Parameters:
    laRadialMenuWidget* mn - the widget
    laRadialMenuEllipseType type - type of ellipse
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/                                             
LIB_EXPORT laResult laRadialMenuWidget_SetEllipseType(laRadialMenuWidget* mn,
                                                laRadialMenuEllipseType type);


// *****************************************************************************
/* Function:
    laWidget* laRadialMenuWidget_GetWidget(laRadialMenuWidget* mn, int32_t index)

  Summary:
    Gets the pointer for the widget by index

  Description:
    Returns the pointer to the widget

  Parameters:
    laRadialMenuWidget* mn - the radial menu widget
	int32_t index - index value

  Returns:
    laWidget* - the pointer to the widget at the index value on the list
	Returns NULL if the index is out-of-range of the current list
    
  Remarks:
    
*/    
LIB_EXPORT laWidget* laRadialMenuWidget_GetWidget(laRadialMenuWidget* mn, int32_t index);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetWidgetAt(laRadialMenuWidget* mn, 
													laWidget* widget, int32_t index)

  Summary:
    Insert a widget to the radial menu at the index specified

  Description:
    

  Parameters:
    laRadialMenuWidget* mn - the widget
	laWidget* widget - the item widget 
	int32_t index - index value
    
  Returns:
    laResult - the operation result, returns LA_SUCCESS if the set was successful
				returns fail if the index value is out of the range of the widget list

    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadialMenuWidget_SetWidgetAt(laRadialMenuWidget* mn, laWidget* widget, int32_t index);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_AddWidget(laRadialMenuWidget* mn, laWidget* widget)

  Summary:
    Add a widget to the radial menu

  Description:
    Adds a widget for the radial menu to manage, increments the total item count

  Parameters:
    laRadialMenuWidget* mn - the radial menu widget
	laWidget* widget - the item widget

  Returns:
	laResult - the operation result

  Remarks:
    
*/    
LIB_EXPORT laResult laRadialMenuWidget_AddWidget(laRadialMenuWidget* mn, laWidget* widget);

// *****************************************************************************
/* Function:
	laResult laRadialMenuWidget_RemoveWidget(laRadialMenuWidget* mn, laWidget* widget)

   Summary:
	Removes a widget to the radial menu

   Description:
	Removes a widget for the radial menu to manage, decrements the total item count

   Parameters:
	laRadialMenuWidget* mn - the radial menu widget
	laWidget* widget - the item widget

   Returns:
	laResult - the operation result, returns LA_FAIL if a match is not found

   Remarks:

*/
LIB_EXPORT laResult laRadialMenuWidget_RemoveWidget(laRadialMenuWidget* mn, laWidget* widget);

// *****************************************************************************
/* Function:
	laResult laRadialMenuWidget_ClearItems(laRadialMenuWidget* mn)

  Summary:
    Clears all items in the radial menu widget

  Description:
    Removes all items

  Parameters:
    laRadialMenuWidget* mn - the widget
        
  Returns:
    laResult - the operation result

  Remarks:
    
*/                                                           
LIB_EXPORT laResult laRadialMenuWidget_ClearItems(laRadialMenuWidget* mn);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetSizeScaling(laRadialMenuWidget* mn,
                                                laRadialMenuWidgetScaleType setting)

  Summary:
    Enables per item size scaling for the radial menu

  Description:
    Enable/Disable the ability for the radial menu to scale the items sizes as they
	travel into the background

  Parameters:
    laRadialMenuWidget* mn - the widget
	laRadialMenuWidgetScaleType setting - setting flag
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadialMenuWidget_SetSizeScaling(laRadialMenuWidget* mn,
														laRadialMenuWidgetScaleType setting);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetAlphaScaling(laRadialMenuWidget* mn,
														laRadialMenuWidgetScaleType setting)

  Summary:
    Enables per item alpha scaling for the radial menu

  Description:
    Enable/Disable the ability for the radial menu to scale the items alpha as they
	travel into the background

  Parameters:
    laRadialMenuWidget* mn - the widget
	laRadialMenuWidgetScaleType setting - setting flag

  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadialMenuWidget_SetAlphaScaling(laRadialMenuWidget* mn,
														laRadialMenuWidgetScaleType setting);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetSizeScaleMinMax(laRadialMenuWidget* mn, 
														int32_t min, int32_t max)

  Summary:
    Sets the minimum and maximum size scaling ratio in percent

  Description:
    Sets the minimum and maximum size scaling ratio for the items. These values
	are only used if laRadialMenuWidget_SetSizeScaling is called and 
	size scaling is enabled 

  Parameters:
    laRadialMenuWidget* mn - the widget
	int32_t min - the minimum size scale percentage, it should be between 1 - 200
	int32_t max - the maximum size scale percentage, it should be between 1 - 200

  Returns:
    laResult - the operation result

  Remarks:
    
*/    
LIB_EXPORT laResult laRadialMenuWidget_SetSizeScaleMinMax(laRadialMenuWidget* mn,
															int32_t min, int32_t max);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetAlphaScaleMinMax(laRadialMenuWidget* mn, 
														int32_t min, int32_t max)

  Summary:
    Sets the minimum and maximum alpha scaling ratio

  Description:
    Sets the minimum and maximum alpha scaling ratio for the items. These values
	are only used if laRadialMenuWidget_SetAlphaScaling is called and 
	alpha scaling is enabled 

  Parameters:
    laRadialMenuWidget* mn - the widget
	int32_t min - the minimum alpha scale, it should be between 0 - 255
	int32_t max - the maximum alpha scale, it should be between 0 - 255

  Returns:
    laResult - the operation result

  Remarks:
    
*/    
LIB_EXPORT laResult laRadialMenuWidget_SetAlphaScaleMinMax(laRadialMenuWidget* mn,
															int32_t min, int32_t max);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetTouchArea(laRadialMenuWidget* mn, 
                                int32_t x, int32_t y, int32_t width, int32_t height)

  Summary:
    Sets the area that touch input is allowed within the radial menu widget

  Description:
    Sets the area that touch input is permitted. This area has to be at or smaller
	than the rectangular area of the entire radial menu widget.  The default is
	the bottom half of the widget.

  Parameters:
    laRadialMenuWidget* mn - the widget
	GFX_Rect rect - rectangular area, x-y represents offsets in local space

  Returns:
    laResult - the operation result

  Remarks:
    
*/    
LIB_EXPORT laResult laRadialMenuWidget_SetTouchArea(laRadialMenuWidget* mn,
                                int32_t x, int32_t y, int32_t width, int32_t height);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetDrawEllipse(laRadialMenuWidget* mn, laBool enable)

  Summary:
    Enables drawing the elliptical track for the radial menu

  Description:
    Enable/Disable the drawing of the elliptical track of travel of the items in the 
 *  radial menu

  Parameters:
    laRadialMenuWidget* mn - the widget
	laRadialMenuWidgetScaleType setting - setting flag

  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadialMenuWidget_SetDrawEllipse(laRadialMenuWidget* mn, laBool enable);

// *****************************************************************************
/* Function:
    laRadialMenuWidget_ItemSelectedEvent laRadialMenuWidget_GetItemSelectedEventCallback(laRadialMenuWidget* mn)

  Summary:
    Gets the current radial menu item selected event callback

  Description:
    

  Parameters:
    laRadialMenuWidget* mn - the widget
        
  Returns:
    laRadialMenuWidget_ItemSelectedEvent - a valid callback pointer or NULL
    
  Remarks:
    
*/    
LIB_EXPORT laRadialMenuWidget_ItemSelectedEvent laRadialMenuWidget_GetItemSelectedEventCallback(laRadialMenuWidget* mn);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetItemSelectedEventCallback(laRadialMenuWidget* mn,
                                                          laRadialMenuWidget_ItemSelectedEvent cb)

  Summary:
    Sets the radial menu item selected event callback

  Description:
    This callback is called when an item widget in the radial menu becomes selected

  Parameters:
    laRadialMenuWidget* mn - the widget
    laRadialMenuWidget_ItemSelectedEvent - a valid callback pointer or NULL
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadialMenuWidget_SetItemSelectedEventCallback(laRadialMenuWidget* mn,
													laRadialMenuWidget_ItemSelectedEvent cb);

// *****************************************************************************
/* Function:
    laRadialMenuWidget_ItemProminenceChangedEvent laRadialMenuWidget_GetItemProminenceChangeddEventCallback(laRadialMenuWidget* mn)

  Summary:
    Gets the current radial menu item prominence change event callback

  Description:
    

  Parameters:
    laRadialMenuWidget* mn - the widget
        
  Returns:
    laRadialMenuWidget_ItemProminenceChangedEvent - a valid callback pointer or NULL
    
  Remarks:
    
*/    
LIB_EXPORT laRadialMenuWidget_ItemProminenceChangedEvent laRadialMenuWidget_GetItemProminenceChangeddEventCallback(laRadialMenuWidget* mn);

// *****************************************************************************
/* Function:
    laResult laRadialMenuWidget_SetItemProminenceChangedEventCallback(laRadialMenuWidget* mn,
                                                            laRadialMenuWidget_ItemProminenceChangedEvent cb)

  Summary:
    Sets the deselected callback pointer

  Description:
    This callback is called when this radial menu is deselected

  Parameters:
    laRadialMenuWidget* mn - the widget
    laRadialMenuWidget_DeselectedEvent - a valid callback pointer or NULL
    
  Returns:
    laResult - the operation result
    
  Remarks:
    
*/    
LIB_EXPORT laResult laRadialMenuWidget_SetItemProminenceChangedEventCallback(laRadialMenuWidget* mn,
												laRadialMenuWidget_ItemProminenceChangedEvent cb);

// DOM-IGNORE-BEGIN
// internal use only
laWidgetUpdateState _laRadialMenuWidget_Update(laRadialMenuWidget* mn);
// DOM-IGNORE-END

#endif // LA_RADIALMENU_WIDGET_ENABLED
#endif /* LIBARIA_RADIALMENU_H */