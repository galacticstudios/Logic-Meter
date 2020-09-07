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
    libaria_context.h

  Summary:
    Context definition for the Aria user interface library.
    
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_CONTEXT_H
#define LIBARIA_CONTEXT_H
// DOM-IGNORE-END

/*********************************************************************
 * Section: Includes
 *********************************************************************/
#include "gfx/libaria/inc/libaria_common.h"

#include "gfx/hal/inc/gfx_common.h"
#include "gfx/utils/gfx_utils.h"

#include "gfx/libaria/inc/libaria_editwidget.h"
#include "gfx/libaria/inc/libaria_event.h"
#include "gfx/libaria/inc/libaria_list.h"
#include "gfx/libaria/inc/libaria_input.h"
#include "gfx/libaria/inc/libaria_scheme.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

typedef struct laScreen_t laScreen;
typedef struct GFXU_StringTableAsset_t GFXU_StringTableAsset;

#ifdef __cplusplus
    extern "C" {
#endif

/*
Enumeration:
        laContextFrameState_t

    Summary:
        Possible values for context frame state.

    Description:
        Possible values for context frame state.

    Remarks:
        None.
 */
typedef enum laContextFrameState_t
{
    LA_CONTEXT_FRAME_READY = 0,
    LA_CONTEXT_FRAME_PREFRAME,
    LA_CONTEXT_FRAME_PRELAYER,
    LA_CONTEXT_FRAME_DRAWING,
    LA_CONTEXT_FRAME_POSTLAYER,
} laContextFrameState;


/*
Enumeration:
        laContextUpdateState

    Summary:
        Possible values for context update state.

    Description:
        Possible values for context update state.

    Remarks:
        None.
 */
typedef enum laContextUpdateState_t
{
    LA_CONTEXT_UPDATE_DONE = 0,
    LA_CONTEXT_UPDATE_PENDING
} laContextUpdateState;

/*
Enumeration:
        laContextPreprocessAssetState

    Summary:
        Possible values for asset preprocess state.

    Description:
        Possible values for asset preprocess state.

    Remarks:
        None.
 */
typedef enum laContextPreprocessAssetsState_t
{
    LA_CONTEXT_PREPROCESS_ASSETS_DONE = 0,
    LA_CONTEXT_PREPROCESS_ASSETS_IN_PROGRESS,            
    LA_CONTEXT_PREPROCESS_ASSETS_PENDING,
} laContextPreprocessAssetsState;

// *****************************************************************************
/* Type:
    laContext_ActiveScreenChangedCallback_FnPtr

  Summary:
    Callback pointer for the active screen change notification.

  Description:
    Callback pointer for the active screen change notification.

  Remarks:
    
*/
typedef void (*laContext_ActiveScreenChangedCallback_FnPtr)(int32_t, int32_t);

// *****************************************************************************
/* Type:
    laContext_LanguageChangedCallback_FnPtr

  Summary:
    Callback pointer for when the language change event occurs.

  Description:
    Callback pointer for when the language change event occurs.

  Remarks:

*/
typedef void (*laContext_LanguageChangedCallback_FnPtr)(uint32_t);

// *****************************************************************************
/* Type:
    laContext_PreprocessAssetsStateChangedCallback_FnPtr

  Summary:
    Callback pointer for when asset pre-processing is about to changes state

  Description:
    Callback pointer for when asset pre-processing is about to change state. 
    Returning LA_FAILURE in the callback function during LA_CONTEXT_PREPROCESS_ASSETS_PENDING
    will suspend the preprocessing. Preprocess will not start until the function 
    returns LA_SUCCESS.

  Remarks:

*/
typedef laResult (*laContext_PreprocessAssetsStateChangedCallback_FnPtr) (laContextPreprocessAssetsState);


// *****************************************************************************
/* Structure:
    laContext

  Summary:
    An instance of the Aria user interface library.

  Description:
    The context represents an discrete instance of Aria user interface library.
    The library is designed to be multi-instance and fully re-entrant.  The
    entire state of the library is stored and referenced through the context
    pointer.

  Remarks:
    None.
*/
typedef struct laContext_t
{
    GFX_Display displayIndex; // the display the library is using 
    void*       gfxContext; // the HAL context the library owns
    
    GFXU_MemoryIntf memIntf; // the memory interface the library is using

    laPreemptionLevel preemptLevel; // the preemption level the library is using

    laArray screenList; // the list of the screens in the context
    laScreen* activeScreen; // the currently active screen
    
    uint32_t frameState; // the context frame state
    uint32_t currentLayer; // the current drawing layer

    laInputState input; // the input state of the instance
    laEventState event; // the event state if the instance
    
    GFXU_StringTableAsset* stringTable; // the string table for the instance
    uint32_t languageID; // the currently active language
    
    uint32_t widgetIDs; // the next unique widget ID
    
    laScheme defaultScheme; // an internal default scheme that widgets use by
                            // default if the user doesn't set one
                            
    GFX_ColorMode colorMode; // the color mode the library uses for all layers

    laWidget* focus; // the widget that currently has focus
    laEditWidget* edit; // the widget that is currently receiving edit events
    
    laContext_ActiveScreenChangedCallback_FnPtr screenChangedCB; // screen changed callback
    laContext_LanguageChangedCallback_FnPtr languageChangedCB; // language changed callback
    laContext_PreprocessAssetsStateChangedCallback_FnPtr preprocessStateChangedCB; // asset preprocess state changed callback
} laContext;


// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    laContext* laContext_Create(laArray*)

   Summary:
    Creates an instance of the Aria user interface library 

   Precondition:
     Should have called laInitialize() before attempting to create a context

   Parameters:
    GFX_Driver - the graphics controller the library will initialize the
                 HAL with
    GFX_Display - the graphics display the library will initialize the HAL
                  with
    GFX_ColorMode - the color mode the library will use and initialize the HAL
                    with
    GFXU_MemoryIntf* - the memory interface the library will use and will
                       initialize the HAL with

  Returns:
    laContext* - a valid context pointer or NULL

  Remarks:    
    
*/
LIB_EXPORT laContext* laContext_Create(GFX_Driver driver,
                                       GFX_Display display,
                                       GFX_Processor processor,
                                       GFX_ColorMode mode,
                                       GFXU_MemoryIntf* memoryIntf);

// *****************************************************************************
/* Function:
    laResult laContext_Destroy(laContext*)

   Summary:
    Destroys an Aria instance

   Parameters:
    laContext* - a valid Aria pointer

  Returns:
    laResult - indicates if the instance was successfully destroyed

  Remarks:    
    
*/                                       
LIB_EXPORT laResult laContext_Destroy(laContext* context);

// *****************************************************************************
/* Function:
    void laContext_Update(uint32_t dt)

   Summary:
    Runs the update loop for a library instance.  

   Description:
    The update loop allows the library to service its event array and allows
    any intelligent widgets to perform active update tasks.  This should be run
    periodically, but not often enough to starve other processes.  Running too
    little may result in a loss of UI responsiveness.

   Precondition:

   Parameters:
    uint32_t dt - a delta time representing how much time has passed since the
                  last time laContext_Update has been called.  This is typically in
                  milliseconds.

  Returns:
    void
*/
LIB_EXPORT void laContext_Update(uint32_t dt);

// *****************************************************************************
/* Function:
    laContext* laContext_GetActive()

   Summary:
    Returns the current active context.

  Returns:
    laContext* 
*/
LIB_EXPORT laContext* laContext_GetActive();

// *****************************************************************************
/* Function:
    laResult laContext_SetActive(laContext* context)

   Summary:
    Make the specified context active 

  Returns:
    laResult - LA_SUCCESS if the context was successfully set as active
*/
LIB_EXPORT laResult laContext_SetActive(laContext* context);

// *****************************************************************************
/* Function:
    GFX_ColorMode laContext_GetColorMode()
    
   Summary:
    Returns the color mode of the current context

   Returns:
    GFX_ColorMode
*/
LIB_EXPORT GFX_ColorMode laContext_GetColorMode();

// *****************************************************************************
/* Function:
    laResult laContext_AddScreen(laScreen* screen)

   Summary:
    Add screen to the list of screens in the current context  

   Description:
    Add screen to the list of screens in the current context  

   Precondition:

   Parameters:
    laScreen*

  Returns:
    laResult

  Remarks:    
    
*/
LIB_EXPORT laResult laContext_AddScreen(laScreen* screen);

// *****************************************************************************
/* Function:
    laResult laContext_RemoveScreen(laScreen* screen)

   Summary:
    Remove the specified screen from the list of screens in the current context  

   Description:
    Remove the specified screen from the list of screens in the current context

   Precondition:

   Parameters:
    laScreen*

  Returns:
    laResult

  Remarks:    
    
*/
LIB_EXPORT laResult laContext_RemoveScreen(laScreen* screen);

// *****************************************************************************
/* Function:
    LIB_EXPORT GFX_Rect laContext_GetScreenRect()

   Summary:
    Returns the display rectangle structure of the physical display

   Description:
    Returns the display rectangle - width height and upper left corner coordinates
    of the physical display

   Precondition:

   Parameters:
    
  Returns:
    GFX_Rect

  Remarks:    
    
*/
LIB_EXPORT GFX_Rect laContext_GetScreenRect();

// *****************************************************************************
/* Function:
    laScreen* laContext_GetActiveScreen()

   Summary:
    Returns the active screen of the current context  

   Description:
    Returns the active screen of the current context 

   Precondition:

   Parameters:
    
  Returns:
    laScreen*

  Remarks:    
    
*/
LIB_EXPORT laScreen* laContext_GetActiveScreen();

// *****************************************************************************
/* Function:
    int32_t laContext_GetActiveScreenIndex()

   Summary:
    Return the index of the active screen  

   Description:
    Return the index of the active screen

   Precondition:

   Parameters:
    
  Returns:
    int32_t

  Remarks:    
    
*/
LIB_EXPORT int32_t laContext_GetActiveScreenIndex();

// *****************************************************************************
/* Function:
    laResult laContext_SetActiveScreen(uint32_t id)

   Summary:
    Change the active screen to the one specified by the index argument  

   Description:
    This operation will tear down the existing layer state of the driver if
    necessary and rebuild the frame buffers if the existing buffers can not be
    reused.  This operation can be potentially slow and expensive.  Widgets
    can be used to simulate screen transitions as applicable.

   Precondition:

   Parameters:
    uint32_t

  Returns:
    void

  Remarks:    
    
*/
LIB_EXPORT laResult laContext_SetActiveScreen(uint32_t id);

// *****************************************************************************
/* Function:
    laResult laContext_HideActiveScreen()

   Summary:
    Hide the active screen   

   Description:
    Hide the active screen. If the screen's persistent flag is set to true then
    the memory for the screen's widgets will not be deallocated.  This will
    maintain the state of the screen.

   Precondition:

   Parameters:
    
  Returns:
    void

  Remarks:    
    
*/
LIB_EXPORT GFX_DEPRECATED laResult laContext_HideActiveScreen();

// *****************************************************************************
/* Function:
    laPreemptionLevel laContext_GetPreemptionLevel()

   Summary:
    Returns the preemption level for the screen  

   Description:
    Returns the preemption level for the screen

   Precondition:

   Parameters:
    
  Returns:
    laPreemptionLevel

  Remarks:    
    
*/
LIB_EXPORT laPreemptionLevel laContext_GetPremptionLevel();

// *****************************************************************************
/* Function:
    laResult laContext_SetPreemptionLevel(laPreemptionLevel level)

   Summary:
    Set the preemption level to the specified value  

   Description:
    Set the preemption level to the specified value

   Precondition:

   Parameters:
    laPreemptionLevel

  Returns:
    laResult

  Remarks:    
    
*/
LIB_EXPORT laResult laContext_SetPreemptionLevel(laPreemptionLevel level);

// *****************************************************************************
/* Function:
    GFXU_StringTableAsset* laContext_GetStringTable()

   Summary:
    Get a pointer to the GFXU_StringTableAsset structure that maintains the strings, associated fonts, etc

   Description:
    Get a pointer to the GFXU_StringTableAsset structure that maintains the strings, associated fonts, etc

   Precondition:

   Parameters:
    
  Returns:
    GFXU_StringTableAsset*

  Remarks:    
    
*/
LIB_EXPORT GFXU_StringTableAsset* laContext_GetStringTable();

// *****************************************************************************
/* Function:
    void laContext_SetStringTable(GFXU_StringTableAsset* table)

   Summary:
    Set the StringTable pointer to the specified new StringTableAsset structure  

   Description:
    Set the StringTable pointer to the specified new StringTableAsset structure 

   Precondition:

   Parameters:
    GFXU_StringTableAsset*

  Returns:
    void

  Remarks:    
    
*/
LIB_EXPORT void laContext_SetStringTable(GFXU_StringTableAsset* table);

// *****************************************************************************
/* Function:
    uint32_t laContext_GetStringLanguage()

   Summary:
    Returns the language index of the current context  

   Description:
    Returns the language index of the current context

   Precondition:

   Parameters:
    
  Returns:
    uint32_t

  Remarks:    
    
*/
LIB_EXPORT uint32_t laContext_GetStringLanguage();

// *****************************************************************************
/* Function:
    void laContext_SetStringLanguage(uint32_t id)

   Summary:
    Set the language index of the current context  

   Description:
    Set the language index of the current context 

   Precondition:

   Parameters:
    uint32_t

  Returns:
    void

  Remarks:    
    
*/
LIB_EXPORT void laContext_SetStringLanguage(uint32_t id);

// *****************************************************************************
/* Function:
    laScheme* laContext_GetDefaultScheme()

   Summary:
    Returns the pointer to the default scheme of the current context  

   Description:
    Returns the pointer to the default scheme of the current context 

   Precondition:

   Parameters:
    
  Returns:
    laScheme*

  Remarks:    
    
*/
LIB_EXPORT laScheme* laContext_GetDefaultScheme();

// *****************************************************************************
/* Function:
    laWidget* laContext_GetFocusWidget()

   Summary:
    Return a pointer to the widget in focus 

   Description:
    The focus widget is the widget that is currently receiving all input events.
    This can happen when the user initiates a touch down event on the widget
    and is currently dragging their finger on the display.  The widget will
    receive all touch moved events until a touch up event is received.

   Precondition:

   Parameters:
    
  Returns:
    laWidget*

  Remarks:    
    
*/
LIB_EXPORT laWidget* laContext_GetFocusWidget();

// *****************************************************************************
/* Function:
    laResult laContext_SetFocusWidget(laWidget* widget)

   Summary:
    Set into focus the widget specified  as the argument 

   Description:
    Set into focus the widget specified  as the argument

   Precondition:

   Parameters:
    laWidget*

  Returns:
    laResult

  Remarks:    
    
*/
LIB_EXPORT laResult laContext_SetFocusWidget(laWidget* widget);

// *****************************************************************************
/* Function:
    laEditWidget* laContext_GetEditWidget()

   Summary:
    Gets the widget that is currently recieving all widget edit events.

   Description:
    Edit widgets are widgets that inherit the 'edit widget' API function list.
    These widgets are capable of receiving edit events from other widgets
    that are edit event broadcasters.  A broadcaster could be a 'key pad' and
    a receiver could be a 'text edit' box.

   Precondition:

   Parameters:
    void

  Returns:
    laEditWidget*

  Remarks:    
    
*/
LIB_EXPORT laEditWidget* laContext_GetEditWidget();

// *****************************************************************************
/* Function:
    laResult laContext_SetEditWidget(laWidget* widget)

   Summary:
    Sets the currently active edit widget.  

   Precondition:

   Parameters:
    laWidget* - a widget that inherits the edit widget API and has its
                'editable' flag set to true.

  Returns:
    laResult

  Remarks:    
    
*/
LIB_EXPORT laResult laContext_SetEditWidget(laWidget* widget);

// *****************************************************************************
/* Function:
    laResult laContext_SetPreprocessAssetStateChangedCallback(laContext_PreprocessAssetsStateChangedCallback_FnPtr cb)

   Summary:
    Set the callback function pointer when the preprocess state change event occurs  

   Description:
    Set the callback function pointer when the preprocess state change event occurs  

   Precondition:

   Parameters:
    laContext_AssetsPreProcessStateChangedCallback_FnPtr

  Returns:
    laResult

  Remarks:    
    
*/
LIB_EXPORT laResult laContext_SetPreprocessAssetStateChangedCallback(laContext_PreprocessAssetsStateChangedCallback_FnPtr cb);

// *****************************************************************************
/* Function:
    laResult laContext_SetActiveScreenChangedCallback(laContext_ActiveScreenChangedCallback_FnPtr cb)

   Summary:
    Set the callback function pointer when the screen change event occurs  

   Description:
    Set the callback function pointer when the screen change event occurs  

   Precondition:

   Parameters:
    laContext_ActiveScreenChangedCallback_FnPtr

  Returns:
    laResult

  Remarks:    
    
*/
LIB_EXPORT laResult laContext_SetActiveScreenChangedCallback(laContext_ActiveScreenChangedCallback_FnPtr cb);

// *****************************************************************************
/* Function:
    laResult laContext_SetLanguageChangedCallback(laContext_LanguageChangedCallback_FnPtr cb)

   Summary:
    Set the callback function pointer when the language change event occurs  

   Description:
    Set the callback function pointer when the language change event occurs

   Precondition:

   Parameters:
    laContext_LanguageChangedCallback_FnPtr

  Returns:
    laResult

  Remarks:    
    
*/
LIB_EXPORT laResult laContext_SetLanguageChangedCallback(laContext_LanguageChangedCallback_FnPtr cb);

// *****************************************************************************
/* Function:
    void laContext_RedrawAll()

   Summary:
    Forces the library to redraw the currently active screen in its entirety.  

   Precondition:

   Parameters:
    
  Returns:
    void

  Remarks:    
    
*/
LIB_EXPORT void laContext_RedrawAll();

/*  Function:
        laBool laContext_IsDrawing()
 
    Summary:
        Indicates if any layers of the active screen are currently drawing a
        frame.
        
    Description:
        Indicates if any layers are currently drawing a frame.  Because frame updates
        can happen long after making changes to the UI state it is best to only make 
        updates to the state of a layer tree only when the layer is not drawing.
        
        Requires an active context and active screen.
        
    Returns:
        laResult    

*/
LIB_EXPORT laBool laContext_IsDrawing();

/*  Function:
        laBool laContext_IsLayerDrawing(uint32_t idx)
 
    Summary:
        Indicates if the layer at the given index of the active screen is
        currently drawing.
        
    Description:
        Indicates if the layer at the given index is currently drawing a frame. 
        Because frame updates can happen long after making changes to the UI
        state it is best to only make updates to the state of a layer tree
        only when the layer is not drawing.
        
        Requires an active context and active screen.
        
    Parameters:
        uint32_t idx - the index of the layer to query
        
    Returns:
        laResult    

*/
LIB_EXPORT laBool laContext_IsLayerDrawing(uint32_t idx);

// DOM-IGNORE-BEGIN
// internal use only
uint32_t _laContext_Update(uint32_t dt);

// internal use only
void _laContext_Paint();

// internal use only
laEventResult _laContext_HandleScreenChangeEvent(laEvent* evt);
void _laContext_ShowScreen(uint32_t idx);
// DOM-IGNORE-END

#ifdef __cplusplus
    }
#endif
    
#endif // LIBARIA_CONTEXT_H
