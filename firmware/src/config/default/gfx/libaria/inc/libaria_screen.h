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
    libaria_screen.h

  Summary:
    A screen describes the state of a set of layers.  It can be orthogonally
    rotated and its life-cycle can be configured.

*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_SCREEN_H
#define LIBARIA_SCREEN_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"
#include "gfx/libaria/inc/libaria_string.h"

typedef struct laContext_t laContext;
typedef struct laLayer_t laLayer;
typedef struct laScreen_t laScreen;

// *****************************************************************************
/* Type:
    laScreen_CreateCallback_FnPtr

  Summary:
    Callback pointer for a new screen create event notification.  This is called
    when the library attempts to create a screen.  
*/
typedef void (*laScreen_CreateCallback_FnPtr)(laScreen*);

// *****************************************************************************
/* Type:
    laScreen_ShowHideCallback_FnPtr

  Summary:
    Callback pointer for the active screen show or hide event change notification.

  Description:
    Callback pointer for the active screen show or hide event change notification.

  Remarks:
    
*/
typedef void (*laScreen_ShowHideCallback_FnPtr)(laScreen*);

/*
Enumeration:
        laScreenOrientation_t

    Summary:
        Possible values for screen orientation.

    Description:
        Possible values for screen orientation.

    Remarks:
        None.
 */
typedef enum laScreenOrientation_t
{
    LA_SCREEN_ORIENTATION_0   = 0x0,
    LA_SCREEN_ORIENTATION_90,
    LA_SCREEN_ORIENTATION_180,
    LA_SCREEN_ORIENTATION_270
} laScreenOrientation;

/*
Structure:
        laScreen_t

    Summary:
        The structure to maintain the screen related variables
        and event handling

    Description:
        Maintains the layers associated with the screen. 
        Marks the screen as persistent or not, which either destroys the 
        screen when changed or preserves it for future reloading.
        Allocates and manages the event handling when screen 
        change / show / hide events occur.

    Remarks:
        None.
 */
typedef struct laScreen_t
{
    uint32_t id;   // the id of the screen
    laString name; // the name of the screen
     
    laBool persistent; // indicates that the screen should not free its widgets
                       // when it hides
    laScreen_CreateCallback_FnPtr createCB; // the function that is called to
                                            // create the contents of the
                                            // screen
                                            
    
    laBool created;                         // indicates if the screen currently
                                            // exists

    laLayer* layers[LA_MAX_LAYERS];         // the layer array for the screen
    
    laScreenOrientation orientation;        // the orientation of the screen
    laBool mirrored;                        // the mirror flag of the screen
    
    laBool layerSwapSync;                   // the layerSwapSync flag of the screen
    
    laScreen_ShowHideCallback_FnPtr showCB; // a callback that is called when the
                                            // screen is shown
    
    laScreen_ShowHideCallback_FnPtr hideCB; // a callback that is called when the
                                            // screen is hidden
} laScreen;

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

/*    Function:
        laScreen* laScreen_New(laBool persistent, 
                               laBool createAtStartup,
                               laScreen_CreateCallback_FnPtr cb)
 
    Summary:
        Create a new screen, initialize it to the values specified.
        

    Description:
        Create a new screen, initialize it to the values specified.
        The key properties to specify include screen persistence, 
        call backs for screen creation, initialize the screen to default
        values either specified through MHGC or manually by user.  

    Parameters:
        laBool persistent - indicates that the screen should not free 
                            the memory of its layers when it is hidden
                            
        laBool createAtStartup - indicates that the screen should be created
                                 as soon as possible to make its widgets
                                 accessible to the application
                                 
        laScreen_CreateCallback_FnPtr cb - the function that should be called
                                           to initialize the screen at a later
                                           time
        
    Returns:
        void    

*/
LIB_EXPORT laScreen* laScreen_New(laBool persistent,
                                  laBool createAtStartup,
                                  laScreen_CreateCallback_FnPtr cb);

/* Function:
        void laScreen_Delete(laScreen* scr)
 
    Summary:
        Frees all memory for all layers and widgets for this screen
        
    Description:
        
    Parameters:
        laScreen* scr - the screen to destroy
        
    Returns:
        void

*/
LIB_EXPORT void laScreen_Delete(laScreen* scr);

/*    Function:
        laResult laScreen_Show(laScreen* scr)
 
    Summary:
        Make the specified screen active and show it on the display
        
    Description:
        The function makes sure that the specified screen is not already active, 
        creates it if it is is not already created, sets the appropriate
        color mode,  make it active and call the show callback function
        pointer.
        
    Parameters:
        laScreen* scr - the screen to show
        
    Returns:
        void    

*/
LIB_EXPORT GFX_DEPRECATED laResult laScreen_Show(laScreen* scr);

/*    Function:
        laResult laScreen_Hide(laScreen* scr)
 
    Summary:
        Hide the currently active screen
        
        This function has been deprecated in favor of laContext_SetActiveScreen
        
    Description:
        The function makes sure that the specified screen is currently active, 
        hides the screen by calling the hide callback function pointer, if
        the persistent flag is not marked for that screen, delete the screen
        and free memory. Reset or turn off the Layers allocated for the screen.
        
    Parameters:
        laScreen* scr - the screen to hide
        
    Returns:
        laResult    

*/
LIB_EXPORT GFX_DEPRECATED laResult laScreen_Hide(laScreen* scr);

/*  Function:
        laResult laScreen_SetLayer(laScreen* scr, uint32_t idx, laLayer* layer)
        
    Summary:
        Assigns the provided layer pointer to the screen at the given index
        
        This function has been deprecated in favor of laContext_SetActiveScreen
        
    Description:
        Screens contain an internal list of layer pointers.  This API assigns
        a layer to a screen.  If the screen is currently active the library
        attempts to immediately enable the new layer in the HAL.
        
    Parameters:
        laScreen* scr - the screen to modify
        uint32_t idx - the index of the layer
        laLayer* layer - the layer pointer to assign to the screen
        
    Returns:
        laResult - the result of the operation

*/
LIB_EXPORT laResult laScreen_SetLayer(laScreen* scr, uint32_t idx, laLayer* layer);

/*  Function:
        int32_t laScreen_GetLayerIndex(laScreen* scr, laLayer* layer)
 
    Summary:
        Returns the index of the layer for the screen specified.
        
    Description:
        Returns the index of the layer for the screen specified.
        
    Parameters:
        laScreen* scr - the screen to reference
        laLayer* layer - the layer to search for
        
    Returns:
        uint32_t - the index of the layer

*/
LIB_EXPORT int32_t laScreen_GetLayerIndex(laScreen* scr, laLayer* layer);

/*  Function:
        laScreenOrientation laScreen_GetOrientation(laScreen* scr)
 
    Summary:
        Returns the orientation object associated with the specified screen
        
    Description:
        Returns the orientation object associated with the specified screen
        
    Parameters:
        laScreen* scr - the screen to reference
        
    Returns:
        laScreenOrientation - the screen orientation

*/
LIB_EXPORT laScreenOrientation laScreen_GetOrientation(laScreen* scr);

/*  Function:
        laResult laScreen_SetOrientation(laScreen* scr, laScreenOrientation ori)
 
    Summary:
        Sets the orientation object to the specified screen
        
    Description:
        Sets the orientation object to the specified screen
        
    Parameters:
        laScreen* scr - the screen to modify
        laScreenOrientation - the new orientation setting
        
    Returns:
        laResult - the result of the operation

*/
LIB_EXPORT laResult laScreen_SetOrientation(laScreen* scr, laScreenOrientation ori);

/*  Function:
        laBool laScreen_GetMirrored(laScreen* scr)
 
    Summary:
        Returns the mirror setting for the specified screen
        
    Description:
        Returns the mirror setting for the specified screen
        
    Parameters:
        laScreen* scr - the screen to reference
        
    Returns:
        laBool - the mirror setting

*/
LIB_EXPORT laBool laScreen_GetMirrored(laScreen* scr);

/*  Function:
        laResult laScreen_SetMirrored(laScreen* scr, laBool mirr)
 
    Summary:
        Sets the mirror setting for the specified screen
        
    Description:
        Sets the mirror setting for the specified screen
        
    Parameters:
        laScreen* scr - the screen to modify
        laBool - the mirror setting
        
    Returns:
        laResult - the result of the operation

*/
LIB_EXPORT laResult laScreen_SetMirrored(laScreen* scr, laBool mirr);

/*  Function:
        laBool laScreen_GetLayerSwapSync(laScreen* scr)
 
    Summary:
        Returns the layer swap sync setting for the specified screen
        
    Description:
        Returns the layer swap sync setting for the specified screen
        
    Parameters:
        laScreen* scr - the screen to reference
        
    Returns:
        laBool - the sync setting

*/
LIB_EXPORT laBool laScreen_GetLayerSwapSync(laScreen* scr);

/*  Function:
        laResult laScreen_SetLayerSwapSync(laScreen* scr, laBool sync)
 
    Summary:
        Sets the layer swap sync setting for the specified screen
        
    Description:
        Layer synchronization allows for the configuration timing of the
        buffer swap chain.  In the case where multiple layers are being
        modified at the same time, it is often desirable to have the
        updates appear on the display at the same time.  Layer sync will
        gate all layer swapping until all dirty layers have finished
        drawing.  All layers will then swap at same time.
        
    Parameters:
        laScreen* scr - the screen to modify
        laBool - the sync setting
        
    Returns:
        laResult - the result of the operation

*/
LIB_EXPORT laResult laScreen_SetLayerSwapSync(laScreen* scr, laBool sync);

/*  Function:
        laScreen_ShowHideCallback_FnPtr laScreen_GetShowEventCallback(laScreen* scr)
 
    Summary:
        Returns the show call back event function pointer for the specified screen
        
    Description:
        Returns the show call back event function pointer for the specified screen
        
    Parameters:
        laScreen* scr - the screen to reference
                
    Returns:
        laScreen_ShowHideCallback_FnPtr

*/
LIB_EXPORT laScreen_ShowHideCallback_FnPtr laScreen_GetShowEventCallback(laScreen* scr);

/*  Function:
        laResult laScreen_SetShowEventCallback(laScreen* scr,
                                                  laScreen_ShowHideCallback_FnPtr cb)
 
    Summary:
        Set the show call back event function pointer for the specified screen
        
    Description:
        Set the show call back event function pointer for the specified screen
        
    Parameters:
        laScreen* scr - the screen to modify
        laScreen_ShowHideCallback_FnPtr - the function pointer to use
        
    Returns:
        laResult - the result of the operation

*/
LIB_EXPORT laResult laScreen_SetShowEventCallback(laScreen* scr,
                                                  laScreen_ShowHideCallback_FnPtr cb);
                                                  
/*  Function:
        aScreen_ShowHideCallback_FnPtr laScreen_GetHideEventCallback(laScreen* scr)
 
    Summary:
        Returns the hide call back event function pointer for the specified screen
        
    Description:
        Returns the hide call back event function pointer for the specified screen
        
    Parameters:
        laScreen* scr - the screen to reference
                
    Returns:
        laScreen_ShowHideCallback_FnPtr    

*/
LIB_EXPORT laScreen_ShowHideCallback_FnPtr laScreen_GetHideEventCallback(laScreen* scr);

/*  Function:
        laResult laScreen_SetHideEventCallback(laScreen* scr,
                                                  laScreen_ShowHideCallback_FnPtr cb)
 
    Summary:
        Set the hide call back event function pointer for the specified screen
        
    Description:
        Set the hide call back event function pointer for the specified screen
        
    Parameters:
        laScreen* scr - the screen to modify
        laScreen_ShowHideCallback_FnPtr
        
    Returns:
        laResult    

*/
LIB_EXPORT laResult laScreen_SetHideEventCallback(laScreen* scr,
                                                  laScreen_ShowHideCallback_FnPtr cb);
                                                  

#endif /* LIBARIA_SCREEN_H */