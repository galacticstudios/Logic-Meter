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
    libaria_editwidget.h

  Summary:
    

  Description:
    This module implements the routines to enable edit of library widgets.
*******************************************************************************/

// DOM-IGNORE-BEGIN

#ifndef LIBARIA_EDITWIDGET_H
#define LIBARIA_EDITWIDGET_H
//DOM-IGNORE-END

#include "gfx/libaria/inc/libaria_common.h"
#include "gfx/libaria/inc/libaria_widget.h"

// DOM-IGNORE-BEGIN
typedef struct laEditWidget_t laEditWidget;
typedef struct laString_t laString;

typedef laResult (*laEditWidget_StartEdit_FnPtr)(laEditWidget*);
typedef void (*laEditWidget_EndEdit_FnPtr)(laEditWidget*);
typedef void (*laEditWidget_Clear_FnPtr)(laEditWidget*);
typedef void (*laEditWidget_Accept_FnPtr)(laEditWidget*);
typedef void (*laEditWidget_Set_FnPtr)(laEditWidget*, laString);
typedef void (*laEditWidget_Append_FnPtr)(laEditWidget*, laString);
typedef void (*laEditWidget_Backspace_FnPtr)(laEditWidget*);
// DOM-IGNORE-END

/* Structure:
        laEditWidget_t

    Summary:
        Specifies the edit widget structure to manage all properties and events
        associated with edit widgets

    Description:
        Edit widgets are a subset of normal widgets that are capable of receiving
        edit events from the UI kernel.  Specialized widgets are capable of
        broadcasting edit events and the active edit event will react to them.

    Remarks:
        None.
        
*/
typedef struct laEditWidget_t
{
    laWidget widget;
    
    laEditWidget_StartEdit_FnPtr startEdit;
    laEditWidget_EndEdit_FnPtr endEdit;
    
    laEditWidget_Clear_FnPtr clear;
    laEditWidget_Accept_FnPtr accept;
    laEditWidget_Set_FnPtr set;
    laEditWidget_Append_FnPtr append;
    laEditWidget_Backspace_FnPtr backspace;
} laEditWidget;

// DOM-IGNORE-BEGIN
void _laEditWidget_Constructor(laEditWidget* edit);
void _laEditWidget_Destructor(laEditWidget* edit);

laResult laEditWidget_StartEdit();
void laEditWidget_EndEdit();

void laEditWidget_Clear();
void laEditWidget_Accept();
void laEditWidget_Set(laString str);
void laEditWidget_Append(laString str);
void laEditWidget_Backspace();
// DOM-IGNORE-END

#endif // LIBARIA_EDITWIDGET_H
