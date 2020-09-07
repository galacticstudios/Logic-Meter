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


#include "gfx/libaria/inc/libaria_widget_textfield.h"

#if LA_TEXTFIELD_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_layer.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget.h"

#define DEFAULT_WIDTH           100
#define DEFAULT_HEIGHT          25

#define DEFAULT_CURSOR_TIME     1000

static laResult startEdit(laTextFieldWidget* txt);
static void endEdit(laTextFieldWidget* txt);
static void clear(laTextFieldWidget* txt);
static void accept(laTextFieldWidget* txt);
static void append(laTextFieldWidget* txt, laString str);
static void set(laTextFieldWidget* txt, laString str);
static void backspace(laTextFieldWidget* txt);

static void invalidateText(laTextFieldWidget* txt)
{
    GFX_Rect textRect, drawRect;
    
    _laTextFieldWidget_GetTextRect(txt, &textRect, &drawRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)txt),
                          &drawRect,
                          GFX_FALSE);
}

static void invalidateCursor(laTextFieldWidget* txt)
{
    GFX_Rect cursorRect;
    
    _laTextFieldWidget_GetCursorRect(txt, &cursorRect);
    
    laLayer_AddDamageRect(laUtils_GetLayer((laWidget*)txt),
                          &cursorRect,
                          GFX_FALSE);
}

static void _setCursorFromPoint(laTextFieldWidget* txt, GFX_Point* pnt)
{
    GFX_Rect textRect;
    
    laString_GetRect(&txt->text, &textRect);
    
    laUtils_ArrangeRectangleRelative(&textRect,
                                     GFX_Rect_Zero,
                                     txt->editWidget.widget.rect,
                                     txt->halign,
                                     LA_VALIGN_MIDDLE,
                                     0,
                                     txt->editWidget.widget.margin.left,
                                     txt->editWidget.widget.margin.top,
                                     txt->editWidget.widget.margin.right,
                                     txt->editWidget.widget.margin.bottom,
                                     0);
    
    laUtils_RectToScreenSpace((laWidget*)txt, &textRect);
    
    laTextFieldWidget_SetCursorPosition(txt, laString_GetCharIndexAtPoint(&txt->text, pnt->x - txt->editWidget.widget.rect.x));
}

void _laTextFieldWidget_Constructor(laTextFieldWidget* txt)
{
    _laEditWidget_Constructor((laEditWidget*)txt);
    
    txt->editWidget.widget.destructor = (laWidget_Destructor_FnPtr)&_laTextFieldWidget_Destructor;

    txt->editWidget.widget.type = LA_WIDGET_TEXTFIELD;

    // override base class methods
    txt->editWidget.widget.update = (laWidget_Update_FnPtr)&_laTextFieldWidget_Update;
    txt->editWidget.widget.paint = (laWidget_Paint_FnPtr)&_laTextFieldWidget_Paint;
    txt->editWidget.widget.focusGained = (laWidget_Focus_FnPtr)&_laTextFieldWidget_FocusGained;
    txt->editWidget.widget.focusLost = (laWidget_Focus_FnPtr)&_laTextFieldWidget_FocusLost;
    txt->editWidget.widget.touchDown = (laWidget_TouchDownEvent_FnPtr)&_laTextFieldWidget_TouchDownEvent;
    txt->editWidget.widget.touchUp = (laWidget_TouchUpEvent_FnPtr)&_laTextFieldWidget_TouchUpEvent;
    txt->editWidget.widget.touchMoved = (laWidget_TouchMovedEvent_FnPtr)&_laTextFieldWidget_TouchMovedEvent;

    // override edit class methods
    txt->editWidget.startEdit = (laEditWidget_StartEdit_FnPtr)&startEdit;
    txt->editWidget.endEdit = (laEditWidget_EndEdit_FnPtr)&endEdit;
    txt->editWidget.clear = (laEditWidget_Clear_FnPtr)&clear;
    txt->editWidget.accept = (laEditWidget_Accept_FnPtr)&accept;
    txt->editWidget.append = (laEditWidget_Append_FnPtr)&append;
    txt->editWidget.set = (laEditWidget_Set_FnPtr)&set;
    txt->editWidget.backspace = (laEditWidget_Backspace_FnPtr)&backspace;

    txt->editWidget.widget.rect.width = DEFAULT_WIDTH;
    txt->editWidget.widget.rect.height = DEFAULT_HEIGHT;
    
    txt->editWidget.widget.borderType = LA_WIDGET_BORDER_BEVEL;

    laString_Initialize(&txt->text);
    
    txt->halign = LA_HALIGN_LEFT;
    txt->cursorEnable = LA_TRUE;
    txt->cursorDelay = DEFAULT_CURSOR_TIME;
    txt->clearOnFirstEdit = LA_TRUE;
}

void _laTextFieldWidget_Destructor(laTextFieldWidget* txt)
{
    laString_Destroy(&txt->text);
    
    // free any existing memory reader
    if(txt->reader != NULL)
    {
        laContext_GetActive()->memIntf.heap.free(txt->reader);
        
        txt->reader = NULL;
    }

    _laEditWidget_Destructor((laEditWidget*)txt);
}

laWidgetUpdateState _laTextFieldWidget_Update(laTextFieldWidget* txt, uint32_t dt)
{
    if(laContext_GetActive()->edit == (laEditWidget*)txt)
    {
        txt->cursorTime += dt;
        
        if(txt->cursorTime > txt->cursorDelay)
        {
            txt->cursorVisible = !txt->cursorVisible;
            
            //printf("%u\n", txt->cursorTime);
            
            txt->cursorTime = 0;
            
            invalidateCursor(txt);
        }
    }
    
    return LA_WIDGET_UPDATE_STATE_DONE;
}

laTextFieldWidget* laTextFieldWidget_New()
{
    laTextFieldWidget* txt = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    txt = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laTextFieldWidget));

    _laTextFieldWidget_Constructor(txt);

    return txt;
}

laHAlignment laTextFieldWidget_GetAlignment(laTextFieldWidget* txt)
{
    if(txt == NULL)
        return 0;

    return txt->halign;
}

laResult laTextFieldWidget_SetAlignment(laTextFieldWidget* txt, laHAlignment align)
{
    if(txt == NULL)
        return LA_FAILURE;

    if(txt->halign == align)
        return LA_SUCCESS;

    txt->halign = align;

    laWidget_Invalidate((laWidget*)txt);
    
    return LA_SUCCESS;
}

uint32_t laTextFieldWidget_GetCursorDelay(laTextFieldWidget* txt)
{
    if(txt == NULL)
        return 0;

    return txt->cursorDelay;
}

laResult laTextFieldWidget_SetCursorDelay(laTextFieldWidget* txt, uint32_t dt)
{
    if(txt == NULL || txt->cursorDelay == dt)
        return LA_FAILURE;

    txt->cursorDelay = dt;

    if(txt->cursorEnable == LA_TRUE && txt->cursorVisible == LA_TRUE)
        invalidateCursor(txt);
        
    return LA_SUCCESS;
}

laBool laTextFieldWidget_GetCursorEnabled(laTextFieldWidget* txt)
{
    if(txt == NULL)
        return LA_FALSE;

    return txt->cursorEnable;
}

laResult laTextFieldWidget_SetCursorEnabled(laTextFieldWidget* txt, laBool en)
{
    if(txt == NULL || txt->cursorEnable == en)
        return LA_FAILURE;

    txt->cursorEnable = en;

    if(txt->cursorEnable == LA_TRUE && txt->cursorVisible == LA_TRUE)
        invalidateCursor(txt);
        
    return LA_SUCCESS;
}

uint32_t laTextFieldWidget_GetCursorPosition(laTextFieldWidget* txt)
{
    if(txt == NULL)
        return 0;

    return txt->cursorPos;
}

laResult laTextFieldWidget_SetCursorPosition(laTextFieldWidget* txt, uint32_t pos)
{
    if(txt == NULL || txt->cursorPos == pos)
        return LA_FAILURE;

    txt->cursorPos = pos;

    if(txt->cursorVisible == LA_TRUE)
        invalidateCursor(txt);
        
    return LA_SUCCESS;
}

laResult laTextFieldWidget_GetText(laTextFieldWidget* txt, laString* str)
{
    if(txt == NULL)
        return LA_FAILURE;
    
    laString_Copy(str, &txt->text);
    
    return LA_SUCCESS;
}

laResult laTextFieldWidget_SetText(laTextFieldWidget* txt, laString str)
{
    if(txt == NULL)
        return LA_FAILURE;
    
    if(laString_Compare(&txt->text, &str) == 0)
        return LA_SUCCESS;
    
    invalidateText(txt);
    
    if(str.table_index != LA_STRING_NULLIDX)
        laString_ExtractFromTable(&txt->text, str.table_index);
    else
        laString_Copy(&txt->text, &str);
    
    txt->cursorPos = txt->text.length;
    
    invalidateText(txt);
    
    return LA_SUCCESS;
}

laResult laTextFieldWidget_SetClearOnFirstEdit(laTextFieldWidget* txt, laBool clear)
{
    if(txt == NULL || txt->clearOnFirstEdit == clear)
        return LA_FAILURE;

    txt->clearOnFirstEdit = clear;

    return LA_SUCCESS;
}

void _laTextFieldWidget_FocusGained(laWidget* widget)
{
    laTextFieldWidget* txt = (laTextFieldWidget*) widget;
            
    laContext_SetEditWidget(widget);
    
    if (txt->focusChangedEvent != NULL)
        txt->focusChangedEvent(txt, LA_TRUE);
}

void _laTextFieldWidget_FocusLost(laWidget* widget)
{
    laTextFieldWidget* txt = (laTextFieldWidget*) widget;
            
    if (txt->focusChangedEvent != NULL)
        txt->focusChangedEvent(txt, LA_FALSE);
}

laTextFieldWidget_TextChangedCallback laTextFieldWidget_GetTextChangedEventCallback(laTextFieldWidget* txt)
{
    if(txt == NULL)
        return NULL;

    return txt->textChangedEvent;
}

laResult laTextFieldWidget_SetTextChangedEventCallback(laTextFieldWidget* txt,
                                                       laTextFieldWidget_TextChangedCallback cb)
{
    if(txt == NULL || txt->textChangedEvent == cb)
        return LA_FAILURE;

    txt->textChangedEvent = cb;

    return LA_SUCCESS;
}

laTextFieldWidget_FocusChangedCallback laTextFieldWidget_GetFocusChangedEventCallback(laTextFieldWidget* txt)
{
    if(txt == NULL)
        return NULL;

    return txt->focusChangedEvent;
}

laResult laTextFieldWidget_SetFocusChangedEventCallback(laTextFieldWidget* txt,
                                                       laTextFieldWidget_FocusChangedCallback cb)
{
    if(txt == NULL || txt->focusChangedEvent == cb)
        return LA_FAILURE;

    txt->focusChangedEvent = cb;

    return LA_SUCCESS;
}

static laResult startEdit(laTextFieldWidget* txt)
{
    txt->cursorTime = 0;
    txt->cursorVisible = LA_TRUE;

    if (txt->clearOnFirstEdit == LA_TRUE)
    {
        invalidateText(txt);
        
        laString_Clear(&txt->text);
        txt->clearOnFirstEdit  = LA_FALSE;
    }
    else
        invalidateCursor(txt);
    
    return LA_SUCCESS;
}

static void endEdit(laTextFieldWidget* txt)
{
    txt->cursorTime = 0;
    txt->cursorVisible = LA_FALSE;
    
    invalidateCursor(txt);
}

static void clear(laTextFieldWidget* txt)
{
    invalidateText(txt);
    invalidateCursor(txt);
    
    laString_Clear(&txt->text);

    if (txt->textChangedEvent != NULL)
        txt->textChangedEvent(txt);
}

static void accept(laTextFieldWidget* txt)
{
    laContext_SetEditWidget(NULL);
}

static void append(laTextFieldWidget* txt, laString str)
{
    // initial string value is probably referencing string table
    // extract it to local cache and to make editable
    if(txt->text.table_index != LA_STRING_NULLIDX)
        laString_ExtractFromTable(&txt->text, txt->text.table_index);

    invalidateText(txt);
    invalidateCursor(txt);
    
    laString_Insert(&txt->text, &str, txt->cursorPos);
    
    txt->cursorPos += laString_Length(&str);
    
    if(txt->cursorEnable == LA_TRUE)
    {
        txt->cursorTime = 0;
        txt->cursorVisible = LA_TRUE;
    }
    
    invalidateText(txt);
    invalidateCursor(txt);

    if (txt->textChangedEvent != NULL)
        txt->textChangedEvent(txt);
}

static void set(laTextFieldWidget* txt, laString str)
{
    invalidateText(txt);
    invalidateCursor(txt);
    
    laString_ExtractFromTable(&txt->text, str.table_index);
    
    txt->cursorPos = txt->text.length;
    
    if(txt->cursorEnable == LA_TRUE)
    {
        txt->cursorTime = 0;
        txt->cursorVisible = LA_TRUE;
    }

    invalidateText(txt);
    invalidateCursor(txt);

    if (txt->textChangedEvent != NULL)
        txt->textChangedEvent(txt);
}

static void backspace(laTextFieldWidget* txt)
{
    if(txt->cursorPos == 0)
        return;
        
    invalidateText(txt);
    invalidateCursor(txt);
        
    laString_Remove(&txt->text, txt->cursorPos - 1, 1);
    
    if(txt->cursorPos > txt->text.length)
        txt->cursorPos = txt->text.length;
    else
        txt->cursorPos--;
        
    if(txt->cursorEnable == LA_TRUE)
    {
        txt->cursorTime = 0;
        txt->cursorVisible = LA_TRUE;
    }

    invalidateText(txt);
    invalidateCursor(txt);

    if (txt->textChangedEvent != NULL)
        txt->textChangedEvent(txt);
}

void _laTextFieldWidget_TouchDownEvent(laTextFieldWidget* txt,
                                       laInput_TouchDownEvent* evt)
{
    GFX_Point pnt;
    
    pnt.x = evt->x;
    pnt.y = evt->y;
    
    _setCursorFromPoint(txt, &pnt);
    
    evt->event.accepted = LA_TRUE;
}

void _laTextFieldWidget_TouchUpEvent(laTextFieldWidget* txt,
                                     laInput_TouchUpEvent* evt)
{
    evt->event.accepted = LA_TRUE;
}

void _laTextFieldWidget_TouchMovedEvent(laTextFieldWidget* txt,
                                        laInput_TouchMovedEvent* evt)
{
    /*GFX_Point pnt;
    
    pnt.x = evt->x;
    pnt.y = evt->y;
    
    _setCursorFromPoint(txt, &pnt);*/
    
    evt->event.accepted = LA_TRUE;
}

#endif // LA_TEXTFIELD_WIDGET_ENABLED
