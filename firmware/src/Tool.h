/* 
 * File:   Tool.h
 * Author: Bob
 *
 * Created on April 24, 2020, 11:26 AM
 */

#ifndef TOOL_H
#define	TOOL_H

#include "Help.h"

class Pane;
class Menu;

class Tool 
{
public:
    Tool(const char *title, Pane *pane, const Menu &menu, const Help &help);
    virtual ~Tool();
    
    // Tell the tool that we want to deactivate it. This can come in asynchronously
    // (e.g. from an interrupt) and it's an indication that any running task or screen
    // update should be terminated
    void RequestDeactivate() {_requestDeactivate = true;}
    
    // Button index = 1..5. This function alters the menu and calls the menu's function as needed
    void OnButtonPressed(int buttonIndex);
    
    // Called when there's time available
    virtual void OnIdle() {}
    
    void ScrollUp();
    void ScrollDown();
    void ExitHelp();

protected:
    void SetStatusText(const char *text);
    bool DeactivateRequested() const {return _requestDeactivate;}
    Pane *GetPane() const {return _pane;}
    
    bool InHelp() const {return _inHelp;}
    
private:
    Tool(const Tool& orig);

    void EnterHelp();
    
    const char *_title;
    Pane *_pane;
    const Menu &_menu;
    const Menu *_menuStack[10];
    int _menuStackPointer;
    bool _requestDeactivate;
    bool _inHelp;
    
    const Help &_help;
};

#endif	/* TOOL_H */

