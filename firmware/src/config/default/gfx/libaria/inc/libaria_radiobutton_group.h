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
    libaria_radiobutton_group.h

  Summary:
    

  Description:
    This module implements functions to control a radio button group.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_RADIOBUTTON_GROUP_H
#define LIBARIA_RADIOBUTTON_GROUP_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"

#if LA_RADIOBUTTON_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_array.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************


typedef struct laRadioButtonGroup_t laRadioButtonGroup;
typedef struct laRadioButtonWidget_t laRadioButtonWidget;


// *****************************************************************************
/*  Structure
        laRadioButtonGroup_t

    Summary:
        Defines the structure used for the Radio Button group.
        
    Description:
        Defines the parameters required for a Radio Button group.
        Marks the current selected Radio button within the group
        
    Remarks:
        None.
        
*/
typedef struct laRadioButtonGroup_t
{
    laArray buttonList;

    laBool initialized;
    laRadioButtonWidget* selected;

} laRadioButtonGroup;

// *****************************************************************************
// *****************************************************************************
// Section: Routines
// *****************************************************************************
// *****************************************************************************

/*    Function:
        laResult laRadioButtonGroup_Create(laRadioButtonGroup** grp)
 
    Summary:
        This function creates a GFX_GOL_RADIOBUTTON group with the
        provided button list. 

    Description:
        This function creates a GFX_GOL_RADIOBUTTON group with the
        given pointer and the button list provided within the laRadioButtonGroup object.

    Parameters:
    laRadioButtnGroup **
    
    Returns:
        laResult    

*/
LIB_EXPORT laResult laRadioButtonGroup_Create(laRadioButtonGroup** grp);

/*    Function:
        void laRadioButtonGroup_Destroy(laRadioButtonGroup* grp)
 
    Summary:
        This function destroys the GFX_GOL_RADIOBUTTON group 

    Description:
        This function destroys he GFX_GOL_RADIOBUTTON group with the
        given pointer. It frees the memory allocated to the button group and clears the button list.
        
    Parameters:
        laRadioButtnGroup *
    
    Returns:
        void

*/
LIB_EXPORT void laRadioButtonGroup_Destroy(laRadioButtonGroup* grp);

/*    Function:
        laResult laRadioButtonGroup_AddButton(laRadioButtonGroup* grp,
                                                 laRadioButtonWidget* btn)
 
    Summary:
        Add a button widget to the button list of the selected Radio button group.

    Description:
        Add a button widget to the button list of the selected Radio button group.
        The function makes sure the radio button grp is valid and the button widget to be added is not already a part of the group.
        The button is then added as the last button in the group button list
        
    Parameters:
        laRadioButtnGroup *
        laRadioButtonWidget*
    
    Returns:
        laResult

*/
LIB_EXPORT laResult laRadioButtonGroup_AddButton(laRadioButtonGroup* grp,laRadioButtonWidget* btn);

/*    Function:
        laResult laRadioButtonGroup_RemoveButton(laRadioButtonGroup* grp,
                                                    laRadioButtonWidget* btn);
 
    Summary:
        Remove a button widget to the button list of the selected Radio button group.

    Description:
        Remove a button widget to the button list of the selected Radio button group.
        The function makes sure the radio button grp is valid and the button widget to be removed is a part of the group.
        The button is then removed properly making sure to handle the list correctly. If the list size is 0, the group is destroyed.
        
    Parameters:
        laRadioButtnGroup *
        laRadioButtonWidget*
    
    Returns:
        laResult

*/                                                 
LIB_EXPORT laResult laRadioButtonGroup_RemoveButton(laRadioButtonGroup* grp,
                                                    laRadioButtonWidget* btn);

/*    Function:
        laResult laRadioButtonGroup_SelectButton(laRadioButtonGroup* grp,
                                                    laRadioButtonWidget* btn)
 
    Summary:
        Select the button widget specified from the button list for the Radio button group.

    Description:
        Select the button widget specified from the button list for the Radio button group.
        The function makes sure the specified button widget is a part of the group.
        It deselects the currently selected button widget and reassigns the focus to the button widget specified.
        
    Parameters:
        laRadioButtnGroup *
        laRadioButtonWidget*
    
    Returns:
        laResult

*/                                                    
LIB_EXPORT laResult laRadioButtonGroup_SelectButton(laRadioButtonGroup* grp,
                                                    laRadioButtonWidget* btn);
                                                    

#endif // LA_RADIOBUTTON_WIDGET_ENABLED
#endif /* LIBARIA_RADIOBUTTON_GROUP_H */