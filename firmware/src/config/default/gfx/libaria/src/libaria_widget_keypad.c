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


#include "gfx/libaria/inc/libaria_widget_keypad.h"

#if LA_KEYPAD_WIDGET_ENABLED && LA_BUTTON_WIDGET_ENABLED

#include "gfx/libaria/inc/libaria_context.h"
#include "gfx/libaria/inc/libaria_string.h"
#include "gfx/libaria/inc/libaria_utils.h"
#include "gfx/libaria/inc/libaria_widget_button.h"

#define DEFAULT_WIDTH           200
#define DEFAULT_HEIGHT          200

static laButtonWidget* _getButton(laKeyPadWidget* pad,
                                  uint32_t row,
                                  uint32_t col)
{
    laButtonWidget* btn;
    
    if(pad == NULL ||
       row >= pad->rows ||
       col >= pad->cols)
        return NULL;
       
    btn = (laButtonWidget*)laWidget_GetChildAtIndex((laWidget*)pad, (row * pad->cols) + col);
    
    if(btn == NULL)
        return NULL;
        
    return btn;
}

static void _resizeCells(laKeyPadWidget* pad)
{
    uint32_t width, height, wrem, hrem, row, col, wmod, hmod;
    int32_t x, y;
    laButtonWidget* btn;
    
    if(pad->widget.children.size == 0)
        return;
    
    x = 0;
    y = 0;
    width = pad->widget.rect.width / pad->cols;
    height = pad->widget.rect.height / pad->rows;
    
    wrem = pad->widget.rect.width - (width * pad->cols);
    hrem = pad->widget.rect.height - (height * pad->rows);
    
    hmod = 0;
    wmod = 0;
    
    // resize and position all button cells
    for(row = 0; row < pad->rows; row++)
    {
        if(row < hrem)
            hmod = 1;
        else
            hmod = 0;    
           
        for(col = 0; col < pad->cols; col++)
        {
            btn = (laButtonWidget*)laWidget_GetChildAtIndex((laWidget*)pad,
                                                            (row * pad->cols) + col);
                          
            if(btn == NULL)
                return;
                                                            
            laWidget_SetPosition((laWidget*)btn, x, y);
            
            if(col < wrem)
                wmod = 1;
            else
                wmod = 0;
                
            laWidget_SetSize((laWidget*)btn, width + wmod, height + hmod);
            
            x += width + wmod;
        }
        
        x = 0;
        
        y += height + hmod;
    }
}

static void _resized(laKeyPadWidget* pad)
{
    _resizeCells(pad);
}

static void _buttonClicked(laButtonWidget* btn)
{
    int32_t id, row, col;
    laKeyPadWidget* pad = (laKeyPadWidget*)btn->widget.parent;
    
    if(pad == NULL)
        return;
    
    if ((laButtonWidget_GetPressed(btn) == LA_TRUE && 
         pad->trigger == LA_KEYPAD_TRIGGER_KEYRELEASED) ||
        (laButtonWidget_GetPressed(btn) == LA_FALSE && 
         pad->trigger == LA_KEYPAD_TRIGGER_KEYPRESSED))
        return;
        
    id = laWidget_GetIndexOfChild(btn->widget.parent, (laWidget*)btn);
    
    row = id / pad->cols;
    col = id % pad->cols; 
    
    if(pad->clickEvt != NULL)
        pad->clickEvt(pad, btn, row, col);
    //printf("%i, %i\n", row, col);
    
    if(pad->cells[id].action == LA_KEYPAD_CELL_ACTION_APPEND)
        laEditWidget_Append(pad->cells[id].value);
    else if(pad->cells[id].action == LA_KEYPAD_CELL_ACTION_SET)
        laEditWidget_Set(pad->cells[id].value);
    else if(pad->cells[id].action == LA_KEYPAD_CELL_ACTION_BACKSPACE)
        laEditWidget_Backspace();
    else if(pad->cells[id].action == LA_KEYPAD_CELL_ACTION_CLEAR)
        laEditWidget_Clear();
    else if(pad->cells[id].action == LA_KEYPAD_CELL_ACTION_ACCEPT)
        laEditWidget_Accept();
}

static void languageChanging(laKeyPadWidget* pad)
{
    uint32_t i;
    
    for(i = 0; i < pad->rows * pad->cols; i++)
    {
        if(&pad->cells[i].button->text != NULL)
        {
            laWidget_Invalidate((laWidget*)pad);
            
            return;
        }
    }
}

void _laKeyPadWidget_Constructor(laKeyPadWidget* pad, uint32_t rows, uint32_t cols)
{
    laButtonWidget* button;
    uint32_t i;
    
    _laWidget_Constructor((laWidget*)pad);
    
    pad->widget.destructor = (laWidget_Destructor_FnPtr)&_laKeyPadWidget_Destructor;

    pad->widget.languageChangeEvent = (laWidget_LanguageChangingEvent_FnPtr)&languageChanging;

    pad->widget.invalidateBorderAreas = (laWidget_InvalidateBorderAreas_FnPtr)&_laKeyPadWidget_InvalidateBorderAreas;

    pad->widget.type = LA_WIDGET_KEYPAD;

    // override base class methods
    pad->widget.paint = (laWidget_Paint_FnPtr)&_laKeyPadWidget_Paint;
    pad->widget.resized = (laWidget_Resized_FnPtr)&_resized;

    pad->widget.rect.width = DEFAULT_WIDTH;
    pad->widget.rect.height = DEFAULT_HEIGHT;

    pad->widget.borderType = LA_WIDGET_BORDER_NONE;
    pad->widget.backgroundType = LA_WIDGET_BACKGROUND_NONE;
    
    pad->rows = rows;
    pad->cols = cols;
    
    pad->trigger = LA_KEYPAD_TRIGGER_KEYRELEASED;
    
    pad->cells = laContext_GetActive()->memIntf.heap.calloc(rows * cols, sizeof(laKeyPadCell));
    
    for(i = 0; i < rows * cols; i++)
    {
        // create button
        button = laButtonWidget_New();
        
        if(button == NULL)
            return;
            
        laButtonWidget_SetPressedEventCallback(button,
                                                (laButtonWidget_PressedEvent)_buttonClicked);
        laButtonWidget_SetReleasedEventCallback(button,
                                                (laButtonWidget_ReleasedEvent)_buttonClicked);        
        
        laWidget_AddChild((laWidget*)pad, (laWidget*)button);
        
        // create string
        laString_Initialize(&pad->cells[i].value);
    }
    
    _resizeCells(pad);
}

void _laKeyPadWidget_Destructor(laKeyPadWidget* pad)
{
    uint32_t i;
    
    for(i = 0; i < pad->rows * pad->cols; i++)
    {
        laWidget_Delete((laWidget*)pad->cells[i].button);
        laString_Destroy(&pad->cells[i].value);
    }
    
    laContext_GetActive()->memIntf.heap.free(pad->cells);
    
    _laWidget_Destructor((laWidget*)pad);
}

laKeyPadWidget* laKeyPadWidget_New(uint32_t rows, uint32_t cols)
{
    laKeyPadWidget* pad = NULL;

    if(laContext_GetActive() == NULL)
        return NULL;

    pad = laContext_GetActive()->memIntf.heap.calloc(1, sizeof(laKeyPadWidget));
    
    _laKeyPadWidget_Constructor(pad, rows, cols);

    return pad;
}

laKeyPadActionTrigger laKeyPadWidget_GetKeyPadActionTrigger(laKeyPadWidget* pad)
{
    if(pad == NULL)
        return LA_KEYPAD_TRIGGER_KEYRELEASED;

    return pad->trigger;
}

laResult laKeyPadWidget_SetKeyPadActionTrigger(laKeyPadWidget* pad,
                                               laKeyPadActionTrigger trigger)
{
    if(pad == NULL)
        return LA_FAILURE;

    pad->trigger = trigger;
    
    return LA_SUCCESS;
}

laKeyPadWidget_KeyClickEvent laKeyPadWidget_GetKeyClickEventCallback(laKeyPadWidget* pad)
{
    if(pad == NULL)
        return NULL;

    return pad->clickEvt;
}

laResult laKeyPadWidget_SetKeyClickEventCallback(laKeyPadWidget* pad,
                                                 laKeyPadWidget_KeyClickEvent cb)
{
    if(pad == NULL)
        return LA_FAILURE;

    pad->clickEvt = cb;
    
    return LA_SUCCESS;
}

laBool laKeyPadWidget_GetKeyEnabled(laKeyPadWidget* pad,
                                    uint32_t row,
                                    uint32_t col)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_FALSE;
        
    return laWidget_GetVisible((laWidget*)_getButton(pad, row, col));
}
                                                     
laResult laKeyPadWidget_SetKeyEnabled(laKeyPadWidget* pad,
                                      uint32_t row,
                                      uint32_t col,
                                      laBool enabled)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_FAILURE;
        
    laWidget_SetVisible((laWidget*)_getButton(pad, row, col), enabled);
    
    return LA_SUCCESS;
}

laResult laKeyPadWidget_GetKeyText(laKeyPadWidget* pad,
                                   uint32_t row,
                                   uint32_t col,
                                   laString* str)
{
    
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_FAILURE;
        
    laButtonWidget_GetText(_getButton(pad, row, col), str);
    
    return LA_SUCCESS;
}
                                               
laResult laKeyPadWidget_SetKeyText(laKeyPadWidget* pad,
                                   uint32_t row,
                                   uint32_t col,
                                   laString str)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_FAILURE;
        
    laButtonWidget_SetText(_getButton(pad, row, col), str);
    
    return LA_SUCCESS;
}
                                               
GFXU_ImageAsset* laKeyPadWidget_GetKeyPressedImage(laKeyPadWidget* pad,
                                                   uint32_t row,
                                                   uint32_t col)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return NULL;
        
    return laButtonWidget_GetPressedImage(_getButton(pad, row, col));
}
                                               
laResult laKeyPadWidget_SetKeyPressedImage(laKeyPadWidget* pad,
                                           uint32_t row,
                                           uint32_t col,
                                           GFXU_ImageAsset* img)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_FAILURE;
        
    laButtonWidget_SetPressedImage(_getButton(pad, row, col), img);
    
    return LA_SUCCESS;
}
                                               
GFXU_ImageAsset* laKeyPadWidget_GetKeyReleasedImage(laKeyPadWidget* pad,
                                                    uint32_t row,
                                                    uint32_t col)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return NULL;
        
    return laButtonWidget_GetReleasedImage(_getButton(pad, row, col));
}
                                               
laResult laKeyPadWidget_SetKeyReleasedImage(laKeyPadWidget* pad,
                                            uint32_t row,
                                            uint32_t col,
                                            GFXU_ImageAsset* img)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_FAILURE;
        
    laButtonWidget_SetReleasedImage(_getButton(pad, row, col), img);
    
    return LA_SUCCESS;
}
                                               
laRelativePosition laKeyPadWidget_GetKeyImagePosition(laKeyPadWidget* pad,
                                                      uint32_t row,
                                                      uint32_t col)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return 0;
        
    return laButtonWidget_GetImagePosition(_getButton(pad, row, col));
}
                                               
laResult laKeyPadWidget_SetKeyImagePosition(laKeyPadWidget* pad,
                                            uint32_t row,
                                            uint32_t col,
                                            laRelativePosition pos)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_FAILURE;
        
    laButtonWidget_SetImagePosition(_getButton(pad, row, col), pos);
    
    return LA_SUCCESS;
}
                                               
uint32_t laKeyPadWidget_GetKeyImageMargin(laKeyPadWidget* pad,
                                          uint32_t row,
                                          uint32_t col)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return 0;
        
    return laButtonWidget_GetImageMargin(_getButton(pad, row, col));
}
                                               
laResult laKeyPadWidget_SetKeyImageMargin(laKeyPadWidget* pad,
                                          uint32_t row,
                                          uint32_t col,
                                          uint32_t mg)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_FAILURE;
        
    laButtonWidget_SetImageMargin(_getButton(pad, row, col), mg);
    
    return LA_SUCCESS;
}

laBackgroundType laKeyPadWidget_GetKeyBackgroundType(laKeyPadWidget* pad,
                                                     uint32_t row,
                                                     uint32_t col)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_WIDGET_BACKGROUND_NONE;
        
    return laWidget_GetBackgroundType((laWidget*)_getButton(pad, row, col));
}
                                                     
laResult laKeyPadWidget_SetKeyBackgroundType(laKeyPadWidget* pad,
                                             uint32_t row,
                                             uint32_t col,
                                             laBackgroundType type)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_FAILURE;
        
    return laWidget_SetBackgroundType((laWidget*)_getButton(pad, row, col), type);
}

laKeyPadCellAction laKeyPadWidget_GetKeyAction(laKeyPadWidget* pad,
                                               uint32_t row,
                                               uint32_t col)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return 0;
    
    return pad->cells[row * pad->cols + col].action;
}
                                               
laResult laKeyPadWidget_SetKeyAction(laKeyPadWidget* pad,
                                     uint32_t row,
                                     uint32_t col,
                                     laKeyPadCellAction action)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_FAILURE;
        
    pad->cells[row * pad->cols + col].action = action;
        
    return LA_SUCCESS;
}

laString* laKeyPadWidget_GetKeyValue(laKeyPadWidget* pad,
                                     uint32_t row,
                                     uint32_t col)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return NULL;
    
    return laString_New(&pad->cells[row * pad->cols + col].value);
}
                                               
laResult laKeyPadWidget_SetKeyValue(laKeyPadWidget* pad,
                                    uint32_t row,
                                    uint32_t col,
                                    laString str)
{
    if(pad == NULL || row * pad->cols + col > pad->rows * pad->cols)
        return LA_FAILURE;
        
    if(laString_Compare(&pad->cells[row * pad->cols + col].value, &str) == 0)
        return LA_FAILURE;
        
    laString_Copy(&pad->cells[row * pad->cols + col].value, &str);
    
    laWidget_Invalidate((laWidget*)pad);
    
    return LA_SUCCESS;
}
                                                    
#endif // LA_WIDGET_KEYPAD_ENABLED && LA_WIDGET_BUTTON_ENABLED