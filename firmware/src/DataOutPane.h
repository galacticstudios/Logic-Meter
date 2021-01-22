/* 
 * File:   UtilityPane.h
 * Author: Bob
 *
 * Created on September 23, 2020, 1:29 PM
 */

#ifndef DATAOUTPANE_H
#define	DATAOUTPANE_H

#include "Pane.h"
#include "TerminalPane.h"

class DataOutPane : public Pane
{
public:
    DataOutPane(bool forceBinary = false);
    virtual ~DataOutPane();
    
    void SetDataName(const char *dataName);
    void SetStatus(const char *status);
    
    void Update() {_terminalPane.Update();}
    
    void AddText(const char *data, size_t size) {_terminalPane.AddText(data, size);}
    // Replace all the text in the pane with new text. This optimizes updating
    // in that it doesn't repaint chars when the new char is identical to the old.
    // But for this to work right, line endings must be \r\n.
    // Also, the optimization isn't currently implemented for non-printed chars
    // (which are shown as inverse hexadecimal). Also, this doesn't handle scrolling,
    // so don't give it more text than will fit.
    void SetText(const char *data, size_t size) {_terminalPane.SetText(data, size);}
    void Clear() {_terminalPane.Clear();}
    
    void ScrollUp() {_terminalPane.ScrollUp();}
    void ScrollDown() {_terminalPane.ScrollDown();}
    
protected:
    virtual laWidget *GetWidget() const;
    
private:
    DataOutPane(const DataOutPane& orig);
    
    TerminalPane _terminalPane;
};

#endif	/* UTILITYPANE_H */

