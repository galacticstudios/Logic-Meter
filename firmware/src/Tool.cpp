/* 
 * File:   Tool.cpp
 * Author: Bob
 * 
 * Created on April 24, 2020, 11:26 AM
 */

#include <string>
#include "Tool.h"
#include "Display.h"
#include "Menu.h"
#include "Utility.h"
#include "Pane.h"


static const MenuItem helpMenuItems[5] = {
    MenuItem(), 
    MenuItem(), 
    MenuItem(), 
    MenuItem(), 
    MenuItem("Done", MenuType::ParentMenu, NULL, CB(&Tool::ExitHelp))};

const Menu helpMenu(helpMenuItems);

Tool::Tool(const char *title, Pane *pane, const Menu &menu, const Help &help) :
    _title(title), _pane(pane), _menu(menu), _requestDeactivate(false),
    _menuStackPointer(0), _help(help), _inHelp(false)
{
    _requestDeactivate = false;
    display.SetTitle(_title);
    SetStatusText("");
    display.SetMenu(_menu);
    display.SetPane(*_pane); 
    _menuStackPointer = 0;
    _menuStack[0] = &_menu;
}

Tool::~Tool() 
{
    if (_inHelp)
        ExitHelp();
    display.ClearPane();
    
    PrimaryD0_InputEnable();
    PrimaryD11_InputEnable();
    PrimaryF4_InputEnable();
   
    Aux1D1_InputEnable();
    Aux1D9_InputEnable();
    Aux1D10_InputEnable();
    Aux1F5_InputEnable();
    
    Aux2D4_InputEnable();
    Aux2D5_InputEnable();
    Aux2F0_InputEnable();
    Aux2F1_InputEnable();
}

void Tool::SetStatusText(const char *text)
{
    display.SetStatus(text);
}

// Button index = 1..5
void Tool::OnButtonPressed(int buttonIndex)
{
    const Menu *currentMenu = _menuStack[_menuStackPointer];
    
    // If the Help button was pressed
    if (buttonIndex >= countof(currentMenu->MenuItems()))
    {
        if (_inHelp)
        {
            // Simulate pressing the "Done" button
            buttonIndex = 4;
        }
        // Else (help screen is not currently displayed)
        else
        {
            _menuStack[++_menuStackPointer] = &helpMenu;
            display.SetMenu(*_menuStack[_menuStackPointer]);
            EnterHelp();
            return;
        }
    }
    const MenuItem &selectedItem = currentMenu->MenuItems()[buttonIndex];
    
    switch (selectedItem.NextMenuType())
    {
        case NoChange :
            break;
            
        case ChildMenu :
            _menuStack[++_menuStackPointer] = selectedItem.NextMenu();
            break;
            
        case SiblingMenu :
            _menuStack[_menuStackPointer] = selectedItem.NextMenu();
            break;
            
        case ParentMenu :
            ASSERT(_menuStackPointer != 0);
            --_menuStackPointer;
            break;
    }
    
    if (_menuStack[_menuStackPointer] != currentMenu)
        display.SetMenu(*_menuStack[_menuStackPointer]);
    selectedItem.Execute(this);
}

void Tool::EnterHelp()
{
    static laString *newline;
    static uint32_t lineWidth;
    if (newline == NULL)
    {
        newline = laString_New(NULL);
        *newline = laString_CreateFromCharBuffer("\n", &RegularFont);
    
        lineWidth = laWidget_GetWidth((laWidget *) HelpTextWidget);
        laMargin margin;
        laWidget_GetMargin((laWidget *) HelpTextWidget, &margin);
        lineWidth -= margin.left + margin.right;
    }
    
    GetPane()->SetVisible(false);
    _inHelp = true;
    
    // Assemble the help text
    std::string text = std::string("P: ") + _help.Primary() + "\n";
    if (_help.Aux1())
    {
        text = text + "A: " + _help.Aux1() + "\n";
        if (_help.Aux2())
        {
            text = text + "B: " + _help.Aux2() + "\n";
        }
        else
            text += "B: Unused\n";
    }
    else
        text += "A & B: Unused\n";
    text = text + "\n" + _help.Text();
    
    // Wrap the text.
    laString s = laString_CreateFromUTF8Buffer(text.c_str(), &RegularFont);
    uint32_t len = laString_Length(&s);
    uint32_t lastWordStartIndex = 0, lastWordStartX = 0;
    uint32_t x = 0;

    // Go through all the characters in the string
    for (uint32_t charIndex = 0; charIndex < len; ++charIndex)
    {
        GFXU_CHAR ch = laString_CharAt(&s, charIndex);
        uint32_t charWidth = laString_GetCharWidth(&s, charIndex);
        if (ch == '\n')
        {
            lastWordStartIndex = charIndex + 1;
            lastWordStartX = 0;
            x = 0;
        }
        
        else if (ch == ' ')
        {
            lastWordStartIndex = charIndex + 1;
            x += charWidth;
            lastWordStartX = x;
        }
        
        else if (x + charWidth >= lineWidth)
        {
            // Replace the last word break with a newline
            laString_Remove(&s, lastWordStartIndex - 1, 1);
            laString_Insert(&s, newline, lastWordStartIndex - 1);
            x = x - lastWordStartX + charWidth;
            lastWordStartX = 0;
        }
        
        else
            x += charWidth;
    }
    
    laLabelWidget_SetText(HelpTextWidget, s);
    laString_Destroy(&s);
    laWidget_SetVisible(HelpPanel, LA_TRUE);
    
}

void Tool::ExitHelp()
{
    laWidget_SetVisible(HelpPanel, LA_FALSE);
    GetPane()->SetVisible(true);
    _inHelp = false;
}
