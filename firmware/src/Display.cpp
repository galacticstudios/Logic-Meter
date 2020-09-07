/* 
 * File:   Display.cpp
 * Author: Bob
 * 
 * Created on April 24, 2020, 12:58 PM
 */

extern "C" 
{
#include "definitions.h"
}
#include "Utility.h"
#include "Display.h"
#include "Pane.h"
#include "Menu.h"

Display display;

#define titleFont RegularFont
#define menuFont RegularFont

Display::Display() :
    _currentPane(NULL)
{
}

void Display::Initialize()
{
    // Just in case we left one of the panels visible, hide them all
    laScreen *screen = laContext_GetActiveScreen();
    // Is there a better way to get the layer?
    laWidget *layer = (laWidget *) screen->layers[0];
    // Go through all the children
    for (uint32_t count = laWidget_GetChildCount(layer); count != 0; )
    {
        laWidget *child = laWidget_GetChildAtIndex(layer, --count);
        // If it's a pane, as indicated by its height
        if (laWidget_GetHeight(child) > 150)
            laWidget_SetVisible(child, LA_FALSE);
    }
    
//    laWidget_SetCornerRadius((laWidget *) MenuKeypad, 5);
}

void Display::SetTitle(const char *text)
{
    laString ariaTitle = laString_CreateFromUTF8Buffer(text, &titleFont);
    laLabelWidget_SetText(TitleLabel, ariaTitle); 
    laString_Destroy(&ariaTitle);
}

void Display::SetStatus(const char *text)
{
    laString ariaStatus = laString_CreateFromUTF8Buffer(text, &titleFont);
    laLabelWidget_SetText(StatusLabel, ariaStatus); 
    laString_Destroy(&ariaStatus);
}

void Display::SetMenu(const Menu &menu)
{
    for (size_t i = 0; i < countof(menu.MenuItems()); ++i)
    {
        laString las = laString_CreateFromUTF8Buffer(menu.MenuItems()[i].Text(), &menuFont);
        laKeyPadWidget_SetKeyText(MenuKeypad, 0, i, las);
        laString_Destroy(&las);
    }
}

void Display::SetPane(Pane &pane)
{
    if (&pane != _currentPane)
    {
        ClearPane();
        pane.Show(true);
        _currentPane = &pane;
        pane.Update();
    }
}

void Display::ClearPane()
{
    if (_currentPane)
    {
        _currentPane->Show(false);
        _currentPane = NULL;
    }
}

