/* 
 * File:   TerminalPainter.h
 * Author: Bob
 *
 * Created on September 15, 2020, 7:37 PM
 */

#ifndef TERMINALPAINTER_H
#define	TERMINALPAINTER_H

#include <list>
#include <vector>
#include "SurfacePainter.h"

class TerminalPainter : public SurfacePainter
{
public:
    TerminalPainter(std::list<std::vector<char> > &chars, int32_t xOffset = 0, int32_t yOffset = 0) : 
        SurfacePainter(xOffset, yOffset), _chars(chars), _firstDisplayLine(_chars.begin()),
        _normalScheme(&NormalTextScheme), _inverseScheme(&InverseTextScheme),
        _top(0), _left(0) {}
    virtual ~TerminalPainter() {}
    
    void SetMargins(int32_t top, int32_t left) {_top = top; _left = left;}
    void SetLineCount(uint32_t lineCount) {_lineCount = lineCount;}
    void SetLineHeight(uint32_t height) {_rowHeight = height;}
    void SetColumnWidth(uint32_t width) {_columnWidth = width;}
    
    void SetFirstDisplayLine(std::list<std::vector<char> >::iterator firstDisplayLine)
    {_firstDisplayLine = firstDisplayLine;}
    
    bool OnDraw(SurfaceWrapper *surface, GFX_Rect *bounds);

    // Puts flags on the characters 0-9A-F to indicate how to paint them
    static char InverseHexChar(char c) {return c | 0x80;}
    typedef enum {PaintNormal, PaintInverse} PaintInstructions;
    static char DecodeInverse(char c, PaintInstructions *instructions)
    {
        if (c & 0x80)
        {
            c &= 0x7f;
            *instructions = PaintInverse;
        }
        else
            *instructions = PaintNormal;
        return c;
    }
    
private:
    TerminalPainter(const TerminalPainter& orig);
    
    // Returns length in pixels of text drawn
    int32_t Draw(SurfaceWrapper *surface, char *text, int32_t x, int32_t y, bool inverse);
    
    std::list<std::vector<char> > &_chars;
    std::list<std::vector<char> >::iterator _firstDisplayLine;
    int32_t _top, _left;
    uint32_t _lineCount, _columnWidth;
    uint32_t _rowHeight;
    laScheme *_normalScheme, *_inverseScheme;
};

#endif	/* TERMINALPAINTER_H */

