/* 
 * File:   Menu.h
 * Author: Bob
 *
 * Created on April 24, 2020, 11:29 AM
 */

#ifndef MENU_H
#define	MENU_H

#include "Utility.h"

class Menu;
class Tool;

typedef enum {NoChange, ChildMenu, SiblingMenu, ParentMenu} MenuType;

class MenuItem
{
public:
    
    MenuItem(const char *text = "", MenuType menuType = NoChange, 
            const Menu *menu = NULL, const Callback *execute = NULL) :
        _text(text), _nextMenuType(menuType), _nextMenu(menu), _execute(execute)
    {
    }
    ~MenuItem() {delete _execute;}
    
    const char *Text() const {return _text;}
    MenuType NextMenuType() const {return _nextMenuType;}
    const Menu *NextMenu() const {return _nextMenu;}
    void Execute(Tool *tool) const 
    {
        if (_execute) 
        {
            Callback *execute = (Callback *) _execute;
            (*execute)(tool);
        }
    }
    
private:
    const char *_text;
    MenuType _nextMenuType;
    const Menu *_nextMenu;
    const Callback *_execute;
};

typedef MenuItem MenuItemArray[5];

class Menu 
{
public:
    Menu(const MenuItemArray &menuItems) :
        _menuItems(menuItems)
    {
    }
    ~Menu() {}
    
    const MenuItemArray &MenuItems() const {return _menuItems;}
    
private:
    Menu(const Menu& orig);
    
    const MenuItemArray &_menuItems;
};

#endif	/* MENU_H */

