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
    libaria_widget_list.h

  Summary:


  Description:
    This module implements list box widget functions.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_LISTWIDGET_H
#define LIBARIA_LISTWIDGET_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_LIST_WIDGET_ENABLED && LA_SCROLLBAR_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_list.h"
#include "gfx/libaria/inc/libaria_widget.h"
#include "gfx/libaria/inc/libaria_widget_scrollbar.h"
#include "gfx/libaria/inc/libaria_string.h"

typedef struct laListWidget_t laListWidget;

// *****************************************************************************
/* Function Pointer:
    laListWidget_SelectedItemChangedEvent

  Summary:
    Selected item changed event function callback type
*/
typedef void (*laListWidget_SelectedItemChangedEvent)(laListWidget*,
                                                      uint32_t idx,
                                                      laBool selected);

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laListWidget_SelectionMode_t

  Summary:
    Defines the list selection modes

  Description:
    Single - a single selection from the list is allowed at any one time
    Multiple - any number of selected items is allowed at any one time
    Contiguous - any number of selected items in a contiguous serious is allowed
                 at any one time

  Remarks:
    None.
*/
typedef enum laListWidget_SelectionMode_t
{
    LA_LIST_WIDGET_SELECTION_MODE_SINGLE,
    LA_LIST_WIDGET_SELECTION_MODE_MULTIPLE,
    LA_LIST_WIDGET_SELECTION_MODE_CONTIGUOUS
} laListWidget_SelectionMode;

// *****************************************************************************
/* Structure:
    laListItem_t

  Summary:
    Defines a list item struct

  Description:


  Remarks:
    None.
*/
typedef struct laListItem_t
{
    laString string; // list item string
    GFXU_ImageAsset* icon; // list item icon
    laBool selected; // list item selected flag
    GFX_Rect rowRect; // list item row rectangle
    laBool enabled; //enable or disable the item
} laListItem;

// *****************************************************************************
/* Structure:
    laListWidget_t

  Summary:
    Defines the implementation of a list widget

  Description:
    A list widget is a widget that contains a series of vertical nodes.  Each
    node can have text, an image, or both, and can be selected or not.  The list
    has a built-in scrollbar.  This allows the list to be larger than the visible
    area of the widget.

  Remarks:
    None.
*/
typedef struct laListWidget_t
{
    laWidget widget; // list base class

    laListWidget_SelectionMode mode; // list selection mode
    laBool allowEmpty; // indicates if the list must always have at least one
                       // selected item

    laArray items; // list containing the list items

    laHAlignment halign; // horizontal alignment of the list
    laRelativePosition iconPos; // icon position for the list icons
    uint32_t iconMargin; // margin for the list icons

    uint32_t itemDown; // tracks whether an input event is in process

    laScrollBarWidget* scrollbar; // internal scrollbar for this widget

    struct
    {
        laListItem* item;
        GFX_Rect itemRect;

        int32_t y;

        uint32_t nextItem;
    } paintState;

    laListWidget_SelectedItemChangedEvent cb; // item selected changed event

    GFXU_ExternalAssetReader* reader;
} laListWidget;

// DOM-IGNORE-BEGIN
// internal use only
void _laListWidget_Constructor(laListWidget* lst);
void _laListWidget_Destructor(laListWidget* lst);

void _laListWidget_Paint(laListWidget* lst);
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laListWidget* laListWidget_New()

  Summary:
    Allocates memory for and initializes a new widget of this type.  The
    application is responsible for the managment of this memory until the
    widget is added to a widget tree.

  Description:


  Parameters:

  Returns:
    laListWidget* lst - the widget

  Remarks:

*/
LIB_EXPORT laListWidget* laListWidget_New();

// *****************************************************************************
/* Function:
    laListWidget_SelectionMode laListWidget_GetSelectionMode(laListWidget* lst)

  Summary:
    Gets the selection mode for the list

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    laListWidget_SelectionMode - the list selection mode

  Remarks:

*/
LIB_EXPORT laListWidget_SelectionMode laListWidget_GetSelectionMode(laListWidget* lst);

// *****************************************************************************
/* Function:
    laResult laListWidget_SetSelectionMode(laListWidget* lst,
                                           laListWidget_SelectionMode mode)

  Summary:
    Set the list selection mode

  Description:


  Parameters:
    laListWidget* lst - the widget
    laListWidget_SelectionMode mode - the desired list selection mode

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SetSelectionMode(laListWidget* lst,
                                                  laListWidget_SelectionMode mode);

// *****************************************************************************
/* Function:
    laBool laListWidget_GetAllowEmptySelection(laListWidget* lst)

  Summary:
    Returns true of the list allows an empty selection set

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    laBool - true if the list allows an empty selection set

  Remarks:

*/
LIB_EXPORT laBool laListWidget_GetAllowEmptySelection(laListWidget* lst);

// *****************************************************************************
/* Function:
    laResult laListWidget_SetAllowEmptySelection(laListWidget* lst,
                                                 laBool allow)

  Summary:
    Configures the list to allow an empty selection set.

  Description:


  Parameters:
    laListWidget* lst - the widget
    laBool allow - the desired empty selection set mode

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SetAllowEmptySelection(laListWidget* lst,
                                                        laBool allow);

// *****************************************************************************
/* Function:
    laHAlignment laListWidget_GetAlignment(laListWidget* lst)

  Summary:
    Gets the horizontal alignment for the list widget

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    laHAlignment - the current list halign mode

  Remarks:

*/
LIB_EXPORT laHAlignment laListWidget_GetAlignment(laListWidget* lst);

// *****************************************************************************
/* Function:
    laResult laListWidget_SetAlignment(laListWidget* lst,
                                       laHAlignment align)

  Summary:
    Sets the horizontal alignment mode for the list widget.

  Description:


  Parameters:
    laListWidget* lst - the widget
    laHAlignment align - the desired halign mode

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SetAlignment(laListWidget* lst,
                                              laHAlignment align);

// *****************************************************************************
/* Function:
    laRelativePosition laListWidget_GetIconPosition(laListWidget* lst)

  Summary:
    Gets the icon position for the list

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    laRelativePosition - the current icon position

  Remarks:

*/
LIB_EXPORT laRelativePosition laListWidget_GetIconPosition(laListWidget* lst);

// *****************************************************************************
/* Function:
    laResult laListWidget_SetIconPosition(laListWidget* lst,
                                          laRelativePosition pos)

  Summary:
    Sets the icon position for the list widget

  Description:


  Parameters:
    laListWidget* lst - the widget
    laRelativePosition pos - the relative position setting

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SetIconPosition(laListWidget* lst,
                                                 laRelativePosition pos);
// *****************************************************************************
/* Function:
    uint32_t laListWidget_GetIconMargin(laListWidget* lst)

  Summary:
    Gets the icon margin value for the list widget

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    uint32_t - the icon margin value

  Remarks:

*/
LIB_EXPORT uint32_t laListWidget_GetIconMargin(laListWidget* lst);

// *****************************************************************************
/* Function:
    laResult laListWidget_SetIconMargin(laListWidget* lst, uint32_t mg)

  Summary:
    Sets the icon margin value for the list widget.

  Description:
    The icon margin value is the distance between the icon image and the text.

  Parameters:
    laListWidget* lst - the widget
    uint32_t mg - the margin value

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SetIconMargin(laListWidget* lst, uint32_t mg);

// *****************************************************************************
/* Function:
    uint32_t laListWidget_GetItemCount(laListWidget* lst)

  Summary:
    Gets the number of items currently contained in the list

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    uint32_t - the number of items in the list

  Remarks:

*/
LIB_EXPORT uint32_t laListWidget_GetItemCount(laListWidget* lst);

// *****************************************************************************
/* Function:
    uint32_t laListWidget_AppendItem(laListWidget* lst)

  Summary:
    Appends a new item entry to the list.  The initial value of the item
    will be empty.

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    uint32_t - the index of the newly appended item

  Remarks:

*/
LIB_EXPORT uint32_t laListWidget_AppendItem(laListWidget* lst);

// *****************************************************************************
/* Function:
    uint32_t laListWidget_InsertItem(laListWidget* lst, uint32_t idx)

  Summary:
    Attempts to insert a new item at the desired index.  Existing items at
    idx or greater will be shuffled one index to the right.

  Description:


  Parameters:
    laListWidget* lst - the widget
    uint32_t idx - the desired index of the new item

  Returns:
    uint32_t - the index of the inserted item

  Remarks:

*/
LIB_EXPORT uint32_t laListWidget_InsertItem(laListWidget* lst, uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laListWidget_RemoveItem(laListWidget* lst, uint32_t idx)

  Summary:
    Attempts to remove an item from the list.

  Description:


  Parameters:
    laListWidget* lst - the widget
    uint32_t idx - the index to remove from the list

  Returns:
    laResult - the operation result

  Remarks:
    The memory owned by the string item will be freed automatically.
*/
LIB_EXPORT laResult laListWidget_RemoveItem(laListWidget* lst, uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laListWidget_RemoveAllItems(laListWidget* lst)

  Summary:
    Attempts to remove all items from the list.

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    laResult - the operation result

  Remarks:
    All memory owned by each item string will be freed automatically.
*/
LIB_EXPORT laResult laListWidget_RemoveAllItems(laListWidget* lst);

// *****************************************************************************
/* Function:
    laBool laListWidget_GetItemSelected(laListWidget* lst,
                                        uint32_t idx)

  Summary:
    Returns true if the item at the given index is currently selected.

  Description:


  Parameters:
    laListWidget* lst - the widget
    uint32_t idx - the index to consider

  Returns:
    laBool - the selection state of the item

  Remarks:

*/
LIB_EXPORT laBool laListWidget_GetItemSelected(laListWidget* lst,
                                               uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laListWidget_SetItemSelected(laListWidget* lst,
                                          uint32_t idx,
                                          laBool selected)

  Summary:
    Attempts to set the item at idx as selected.

  Description:


  Parameters:
    laListWidget* lst - the widget
    uint32_t idx - the index to consider
    laBool - the select state to set to the item

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SetItemSelected(laListWidget* lst,
                                                 uint32_t idx,
                                                 laBool selected);

// *****************************************************************************
/* Function:
    laResult laListWidget_ToggleItemSelected(laListWidget* lst,
                                             uint32_t idx)

  Summary:
    Attempts to toggle the selected state of the item at idx.

  Description:


  Parameters:
    laListWidget* lst - the widget
    uint32_t idx - the index to consider

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWidget_ToggleItemSelected(laListWidget* lst,
                                                    uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laListWidget_SelectAll(laListWidget* lst)

  Summary:
    Attempts to set all item states to selected.

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SelectAll(laListWidget* lst);

// *****************************************************************************
/* Function:
    laResult laListWidget_DeselectAll(laListWidget* lst)

  Summary:
    Attempts to set all item states as not selected.

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWidget_DeselectAll(laListWidget* lst);


// *****************************************************************************
/* Function:
    uint32_t laListWidget_GetFirstSelectedItem(laListWidget* lst)

  Summary:
    Returns the lowest selected item index.

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    uint32_t - the lowest selected item index or -1 if nothing is selected.

  Remarks:

*/
LIB_EXPORT uint32_t laListWidget_GetFirstSelectedItem(laListWidget* lst);

// *****************************************************************************
/* Function:
    uint32_t laListWidget_GetLastSelectedItem(laListWidget* lst)

  Summary:
    Returns the highest selected item index.

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    uint32_t - the highest selected item index or -1 if nothing is selected.

  Remarks:

*/
LIB_EXPORT uint32_t laListWidget_GetLastSelectedItem(laListWidget* lst);

// *****************************************************************************
/* Function:
    uint32_t laListWidget_GetSelectionCount(laListWidget* lst)

  Summary:
    Returns the number of selected items in the list.

  Description:


  Parameters:
    laListWidget* lst - the widget

  Returns:
    uint32_t - the number of selected items

  Remarks:

*/
LIB_EXPORT uint32_t laListWidget_GetSelectionCount(laListWidget* lst);


// *****************************************************************************
/* Function:
    laResult laListWidget_GetItemText(laListWidget* lst,
                                      uint32_t idx,
                                      laString* str)

  Summary:
    Gets the text value for an item in the list.

  Description:
    This function allocates memory and initializes the input string pointer.  The
    caller is responsible for managing the memory once this function returns.

  Parameters:
    laListWidget* lst - the widget
    uint32_t idx - the index to consider
    laString* str - a pointer to an laString object

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWidget_GetItemText(laListWidget* lst,
                                             uint32_t idx,
                                             laString* str);

// *****************************************************************************
/* Function:
    laResult laListWidget_SetItemText(laListWidget* lst,
                                      uint32_t index,
                                      laString str)

  Summary:
    Sets the text value for an item in the list.

  Description:
    This function copies the contents of the input string into its internal
    string buffer.  The input string can then be freed or altered without
    affecting the label's internal string value.

  Parameters:
    laListWidget* lst - the widget
    uint32_t idx - the index to consider
    laString str - an laString object

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SetItemText(laListWidget* lst,
                                             uint32_t index,
                                             laString str);

// *****************************************************************************
/* Function:
    GFXU_ImageAsset* laListWidget_GetItemIcon(laListWidget* lst,
                                              uint32_t idx)

  Summary:
    Gets the pointer to the image asset for the icon for the item at the given
    index.

  Description:


  Parameters:
    laListWidget* lst - the widget
    uint32_t idx - the index to consider

  Returns:
    GFXU_ImageAsset* - the image asset pointer or NULL

  Remarks:

*/
LIB_EXPORT GFXU_ImageAsset* laListWidget_GetItemIcon(laListWidget* lst,
                                                     uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laListWidget_SetItemIcon(laListWidget* lst,
                                      uint32_t idx,
                                      GFXU_ImageAsset* img)

  Summary:
    Sets the icon pointer for a given index.

  Description:


  Parameters:
    laListWidget* lst - the widget
    uint32_t idx - the index to configure
    GFXU_ImageAsset* - the image asset pointer to use as the icon

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SetItemIcon(laListWidget* lst,
                                             uint32_t idx,
                                              GFXU_ImageAsset* img);

// *****************************************************************************
/* Function:
    laResult laListWidget_SetItemVisible(laListWidget* lst,
                                         uint32_t index)

  Summary:


  Description:


  Parameters:
    laListWidget* lst - the widget
    uint32_t index - the index to modify

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SetItemVisible(laListWidget* lst,
                                                uint32_t idx);

// *****************************************************************************
/* Function:
    laBool laListWidget_GetItemEnable(laListWidget* lst,
                                  uint32_t idx)

  Summary:
    Returns the enable state of the item in the list widget

  Description:


  Parameters:
    laListWidget* lst - the widget
    uint32_t index - the index of the item in the list

  Returns:
    laBool - the enable state of the item

  Remarks:

*/
LIB_EXPORT laBool laListWidget_GetItemEnable(laListWidget* lst,
                                               uint32_t idx);

// *****************************************************************************
/* Function:
    laResult laListWidget_SetItemEnable(laListWidget* lst,
                                  uint32_t idx,
                                  laBool newEnableState)

  Summary:
    Enables or disables an item in the list. A disable item becomes
    un-selectable.

  Description:


  Parameters:
    laListWidget* lst - the widget
    uint32_t index - the index of the item in the list
    laBool enable - enable/disable the item

  Returns:
    laResult - the result of the operation

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SetItemEnable(laListWidget* lst,
                                               uint32_t idx,
                                               laBool enable);

// *****************************************************************************
/* Function:
    laListWidget_SelectedItemChangedEvent laListWidget_GetSelectedItemChangedEventCallback(laListWidget* lst)

  Summary:
    Gets the callback for the item selected changed event

  Description:

  Parameters:
    laListWidget* lst - the widget

  Returns:
    laListWidget_SelectedItemChangedEvent - the current pointer to callback or NULL

  Remarks:

*/
LIB_EXPORT laListWidget_SelectedItemChangedEvent laListWidget_GetSelectedItemChangedEventCallback(laListWidget* lst);

// *****************************************************************************
/* Function:
    laResult laListWidget_SetSelectedItemChangedEventCallback(laListWidget* lst,
                                                              laListWidget_SelectedItemChangedEvent cb)

  Summary:
    Sets the callback for the item selected changed event

  Description:
    This callback is called whenver an items selected state is modified.

  Parameters:
    laListWidget* lst - the widget
    laListWidget_SelectedItemChangedEvent - the desired pointer to callback or NULL

  Returns:
    laResult - the operation result

  Remarks:

*/
LIB_EXPORT laResult laListWidget_SetSelectedItemChangedEventCallback(laListWidget* lst,
                                                                     laListWidget_SelectedItemChangedEvent cb);

// DOM-IGNORE-BEGIN
// internal use only
void _laListWidget_GetListRect(laListWidget* lst,
                               GFX_Rect* rect);

void _laListWidget_GetRowRect(laListWidget* lst,
                              uint32_t idx,
                              GFX_Rect* rect);

// internal use only
int32_t _laListWidget_GetRowY(laListWidget* lst, uint32_t idx);

// internal use only
void _laListWidget_GetLogicalRect(laListWidget* lst, GFX_Rect* rect);

void _laListWidget_GetTextRect(laListWidget* lst,
                               uint32_t idx,
                               GFX_Rect* textRect,
                               GFX_Rect* drawRect);

void _laListWidget_GetIconRect(laListWidget* lst,
                               uint32_t idx,
                               GFX_Rect* iconRect,
                               GFX_Rect* imgSrcRect);

void _laListWidget_recalculateRowRect(laListWidget* lst,
                                     uint32_t idx);
// DOM-IGNORE-END

#endif // LA_LIST_WIDGET_ENABLED && LA_SCROLLBAR_WIDGET_ENABLED
#endif /* LIBARIA_LISTWIDGET_H */
