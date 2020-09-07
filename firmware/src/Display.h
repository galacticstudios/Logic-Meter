/* 
 * File:   Display.h
 * Author: Bob
 *
 * Created on April 24, 2020, 12:58 PM
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

class Menu;
class Pane;

#define RegularFont Arial
#define MonoFont Consolas

#define RGB(r, g, b) (((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3))

class Display {
public:
    Display();
    ~Display() {}

    void Initialize();
    void SetTitle(const char *text);
    void SetStatus(const char *text);
    void SetPane(Pane &pane);
    void ClearPane();
    void SetMenu(const Menu &menu);
    
private:
    Display(const Display& orig);
    Pane *_currentPane;
};

extern Display display;

#endif	/* DISPLAY_H */

