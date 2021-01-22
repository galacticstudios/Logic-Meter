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
    libaria_widget.h

  Summary:


  Description:
    This module implements top level widget control functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_WIDGET_H
#define LIBARIA_WIDGET_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"
#include "gfx/libaria/inc/libaria_input.h"
#include "gfx/libaria/inc/libaria_array.h"
#include "gfx/libaria/inc/libaria_scheme.h"

#include "gfx/hal/inc/gfx_rect.h"
#include "gfx/utils/gfx_utils.h"

// DOM-IGNORE-BEGIN
#define DEFAULT_BORDER_MARGIN   4
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

/*
Enumeration:
        laWidgetType_t

    Summary:
        Specifies the different widget types used in the library

    Description:
        This enumeration specifies the different widget types
        used in the library.

    Remarks:
        None.
 */
typedef enum laWidgetType_t
{
    LA_WIDGET_WIDGET,
    LA_WIDGET_LAYER,
#if LA_ARC_WIDGET_ENABLED
    LA_WIDGET_ARC,
#endif
#if LA_BAR_GRAPH_WIDGET_ENABLED
    LA_WIDGET_BAR_GRAPH,
#endif
#if LA_BUTTON_WIDGET_ENABLED
    LA_WIDGET_BUTTON,
#endif
#if LA_BUTTON_WIDGET_ENABLED
    LA_WIDGET_CHECKBOX,
#endif
#if LA_CIRCLE_WIDGET_ENABLED
    LA_WIDGET_CIRCLE,
#endif
#if LA_CIRCULAR_GAUGE_WIDGET_ENABLED
    LA_WIDGET_CIRCULAR_GAUGE,
#endif
#if LA_CIRCULAR_SLIDER_WIDGET_ENABLED
    LA_WIDGET_CIRCULAR_SLIDER,
#endif
#if LA_DRAWSURFACE_WIDGET_ENABLED
    LA_WIDGET_DRAWSURFACE,
#endif
#if LA_IMAGE_WIDGET_ENABLED
    LA_WIDGET_IMAGE,
#endif
#if LA_IMAGEPLUS_WIDGET_ENABLED
    LA_WIDGET_IMAGEPLUS,
#endif
#if LA_IMAGESEQUENCE_WIDGET_ENABLED
    LA_WIDGET_IMAGESEQUENCE,
#endif
#if LA_GRADIENT_WIDGET_ENABLED
    LA_WIDGET_GRADIENT,
#endif
#if LA_GROUPBOX_WIDGET_ENABLED
    LA_WIDGET_GROUPBOX,
#endif
#if LA_KEYPAD_WIDGET_ENABLED && LA_BUTTON_WIDGET_ENABLED
    LA_WIDGET_KEYPAD,
#endif
#if LA_LABEL_WIDGET_ENABLED
    LA_WIDGET_LABEL,
#endif
#if LA_LINE_WIDGET_ENABLED
    LA_WIDGET_LINE,
#endif
#if LA_LINE_GRAPH_WIDGET_ENABLED
    LA_WIDGET_LINE_GRAPH,
#endif
#if LA_LIST_WIDGET_ENABLED && LA_SCROLLBAR_WIDGET_ENABLED
    LA_WIDGET_LIST,
#endif
#if LA_LISTWHEEL_WIDGET_ENABLED
    LA_WIDGET_LISTWHEEL,
#endif
#if LA_PIE_CHART_WIDGET_ENABLED
    LA_WIDGET_PIE_CHART,
#endif
#if LA_PROGRESSBAR_WIDGET_ENABLED
    LA_WIDGET_PROGRESSBAR,
#endif
#if LA_RADIAL_MENU_WIDGET_ENABLED
    LA_WIDGET_RADIAL_MENU,
#endif
#if LA_RADIOBUTTON_WIDGET_ENABLED
    LA_WIDGET_RADIOBUTTON,
#endif
#if LA_RECTANGLE_WIDGET_ENABLED
    LA_WIDGET_RECTANGLE,
#endif
#if LA_SCROLLBAR_WIDGET_ENABLED
    LA_WIDGET_SCROLLBAR,
#endif
#if LA_SLIDER_WIDGET_ENABLED
    LA_WIDGET_SLIDER,
#endif
#if LA_TEXTFIELD_WIDGET_ENABLED
    LA_WIDGET_TEXTFIELD,
#endif
#if LA_TOUCHTEST_WIDGET_ENABLED
    LA_WIDGET_TOUCHTEST,
#endif
#if LA_WINDOW_WIDGET_ENABLED
    LA_WIDGET_WINDOW,
#endif
    // begin custom types, not essential but good for identifying base pointers

    // end custom types
    //LA_WIDGET_LAST = LA_WIDGET_WINDOW, // change this if adding custom
    //LA_WIDGET_TYPE_COUNT = LA_WIDGET_LAST + 1
} laWidgetType;

/*
Enumeration:
        laBackgroundType_t

    Summary:
        Specifies the different background types used for the widgets in the library

    Description:
        Specifies the different background types used for the widgets in the library

        None - No background fill.  Widget must defer to its parent to erase dirty
               pixels.  This may cause additional overhead as clean pixels may be
               repainted as well.

        Fill - a scheme color is used to fill the widget rectangle.

        Cache - a local framebuffer cache is maintained by the widget and used to
                clean up dirty pixels.  Will not cause a parent repaint event but
                will use additional memory to contain the cache.

    Remarks:
        None.
 */
typedef enum laBackgroundType_t
{
    LA_WIDGET_BACKGROUND_NONE,
    LA_WIDGET_BACKGROUND_FILL,
    LA_WIDGET_BACKGROUND_CACHE,
    LA_WIDGET_BACKGROUND_LAST = LA_WIDGET_BACKGROUND_CACHE
} laBackgroundType;

/*
Enumeration:
        laBorderType_t

    Summary:
        Specifies the different border types used for the widgets in the library

    Description:
        Specifies the different border types used for the widgets in the library

    Remarks:
        None.
 */
typedef enum laBorderType_t
{
    LA_WIDGET_BORDER_NONE,
    LA_WIDGET_BORDER_LINE,
    LA_WIDGET_BORDER_BEVEL,
    LA_WIDGET_BORDER_LAST = LA_WIDGET_BORDER_BEVEL
} laBorderType;

/*
Enumeration:
        laWidgetDirtyState_t

    Summary:
        Specifies the different dirty states the widget can be assigned

    Description:
        Specifies the different dirty states the widget can be assigned
        This decides whether the particular widget would be re-drawn or not.
        Dirty widget are re-drawn and clean are not painted over.

    Remarks:
        None.
 */
typedef enum laWidgetDirtyState_t
{
    LA_WIDGET_DIRTY_STATE_CLEAN,
    LA_WIDGET_DIRTY_STATE_CHILD,
    LA_WIDGET_DIRTY_STATE_DIRTY,
} laWidgetDirtyState;

/*
Enumeration:
        laWidgetDrawState_t

    Summary:
        Specifies the different draw states the widget can be assigned

    Description:
        Specifies the different draw states the widget can be assigned

    Remarks:
        None.
 */
typedef enum laWidgetDrawState_t
{
    LA_WIDGET_DRAW_STATE_READY,
    LA_WIDGET_DRAW_STATE_DONE,

    // widget-specific draw states start here
} laWidgetDrawState;


/*
Enumeration:
        laWidgetUpdateState_t

    Summary:
        Specifies the different update states the widget can be assigned

    Description:
        Specifies the different update states the widget can be assigned

    Remarks:
        None.
 */
typedef enum laWidgetUpdateState_t
{
    LA_WIDGET_UPDATE_STATE_DONE,
    LA_WIDGET_UPDATE_STATE_PENDING,

    // widget-specific update states start here
} laWidgetUpdateState;

/*
Enumeration:
        laWidgetOptimizationFlags_t

    Summary:
        Specifies the different draw optimization flags for a widget

    Description:
        Specifies the different draw optimization flags for a widget

    Remarks:
        None.
 */
typedef enum laWidgetOptimizationFlags_t
{
    /*
     * local redraw
     * If a widget has no background then normally the parent would need to
     * redraw to erase the contents of the widget.  This flag indicates to
     * the renderer to not redraw the parent event if the widget has no
     * background
     */
    LA_WIDGET_OPT_LOCAL_REDRAW = 0x1,

    /*
     * draw once
     * Indicates that a widget should draw once per screen show event
     * all other attempts to invalidate or paint a widget will be rejected
     */
    LA_WIDGET_OPT_DRAW_ONCE    = 0x2,

    /*
     * opaque
     * Indicates that a widget is fully opaque regardless of its background
     * setting.  This is often used for cases like image widgets where
     * the image fills the entire widget space but you don't want the
     * overhead of drawing a background behind it as well.  This flag
     * helps widgets without backgrounds to pass occlusion tests.
     */
    LA_WIDGET_OPT_OPAQUE    = 0x4,

} laWidgetOptimizationFlags;


typedef struct laWidget_t laWidget;
typedef struct laRectArray_t laRectArray;

// DOM-IGNORE-BEGIN
typedef void (*laWidget_Constructor_FnPtr)(laWidget*);
typedef void (*laWidget_Destructor_FnPtr)(laWidget*);
typedef laWidgetUpdateState (*laWidget_Update_FnPtr)(laWidget*, uint32_t);
typedef void (*laWidget_Paint_FnPtr)(laWidget*);
typedef void (*laWidget_DrawFunction_FnPtr)(void*);

typedef void (*laWidget_Moved_FnPtr)(laWidget*);
typedef void (*laWidget_Resized_FnPtr)(laWidget*);

typedef void (*laWidget_Focus_FnPtr)(laWidget*);

typedef void (*laWidget_TouchDownEvent_FnPtr)(laWidget*, laInput_TouchDownEvent*);
typedef void (*laWidget_TouchUpEvent_FnPtr)(laWidget*, laInput_TouchUpEvent*);
typedef void (*laWidget_TouchMovedEvent_FnPtr)(laWidget*, laInput_TouchMovedEvent*);

typedef void (*laWidget_LanguageChangingEvent_FnPtr)(laWidget*);

typedef void (*laWidget_InvalidateBorderAreas_FnPtr)(laWidget*);
// DOM-IGNORE-END

/* Structure:
        laWidget_t

    Summary:
        Specifies Graphics widget structure to manage all properties and events associated with the widget

    Description:
        Specifies Graphics widget structure to manage all properties and events associated with the widget.
        It also contains information about the parent and children for the widget to manage the tree structure that the library supports.


    Remarks:
        None.

*/
typedef struct laWidget_t
{
    uint32_t id;  // the id of the widget
    laWidgetType type; // the type of the widget

    laBool editable; // indicates if this widget implements the editable interface

    laBool visible; // the widget visible flag
    laBool enabled; // the widget enabled flag

    GFX_Rect rect; // the bounding rectangle of the widget
    uint32_t cornerRadius; //corner radius, draws round corners if > 0

    laMargin margin; // the margin settings for the widget

    laBorderType borderType; // the widget border type
    laBackgroundType backgroundType; // the widget background type

    uint32_t optimizationFlags; // optimization flags

    uint32_t drawCount; // number of times this widget has been drawn
                        // for the active screen

    GFX_PixelBuffer* cache; // the local framebuffer cache for the widget
                            // this can be used to avoid costly parent
                            // redraw operations at the cost of using
                            // more memory
    laBool cacheInvalid;    // indicates that the local cache is invalid
                            // and needs to be refilled

    laBool alphaEnabled;    // indicates that the global alpha blending
                            // setting is enabled for this widget
    uint32_t alphaAmount;   // the global alpha amount to apply to this
                            // widget (cumulative with parent widgets)

    uint32_t dirtyState;    // the widget's dirty state
    uint32_t drawState;     // the widget's draw state

    laWidget_DrawFunction_FnPtr drawFunc; // the next draw function to call

    laScheme* scheme; // the widget's color scheme

    laBool root;      // indicates if this widget is a root widget
    laWidget* parent; // pointer to the widget's parent
    laArray children;  // pointers for the widget's children

    // overrideable functions
    laWidget_Destructor_FnPtr destructor; // the widget's destructor

    laWidget_Moved_FnPtr moved; // moved function pointer
    laWidget_Resized_FnPtr resized; // resized function pointer

    laWidget_Focus_FnPtr focusGained; // focus gained function pointer
    laWidget_Focus_FnPtr focusLost; // focus lost function pointer

    laWidget_Update_FnPtr update; // update function pointer
    laWidget_Paint_FnPtr paint; // paint function pointer

    laWidget_TouchDownEvent_FnPtr touchDown; // touch down function pointer
    laWidget_TouchUpEvent_FnPtr touchUp; // touch up function pointer
    laWidget_TouchMovedEvent_FnPtr touchMoved; // touch moved function pointer

    laWidget_LanguageChangingEvent_FnPtr languageChangeEvent; // languaged event pointer

    laWidget_InvalidateBorderAreas_FnPtr invalidateBorderAreas;
} laWidget;

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

/*    Function:
        laWidget* laWidget_New()

    Summary:
        Create a new widget.


    Description:
        Create a new widget, alocate memory for the widget through the current
        active context.  Returns a widget object pointer.  Application is
        responsible for managing the widget pointer until the widget is added
        to a widget tree.

    Parameters:


    Returns:
        lawidget*

*/
LIB_EXPORT laWidget* laWidget_New();

/*    Function:
        void laWidget_Delete(laWidget* wgt)

    Summary:
        Delete the widget object specified

    Description:
        Delete a widget object specified, de-allocate memory for the widget
        through the current active context.  All child widgets are also
        destructed and freed.

    Parameters:
        lawidget*

    Returns:
        void

*/
LIB_EXPORT void laWidget_Delete(laWidget* wgt);

// DOM-IGNORE-BEGIN
// internal constructor and destructor
void _laWidget_Constructor(laWidget* wgt);
void _laWidget_Destructor(laWidget* wgt);

// internal use only
void _laWidget_Paint(laWidget* wgt);
// DOM-IGNORE-END


/*    Function:
        int32_t laWidget_GetX(laWidget* wgt)

    Summary:
        Returns the widget rectangles upper left corner x-coordinate

    Description:
        Returns the widget rectangles upper left corner x-coordinate

    Parameters:
        lawidget* wgt - the widget

    Returns:
        uint32_t

*/
LIB_EXPORT int32_t laWidget_GetX(laWidget* wgt);

/*    Function:
        laResult laWidget_SetX(laWidget* wgt, int32_t x)

    Summary:
        Sets the widget's x coordinate position

    Description:
        Sets the widget's x coordinate position

    Parameters:
        lawidget* wgt - the widget
        int32_t x - the desired x value

    Returns:
        laResult - result of the operation

*/
LIB_EXPORT laResult laWidget_SetX(laWidget* wgt, int32_t x);

/*    Function:
        int32_t laWidget_GetY(laWidget* wgt)

    Summary:
        Returns the widget rectangles upper left corner y-coordinate

    Description:
        Returns the widget rectangles upper left corner y-coordinate

    Parameters:
        lawidget* wgt - the widget

    Returns:
        uint32_t - the y value

*/
LIB_EXPORT int32_t laWidget_GetY(laWidget* wgt);

/*    Function:
        laResult laWidget_SetY(laWidget* wgt, int32_t y)

    Summary:
        Sets the widget's y coordinate position

    Description:
        Sets the widget's y coordinate position

    Parameters:
        lawidget* wgt - the widget
        int32_t y - the desired y value

    Returns:
        laResult - result of the operation

*/
LIB_EXPORT laResult laWidget_SetY(laWidget* wgt, int32_t y);

/*    Function:
        int32_t laWidget_GetWidth(laWidget* wgt)

    Summary:
        Returns the widget rectangles width

    Description:
        Returns the widget rectangles width

    Parameters:
        lawidget* wgt - the widget

    Returns:
        uint32_t - the widget's y coordinate value

*/
LIB_EXPORT int32_t laWidget_GetWidth(laWidget* wgt);

/*    Function:
        laResult laWidget_SetWidth(laWidget* wgt, int32_t width)

    Summary:
        Sets the widget's width value

    Description:
        Sets the widget's width value

    Parameters:
        lawidget* wgt - the widget
        int32_t width - the desired width value, must be > 0

    Returns:
        laResult - result of the operation

*/
LIB_EXPORT laResult laWidget_SetWidth(laWidget* wgt, int32_t width);

/*    Function:
        int32_t laWidget_GetHeight(laWidget* wgt)

    Summary:
        Returns the widget rectangles height

    Description:
        Returns the widget rectangles height

    Parameters:
        lawidget* wgt - the widget

    Returns:
        uint32_t - the widget's width value

*/
LIB_EXPORT int32_t laWidget_GetHeight(laWidget* wgt);

/*    Function:
        laResult laWidget_SetHeight(laWidget* wgt, int32_t height)

    Summary:
        Sets the widget's height value

    Description:
        Sets the widget's height value

    Parameters:
        lawidget* wgt - the widget
        int32_t height - the desired height value, must be > 0

    Returns:
        laResult - result of the operation

*/
LIB_EXPORT laResult laWidget_SetHeight(laWidget* wgt, int32_t height);

// *****************************************************************************
/* Function:
    laBool laWidget_GetAlphaEnable(laWidget* wgt)

  Summary:
    Return the alpha enable property of the widget

  Description:
    Return the alpha enable property of the widget

  Parameters:
    lawidget* wgt - the widget

  Returns:
    laBool - the widget's alpha enable flag value

  Remarks:

*/
LIB_EXPORT laBool laWidget_GetAlphaEnable(laWidget* wgt);

// *****************************************************************************
/* Function:
    laBool laWidget_GetCumulativeAlphaEnable(laWidget* wgt)

  Summary:
    Determines if this or any ancestor widget has alpha enabled

  Description:


  Parameters:
    lawidget* wgt - the widget

  Returns:
    laBool - whether the widget has alpha enabled

  Remarks:

*/
LIB_EXPORT laBool laWidget_GetCumulativeAlphaEnable(laWidget* wgt);

// *****************************************************************************
/* Function:
    laResult laWidget_SetAlphaEnable(laWidget* wgt, laBool enable)

  Summary:
    Set the alpha enable property of the widget with the boolean
    value specified

  Description:
    Set the alpha enable property of the widget with the boolean
    value specified

  Parameters:
    lawidget* wgt - the widget
    laBool enable - the desired alpha enable flag value

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetAlphaEnable(laWidget* wgt, laBool enable);

// *****************************************************************************
/* Function:
    uint32_t laWidget_GetAlphaAmount(laWidget* wgt)

  Summary:
    Return the widget's global alpha amount

  Description:
    Return the widget's global alpha amount

  Parameters:
    lawidget* wgt - the widget


  Returns:
    uint32_t - the widget's global alpha amount

  Remarks:

*/
LIB_EXPORT uint32_t laWidget_GetAlphaAmount(laWidget* wgt);


// *****************************************************************************
/* Function:
    uint32_t laWidget_GetCumulativeAlphaAmount(laWidget* wgt)

  Summary:
    Calculates the cumulative alpha amount for a hierarchy of widgets

  Description:
    Alpha blending amounts are cumulative from parent to child.  If a parent
    is blended at 50% then logically a child should also implicitely be
    blended at 50%. If a child further explictely enables blending at 50% then
    the cumulative amount is 25%.

  Parameters:
    lawidget* wgt - the widget

  Returns:
    uint32_t - the cumulative blending amount

  Remarks:

*/
LIB_EXPORT uint32_t laWidget_GetCumulativeAlphaAmount(laWidget* wgt);

// *****************************************************************************
/* Function:
    laResult laWidget_SetAlphaAmount(laWidget* wgt, uint32_t alpha)

  Summary:
    Set the widget's global alpha amount to the specified alpha amount

  Description:
    Set the widget's global alpha amount to the specified alpha amount.  Widgets
    may enable alpha blending even for color modes that don't support an alpha
    channel.

  Parameters:
    laWidget* wgt - the widget
    uint32_t alpha - the desired global alpha amount

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetAlphaAmount(laWidget* wgt, uint32_t alpha);

// *****************************************************************************
/* Function:
    laBool laWidget_isOpaque(laWidget* wgt)

  Summary:
    Returns true if the widget is considered opaque.

  Description:
    Opacity is determined by a number of factors including: cumulative alpha
    amount, background type, and the opaque optimization flag.

  Parameters:
    laWidget* wgt - the widget

  Returns:
    laBool - true if the widget is fully opaque

  Remarks:

*/
LIB_EXPORT laBool laWidget_isOpaque(laWidget* wgt);

// *****************************************************************************
/* Function:
    GFX_Rect laWidget_RectToParentSpace(laWidget* wgt)

  Summary:
    Returns the rectangle containing the parent of the widget specified

  Description:
    Returns the rectangle containing the parent of the widget specified
    If the widget and the parent are not null, the rectangle defining
    the parent widget with its upper left corner x and y coordinates is returned

  Parameters:
    laWidget* wgt - the widget

  Returns:
    GFX_Rect - the widget rectangle in parent space

  Remarks:

*/
LIB_EXPORT GFX_Rect laWidget_RectToParentSpace(laWidget* wgt);

// *****************************************************************************
/* Function:
    GFX_Rect laWidget_RectToLayerSpace(laWidget* wgt)

  Summary:
    Transforms a widget rectangle from local space to its root layer space.

  Description:

  Parameters:
    laWidget* wgt - the widget

  Returns:
    GFX_Rect - the transformed rectangle

  Remarks:

*/
LIB_EXPORT GFX_Rect laWidget_RectToLayerSpace(laWidget* wgt);

// *****************************************************************************
/* Function:
    GFX_Rect laWidget_RectToScreenSpace(laWidget* wgt)

  Summary:
    Transforms a widget rectangle from local space to screen space coordinates.

  Description:

  Parameters:
    laWidget* wgt - the widget

  Returns:
    GFX_Rect - the transformed rectangle

  Remarks:

*/
LIB_EXPORT GFX_Rect laWidget_RectToScreenSpace(laWidget* wgt);

// *****************************************************************************
/* Function:
    laResult laWidget_AddChild(laWidget* parent, laWidget* child)

  Summary:
    Adds the child to the parent widget specified in the argument

  Description:
    The function checks to see if the child and parent are valid, removes the child from its current parents children list, and assigns the child to the parent widget specified.
    The child is attached at the end of the list of the parent widgets children list.

  Parameters:
    laWidget* parent - the parent widget
    laWidget* child - the child to add

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_AddChild(laWidget* parent, laWidget* child);

// *****************************************************************************
/* Function:
    laResult laWidget_RemoveChild(laWidget* parent, laWidget* child)

  Summary:
    Removes the child from the parent widget specified in the argument

  Description:
    The function checks to see if the child and parent are valid, removes the child from its current parents children list

  Parameters:
    laWidget* parent - the parent widget
    laWidget* child - the child to remove

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_RemoveChild(laWidget* parent, laWidget* child);

// *****************************************************************************
/* Function:
    laResult laWidget_SetParent(laWidget* wgt, laWidget* parent)

  Summary:
    Sets the parent of the child widget to that specified in the argument list

  Description:
    The function checks to see if the child and parent are valid, removes the child from its current parents children list, and assigns the child to the parent widget specified.
    The child is attached at the end of the list of the parent widgets children list.

  Parameters:
    laWidget* wgt - the widget
    laWidget* parent - the desired parent widget

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetParent(laWidget* wgt, laWidget* parent);

// *****************************************************************************
/* Function:
    uint32_t laWidget_GetChildCount(laWidget* parent)

  Summary:
    Returns the size of the children list of the specified parent widget

  Description:
    Returns the size of the children list of the specified parent widget

  Parameters:
    laWidget* wgt - the widget

  Returns:
    uint32_t - the number of children of this widget

  Remarks:

*/
LIB_EXPORT uint32_t laWidget_GetChildCount(laWidget* parent);

// *****************************************************************************
/* Function:
    laWidget* laWidget_GetChildAtIndex(laWidget* parent, uint32_t idx)

  Summary:
    Fetches the child at the specified index from the children list of the
    specified parent widget

  Description:
    Fetches the child at the specified index from the children list of the
    specified parent widget

  Parameters:
    laWidget* wgt - the widget
    uint32_t idx - the desired child index

  Returns:
    laWidget* - a valid child pointer or NULL

  Remarks:

*/
LIB_EXPORT laWidget* laWidget_GetChildAtIndex(laWidget* parent, uint32_t idx);

// *****************************************************************************
/* Function:
    int32_t laWidget_GetIndexOfChild(laWidget* parent, laWidget* child)

  Summary:
    Fetches the index of the child from the children list of the specified
    parent widget

  Description:
    Traverses the children list of the specified parent widget and finds the
    index of the child widget specified.

  Parameters:
    laWidget* parent - the parent widget
    laWidget* child - the child widget

  Returns:
    int32_t - the index of the given child pointer or -1 if not found

  Remarks:

*/
LIB_EXPORT int32_t laWidget_GetIndexOfChild(laWidget* parent, laWidget* child);

/*    Function:
        void laWidget_DeleteAllDescendants(laWidget* wgt)

    Summary:
        Deletes all of the descendants of the given widget.

    Description:
        All descendants of this widget are removed and deleted.

    Parameters:
        lawidget*

    Returns:
        void

*/
LIB_EXPORT void laWidget_DeleteAllDescendants(laWidget* wgt);

// *****************************************************************************
/* Function:
    laBool laWidget_GetEnabled(laWidget* wgt)

  Summary:
    Returns the boolean value of the widget enabled property

  Description:
    Returns the boolean value of the widget enabled property.  The widget enable
    flag often governs things like appearing 'greyed out' and prohibits user
    interactiong if it is false.  Widgets must individually support this flag.

  Parameters:
    laWidget* wgt - the widget

  Returns:
    laBool - the value of the enabled flag

  Remarks:

*/
LIB_EXPORT laBool laWidget_GetEnabled(laWidget* wgt);

// *****************************************************************************
/* Function:
    laResult laWidget_SetEnabled(laWidget* wgt, laBool enable)

  Summary:
    Sets the boolean value of the widget enabled property

  Description:
    Sets the boolean value of the widget enabled property

  Parameters:
    laWidget* wgt - the widget
    laBool - the desired enabled flag value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetEnabled(laWidget* wgt, laBool enable);

// *****************************************************************************
/* Function:
    laBool laWidget_GetVisible(laWidget* wgt)

  Summary:
    Returns the boolean value of the widget visible property

  Description:
    Returns the boolean value of the widget visible property.  Widgets that are
    invisible will be skipped during the rendering phase.  All descendants also
    logically become invisible when an ancestor does.

  Parameters:
    laWidget* wgt - the widget

  Returns:
    laBool - the flag value

  Remarks:

*/
LIB_EXPORT laBool laWidget_GetVisible(laWidget* wgt);

// *****************************************************************************
/* Function:
    laResult laWidget_SetVisible(laWidget* wgt, laBool visible)

  Summary:
    Sets the boolean value of the widget visible property

  Description:
    Sets the boolean value of the widget visible property

  Parameters:
    laWidget* wgt - the widget
    laBool - the desired setting

  Returns:
    laResult - the operation result

  Remarks:
    This value has no effect on layer objects.  Use laLayer_SetEnabled instead.
*/
LIB_EXPORT laResult laWidget_SetVisible(laWidget* wgt, laBool visible);

// *****************************************************************************
/* Function:
    laScheme* laWidget_GetScheme(laWidget* wgt)

  Summary:
    Returns the scheme associated with the specified widget

  Description:
    Returns the scheme associated with the specified widget

  Parameters:
    laWidget* wgt - the widget

  Returns:
    laScheme* - a pointer to the active scheme for a widget

  Remarks:

*/
LIB_EXPORT laScheme* laWidget_GetScheme(laWidget* wgt);

// *****************************************************************************
/* Function:
    void laWidget_SetScheme(laWidget* wgt, laScheme* scheme)

  Summary:
    Sets the scheme variable for the specified widget

  Description:
    Sets the scheme variable for the specified widget.  The scheme defines the
    appearance of the widget.  Setting this to NULL may result in undefined
    behavior if the widget doesn't properly support a NULL scheme.

  Parameters:
    laWidget* wgt - the widget
    laScheme* scheme - a pointer to a scheme or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetScheme(laWidget* wgt,
                                       laScheme* scheme);

// *****************************************************************************
/* Function:
    void laWidget_SetPosition(laWidget* wgt, int32_t x, int32_t y)

  Summary:
    Changes the widget position to the new defined x and y coordinates.

  Description:
    Changes the widget position to the new defined x and y coordinates.  Moving
    widgets can be expensive as it needs to repaint multiple areas of its
    parent widget.

  Parameters:
    laWidget* wgt - the widget
    int32_t x - the new x coordinate
    int32_t y - the new y coordinate

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetPosition(laWidget* wgt,
                                         int32_t x,
                                         int32_t y);

// *****************************************************************************
/* Function:
    void laWidget_Translate(laWidget* wgt, int32_t x, int32_t y)

  Summary:
    Changes the widget position by moving the widget by the defined
    x and y increments.

  Description:
    Changes the widget position by moving the widget by the defined
    x and y increments.Moving widgets can be expensive as it needs to
    repaint multiple areas of its parent widget.


  Parameters:
    laWidget* wgt - the widget
    int32_t x - the amount to move in x
    int32_t y - the amount to move in y

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_Translate(laWidget* wgt,
                                       int32_t x,
                                       int32_t y);

// *****************************************************************************
/* Function:
    void laWidget_SetSize(laWidget* wgt, uint32_t width, uint32_t height)

  Summary:
    Changes the widget size to the new defined width and height dimensions.

  Description:
    Changes the widget size to the new width and height dimensions.

  Parameters:
    laWidget* wgt - the widget
    int32_t width - the new width size
    int32_t height - the new height size

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetSize(laWidget* wgt,
                                     uint32_t width,
                                     uint32_t height);

// *****************************************************************************
/* Function:
    void laWidget_Resize(laWidget* wgt, int32_t width, int32_t height)

  Summary:
    Changes the widget size by the new defined width and height increments.

  Description:
    Changes the widget size by the new defined width and height increments.

  Parameters:
    laWidget* wgt - the widget
    int32_t width - the amount to change the width by
    int32_t height - the amount ot change the height by

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_Resize(laWidget* wgt, int32_t width, int32_t height);

// *****************************************************************************
/* Function:
    laBorderType laWidget_GetBorderType(laWidget* wgt)

  Summary:
    Return the border type associated with the widget object

  Description:
    Return the border type associated with the widget object

  Parameters:
    laWidget* wgt - the widget

  Returns:
    laBorderType - the current widget border type

  Remarks:

*/
LIB_EXPORT laBorderType laWidget_GetBorderType(laWidget* wgt);

// *****************************************************************************
/* Function:
    laResult laWidget_SetBorderType(laWidget* wgt, laBorderType type)

  Summary:
    Set the border type associated with the widget object

  Description:
    Set the border type associated with the widget object

  Parameters:
    laWidget* wgt - the widget
    laBorderType type - the desired border type

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetBorderType(laWidget* wgt, laBorderType type);

// *****************************************************************************
/* Function:
    laBackgroundType laWidget_GetBackgroundType(laWidget* wgt)

  Summary:
    Return the property value 'background type' associated with the widget object

  Description:
    Return the property value 'background type' associated with the widget object
    The background type property decides if the widget background is drawn and
    re-drawn. If background is none, the entire parent widget will be re-drawn
    in the event that the widget gets dirty and needs re-drawing.

  Parameters:
    laWidget* wgt - the widget

  Returns:
    laBackgroundType - the current background type

  Remarks:

*/
LIB_EXPORT laBackgroundType laWidget_GetBackgroundType(laWidget* wgt);

// *****************************************************************************
/* Function:
    laResult laWidget_SetBackgroundType(laWidget* wgt, laBackgroundType type)

  Summary:
    Set the property value 'background type' associated with the widget object

  Description:
    Set the property value 'draw background' associated with the widget object

  Parameters:
    laWidget* wgt - the widget
    laBackgroundType type - the desired background type

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetBackgroundType(laWidget* wgt,
                                               laBackgroundType type);

// *****************************************************************************
/* Function:
    laBool laWidget_GetOptimizationFlags(laWidget* wgt)

  Summary:
    Returns the optimization flags for the widget

  Description:


  Parameters:
    laWidget* wgt - the widget

  Returns:
    laBool - the flag value

  Remarks:

*/
LIB_EXPORT laBool laWidget_GetOptimizationFlags(laWidget* wgt);

// *****************************************************************************
/* Function:
    laResult laWidget_SetOptimizationFlags(laWidget* wgt, uint32_t flags)

  Summary:
    Sets the optimizations for a widget

  Description:
    See the optimizations enum for a descriptions of the individual flags

  Parameters:
    laWidget* wgt - the widget

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetOptimizationFlags(laWidget* wgt, uint32_t flags);

// *****************************************************************************
/* Function:
// BA changed "laWidget_GetMargins to laWidget_GetMargin because that's the
// correct function name. This was a Harmont bug.
    laResult laWidget_GetMargin (laWidget* wgt, laMargin* mg)

  Summary:
    Returns the margin object, which contains the left, right, top and bottom 
	values, associated with the widget in the laMargin pointer

  Description:
    Returns the margin object associated with the widget in the laMargin pointer

  Parameters:
    laWidget* wgt - the widget
    laMargin* mg - a pointer to an laMargin object to store the margin values

  Returns:
    laResult - the operation result

  Remarks:

*/
// BA changed "laWidget_GetMargins to laWidget_GetMargin because that's the
// correct function name. This was a Harmont bug.
LIB_EXPORT laResult laWidget_GetMargin (laWidget* wgt, laMargin* mg);

// *****************************************************************************
/* Function:
    laResult laWidget_SetMargins(laWidget* wgt,
                                 uint32_t left,
                                 uint32_t top,
                                 uint32_t right,
                                 uint32_t bottom)

  Summary:
    Set the margin value for left, right, top and bottom margins associated
    with the widget

  Description:
    Set the margin value for left, right, top and bottom margins associated
    with the widget.  Margins are a generic property and it is up to the
    individual widget to implement them (or not).

  Parameters:
    laWidget* wgt - the widget
    uint32_t left - the left margin value
    uint32_t top - the top margin value
    uint32_t right - the right margin value
    uint32_t bottom - the bottom margin value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetMargins(laWidget* wgt,
                                        uint32_t left,
                                        uint32_t top,
                                        uint32_t right,
                                        uint32_t bottom);

// *****************************************************************************
/* Function:
    uint32_t laWidget_GetCornerRadius(laWidget* wgt)

  Summary:
    Returns the corner radius of the widget

  Description:
    Returns the corner radius of the widget

  Parameters:
    laWidget* wgt - the widget

  Returns:
    uint32_t - the corner radius

  Remarks:

*/
LIB_EXPORT uint32_t laWidget_GetCornerRadius(laWidget* wgt);

// *****************************************************************************
/* Function:
    laResult laWidget_SetCornerRadius(laWidget* wgt,
                                      uint32_t radius)

  Summary:
    Sets the widget corner radius.

  Description:
    Sets the widget corner radius. A widget with non-zero corner radius will have
    round edges. This only supports widgets with non-beveled borders.

  Parameters:
    laWidget* wgt - the widget
    uint32_t radius - the radius

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetCornerRadius(laWidget* wgt,
                                            uint32_t radius);

// *****************************************************************************
/* Function:
    laBool laWidget_HasFocus(laWidget* wgt)

  Summary:
    Checks if the widget specified has focus in the current context

  Description:
    Checks if the widget specified has focus in the current context

  Parameters:
    laWidget* wgt - the widget

  Returns:
    laBool - true of the widget currently has context focus

  Remarks:

*/
LIB_EXPORT laBool laWidget_HasFocus(laWidget* wgt);

// *****************************************************************************
/* Function:
    laResult laWidget_SetFocus(laWidget* wgt)

  Summary:
    Set the widget into focus for the current context.

  Description:
    Set the widget into focus for the current context.
    The input events etc are received by the widget once it is in focus

  Parameters:
    laWidget* wgt - the widget

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_SetFocus(laWidget* wgt);

// *****************************************************************************
/* Function:
    laResult laWidget_OverrideTouchDownEvent(laWidget* wgt,
                                             laWidget_TouchDownEvent_FnPtr ptr)

  Summary:
    Replace the TouchDownEvent callback for the widget with the new function
    pointer specified

  Description:
    This function will replace the current touch down event handler for a widget.
    Widgets may have their own internal override for this function and replacing
    it will break their internal capabilties.

  Parameters:
    laWidget* wgt - the widget
    laWidget_TouchDownEvent_FnPtr - a valid pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_OverrideTouchDownEvent(laWidget* wgt,
                                                    laWidget_TouchDownEvent_FnPtr ptr);

// *****************************************************************************
/* Function:
    laResult laWidget_OverrideTouchUpEvent(laWidget* wgt,
                                           laWidget_TouchUpEvent_FnPtr ptr)

  Summary:
    Replace the TouchUpEvent callback for the widget with the new function
    pointer specified

  Description:
    This function will replace the current touch up event handler for a widget.
    Widgets may have their own internal override for this function and replacing
    it will break their internal capabilties.

  Parameters:
    laWidget* wgt - the widget
    laWidget_TouchUpEvent_FnPtr - a valid pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_OverrideTouchUpEvent(laWidget* wgt, laWidget_TouchUpEvent_FnPtr ptr);

// *****************************************************************************
/* Function:
    laResult laWidget_OverrideTouchMovedEvent(laWidget* wgt,
                                              laWidget_TouchMovedEvent_FnPtr ptr)

  Summary:
    Replace the TouchMovedEvent callback for the widget with the new function
    pointer specified

  Description:
    This function will replace the current touch moved event handler for a widget.
    Widgets may have their own internal override for this function and replacing
    it will break their internal capabilties.

  Parameters:
    laWidget* wgt - the widget
    laWidget_TouchMovedEvent_FnPtr - a valid pointer or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laWidget_OverrideTouchMovedEvent(laWidget* wgt,
                                                     laWidget_TouchMovedEvent_FnPtr ptr);

// *****************************************************************************
/* Function:
    void laWidget_Invalidate(laWidget* wgt)

  Summary:
    Invalidates the specified widget.

  Description:
    This function invalidates the specified widget.  Invalid widgets are redrawn
    during the next paint loop call.  This function may also invalidate the
    widget's parent, siblings, ancestors, or cousins.

  Parameters:
    laWidget* wgt - the widget

  Returns:
    void

  Remarks:

*/
LIB_EXPORT void laWidget_Invalidate(laWidget* wgt);

// DOM-IGNORE-BEGIN
// internal use only
void _laWidget_Invalidate(laWidget* wgt, const GFX_Rect* rect);
laBool _laWidget_IsOccluded(laWidget* wgt, const GFX_Rect* rect);

void _laWidget_ValidateChildren(laWidget* wgt);

void _laWidget_IncreaseDirtyState(laWidget* wgt, uint32_t state);
void _laWidget_SetDirtyState(laWidget* wgt, uint32_t state);
void _laWidget_ClearDirtyState(laWidget* wgt);
// DOM-IGNORE-END

#endif /* LIBARIA_WIDGET_H */
