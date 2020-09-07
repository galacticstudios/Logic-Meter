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


#include "gfx/libaria/inc/libaria_editwidget.h"
#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_string.h"

static laResult startEdit(laEditWidget* edit) { return LA_FAILURE; }
static void endEdit(laEditWidget* edit) { }

static void clear(laEditWidget* edit) { }
static void accept(laEditWidget* edit) { }
static void set(laEditWidget* edit, laString str) { }
static void append(laEditWidget* edit, laString str) { }
static void backspace(laEditWidget* edit) { }

void _laEditWidget_Constructor(laEditWidget* edit)
{
    _laWidget_Constructor((laWidget*)edit);
    
    edit->widget.editable = LA_TRUE;
    
    edit->startEdit = &startEdit;
    edit->endEdit = &endEdit;
    
    edit->clear = &clear;
    edit->accept = &accept;
    edit->append = &append;
    edit->set = &set;
    edit->backspace = &backspace;
}

void _laEditWidget_Destructor(laEditWidget* edit)
{
    if(edit == NULL)
        return;
        
    _laWidget_Destructor((laWidget*)edit);
}

laResult laEditWidget_StartEdit()
{
    laEditWidget* edit = laContext_GetEditWidget();
    
    if(edit == NULL)
        return LA_FAILURE;
        
    return edit->startEdit(edit);
}

void laEditWidget_EndEdit()
{ 
    laEditWidget* edit = laContext_GetEditWidget();
    
    if(edit == NULL)
        return;
        
    edit->endEdit(edit);
}

void laEditWidget_Clear()
{ 
    laEditWidget* edit = laContext_GetEditWidget();
    
    if(edit == NULL)
        return;
        
    edit->clear(edit);
}

void laEditWidget_Accept()
{
    laEditWidget* edit = laContext_GetEditWidget();
    
    if(edit == NULL)
        return;
        
    edit->accept(edit);
}

void laEditWidget_Set(laString str)
{
    laEditWidget* edit = laContext_GetEditWidget();
    
    if(edit == NULL)
        return;
        
    edit->set(edit, str);
}

void laEditWidget_Append(laString str)
{
    laEditWidget* edit = laContext_GetEditWidget();
    
    if(edit == NULL)
        return;
        
    edit->append(edit, str);
}

void laEditWidget_Backspace()
{
    laEditWidget* edit = laContext_GetEditWidget();
    
    if(edit == NULL)
        return;
        
    edit->backspace(edit);
}