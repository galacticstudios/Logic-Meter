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
    libaria_event.h

  Summary:
    Defines events that are used in the UI library.  Events are created and
    stored for later processing during a library context's update loop.

*******************************************************************************/

// DOM-IGNORE-BEGIN
#ifndef LIBARIA_EVENT_H
#define LIBARIA_EVENT_H
//DOM-IGNORE-END

#include "gfx/libaria/libaria_config.h"
#ifdef LIBARIA_USE_RTOS_EXTENSIONS
#include "osal/osal.h"
#endif
#include "gfx/libaria/inc/libaria_common.h"
#include "gfx/libaria/inc/libaria_list.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Enumeration:
    laEventID

  Summary:
    Defines internal event type IDs
*/
typedef enum laEventID_t
{
    // internal events
    LA_EVENT_NONE,
    LA_EVENT_SCREEN_CHANGE,
    LA_EVENT_TOUCH_DOWN,
    LA_EVENT_TOUCH_UP,
    LA_EVENT_TOUCH_MOVED
} laEventID;

typedef struct laWidget_t laWidget;

// *****************************************************************************
/* Structure:
    laEvent_t

  Summary:
    Basic UI event definition
*/
typedef struct laEvent_t
{
    laEventID id;
} laEvent;

// *****************************************************************************
/* Structure:
    laWidgetEvent_t

  Summary:
    Basic widget event definition  
*/
typedef struct laWidgetEvent_t
{
    laEventID id;
    laWidget* source;
    laWidget* target;
    laBool accepted;
} laWidgetEvent;

// *****************************************************************************
/* Function Pointer:
    laEvent_FilterEvent

  Summary:
    Function pointer to define an event filter.  Event filters allow a 
    receiver to discard undesirable events
*/
typedef laBool (*laEvent_FilterEvent)(laEvent*);

// *****************************************************************************
/* Structure:
    laEventState_t

  Summary:
    Structure to manage the event lists, state and call back pointers

  Remarks:
    None.
*/
typedef struct laEventState_t
{
#ifndef _WIN32
#ifdef LIBARIA_USE_RTOS_EXTENSIONS
    OSAL_SEM_HANDLE_TYPE eventCountSem;
    OSAL_MUTEX_HANDLE_TYPE eventLock;
#endif
#endif
    laList events;
    laEvent_FilterEvent filter;
} laEventState;


// *****************************************************************************
/* Enumeration:
    laEventResult

  Summary:
    Defines what happened when processing an event
*/
typedef enum laEventResult_t
{
    // internal events
    LA_EVENT_HANDLED,     // the event was handled
    LA_EVENT_DEFERRED,    // the event needs to wait
    LA_EVENT_RESET_QUEUE  // the entire event queue should be flushed and reset
} laEventResult;


// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t laEvent_GetCount()

  Summary:
    Returns the number of events listed in the current context

  Description:
    Returns the number of events listed in the current context

  Parameters:
    void
    
  Returns:
    uint32_t
    
  Remarks:
    
*/
LIB_EXPORT uint32_t laEvent_GetCount();

// *****************************************************************************
/* Function:
    laResult laEvent_SetFilter(laEvent_FilterEvent cb)

  Summary:
    Set callback pointer for current context filter event 

  Description:
    Set callback pointer for current context filter event 
  Parameters:
    laEvent_FilterEvent
    
  Returns:
    laResult
  Remarks:
    
*/
LIB_EXPORT laResult laEvent_SetFilter(laEvent_FilterEvent cb); 

// *****************************************************************************
/* Function:
    laResult laEvent_AddEvent(laEvent* evt)

  Summary:
    Add the mentioned event callback to the list of events maintained by the current context

  Description:
    Add the mentioned event callback to the list of events maintained by the current context

  Parameters:
    laEvent*
    
  Returns:
    laResult
    
  Remarks:
    
*/
laResult laEvent_AddEvent(laEvent* evt);

// *****************************************************************************
/* Function:
    laResult laEvent_ClearList()

  Summary:
    Clear the event list maintained by the current context.

  Description:
    Clear the event list maintained by the current context.

  Parameters:
    void
    
  Returns:
    laResult
    
  Remarks:
    
*/
laResult laEvent_ClearList();

// *****************************************************************************
/* Function:
    laResult laEvent_ProcessEvents()

  Summary:
    Processes the screen change as well as touch events 

  Description:
    When a screen change event occurs, the specific screen change event handler 
    has to be called as well as some generic maintenance for the screen change 
    like destroying or hiding screen resources needs to be done. This function 
    handles these tasks. It also handles similarly the touch events for individual 
    widgets in the same manner.

  Parameters:
    void
    
  Returns:
    laResult

  Remarks:
    
*/
laResult laEvent_ProcessEvents();

#endif /* LIBARIA_EVENT_H */