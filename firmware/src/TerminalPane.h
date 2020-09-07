/* 
 * File:   TerminalPane.h
 * Author: Bob
 *
 * Created on May 20, 2020, 10:47 AM
 */

#ifndef TERMINALPANE_H
#define	TERMINALPANE_H

#include <vector>
#include "Pane.h"

struct laScheme_t;
typedef struct laScheme_t laScheme;

class TerminalPane : public Pane
{
public:
    // forceBinary causes all characters to be displayed as hex digits
    TerminalPane(bool forceBinary = false);
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
    
    void Invalidate();
    
protected:
    virtual laWidget *GetWidget() const;
    
private:
    TerminalPane(const TerminalPane& orig);
    void Draw(const char *text, int32_t x, int32_t y, bool inverse);
    
    struct Char
    {
        Char() : ch(' '), dirty(false), inverse(false) {}
        bool operator==(const Char &r) {return ch == r.ch && inverse == r.inverse;}
        bool operator!=(const Char &r) {return !(*this == r);}
        char ch;
        bool dirty;
        bool inverse;
    };
    std::vector<std::vector<Char> > _chars;
    int _cursorLine, _cursorColumn;
    int32_t _topMargin, _leftMargin;
    int32_t _rowHeight, _colWidth;
    laScheme *_normalScheme, *_inverseScheme;
    bool _dirtyAll;
    bool _forceBinary;
};

#endif	/* TERMINALPANE_H */

