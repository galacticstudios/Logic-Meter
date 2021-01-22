/* 
 * File:   TerminalPane.h
 * Author: Bob
 *
 * Created on May 20, 2020, 10:47 AM
 */

#ifndef TERMINALPANE_H
#define	TERMINALPANE_H

#include <vector>
#include <list>
#include "Pane.h"
#include "TerminalPainter.h"

struct laScheme_t;
typedef struct laScheme_t laScheme;

#define BUFFER_LINES 30

class TerminalPane : public Pane
{
public:
    // forceBinary causes all characters to be displayed as hex digits
    TerminalPane(bool forceBinary = false, laDrawSurfaceWidget *terminalDrawSurface = TerminalDrawSurface);
    virtual ~TerminalPane();
    
    void Update();
    
    void AddText(const char *data, size_t size);
    // Replace all the text in the pane with new text. This optimizes updating
    // in that it doesn't repaint chars when the new char is identical to the old.
    // But for this to work right, line endings must be \r\n.
    // Also, the optimization isn't currently implemented for non-printed chars
    // (which are shown as inverse hexadecimal). Also, this doesn't handle scrolling,
    // so don't give it more text than will fit.
    void SetText(const char *data, size_t size);
    void Clear();
    
    void ScrollUp();
    void ScrollDown();
    
protected:
    virtual laWidget *GetWidget() const;
    
private:
    TerminalPane(const TerminalPane& orig);
    
    bool ShowingLastLine() const;
    
    void NewLine();
    
    int _displayLines, _displayColumns;
    std::list<std::vector<char> > _chars;
    std::list<std::vector<char> >::iterator _firstDisplayLine;
    uint16_t _cursorColumn;
    int32_t _topMargin, _leftMargin;
    bool _forceBinary;

    SurfaceWrapper _terminalWidget;    
    TerminalPainter _terminalPainter;
};

#endif	/* TERMINALPANE_H */

