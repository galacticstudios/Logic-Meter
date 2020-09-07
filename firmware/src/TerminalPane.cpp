/* 
 * File:   TerminalPane.cpp
 * Author: Bob
 * 
 * Created on May 20, 2020, 10:47 AM
 */

extern "C"
{
#include "definitions.h"
}
#include "Display.h"
#include "TerminalPane.h"

TerminalPane::TerminalPane(bool forceBinary) : 
    _normalScheme(&NormalTextScheme), _inverseScheme(&InverseTextScheme), _dirtyAll(true),
    _forceBinary(forceBinary)
{
    // Figure out the number of columns and lines
    int32_t panelWidth = laWidget_GetWidth(GetWidget());
    int32_t panelHeight = laWidget_GetHeight(GetWidget());
    laString em = laString_CreateFromCharBuffer("M", &MonoFont);
    GFX_Rect emRect;
    laString_GetRect(&em, &emRect);
    laString_Destroy(&em);
    
    int32_t lines = panelHeight / emRect.height;
    int32_t columns = panelWidth / emRect.width;
    _rowHeight = emRect.height;
    _colWidth = emRect.width;
    _topMargin = (panelHeight - lines * _rowHeight) / 2;
    _leftMargin = (panelWidth - columns * _colWidth) / 2;
    
    if (_chars.empty())
    {
        _chars.resize(lines);
        for (size_t row = 0; row < _chars.size(); ++row)
        {
            _chars[row].resize(columns);
        }
    }
    else
        Invalidate();
    
    _cursorLine = _cursorColumn = 0;
}

TerminalPane::~TerminalPane() 
{
}

void TerminalPane::Invalidate()
{
    for (auto &row : _chars)
    {
        for (auto &col : row)
            col.dirty = true;
    }
}

laWidget *TerminalPane::GetWidget() const
{
    return TerminalPanel;
}

void TerminalPane::AddText(const char *data, size_t size)
{
    while (size--)
    {
        if (*data == '\r' && !_forceBinary)
            _cursorColumn = 0;
        
        else if (*data == '\n' && !_forceBinary)
            ++_cursorLine;
        
        else if (*data >= ' ' && *data <= '\x7e' && !_forceBinary)
        {
            _chars[_cursorLine][_cursorColumn].ch = *data;
            _chars[_cursorLine][_cursorColumn].inverse = false;
            _chars[_cursorLine][_cursorColumn].dirty = true;
            if (++_cursorColumn >= _chars[_cursorLine].size())
            {
                _cursorColumn = 0;
                ++_cursorLine;
            }
        }
        
        // Else (show the character as hexadecimal)
        else
        {
            int charCount = 2;
            // If the previous character on this line is hex, as indicated by its scheme,
            // we need to add a space
            if (_cursorColumn && _chars[_cursorLine][_cursorColumn - 1].inverse)
            {
                ++charCount;
            }
            // If the hex character won't fit
            if (_cursorColumn + charCount >= _chars[_cursorLine].size())
            {
                // Go to the next line. Back up the data, which will let us fall through,
                // handle any scrolling that needs to be done, and re-handle this character
                // on the next loop iteration
                ++_cursorLine;
                _cursorColumn = 0;
                --data;
                ++size;
            }
            // Else (the hex character will fit)
            else
            {
                static const char *hexDigits = "0123456789ABCDEF";
                if (charCount == 3)
                {
                    _chars[_cursorLine][_cursorColumn].ch = ' ';
                    _chars[_cursorLine][_cursorColumn].inverse = true;
                    _chars[_cursorLine][_cursorColumn].dirty = true;
                    ++_cursorColumn;
                }
                _chars[_cursorLine][_cursorColumn].ch = hexDigits[(*data >> 4) & 0xf];
                _chars[_cursorLine][_cursorColumn].inverse = true;
                _chars[_cursorLine][_cursorColumn].dirty = true;
                ++_cursorColumn;
                _chars[_cursorLine][_cursorColumn].ch = hexDigits[*data & 0xf];
                _chars[_cursorLine][_cursorColumn].inverse = true;
                _chars[_cursorLine][_cursorColumn].dirty = true;
                ++_cursorColumn;
            }
        }
        
        // Scroll up if necessary
        if (_cursorLine >= _chars.size())
        {
            // Move the text up
            for (size_t row = 0; row < _chars.size() - 1; ++row)
            {
                for (size_t col = 0; col < _chars[row].size(); ++col)
                {
                    if (_chars[row][col] != _chars[row + 1][col])
                    {
                        _chars[row][col] = _chars[row + 1][col];
                        _chars[row][col].dirty = true;
                    }
                }
            }
            
            // Clear the bottom row
            for (size_t col = 0; col < _chars.back().size(); ++col)
            {
                if (_chars.back()[col].ch != ' ' || _chars.back()[col].inverse)
                {
                    _chars.back()[col].ch = ' ';
                    _chars.back()[col].inverse = false;
                    _chars.back()[col].dirty = true;
                }
            }
            
            _cursorLine = _chars.size() - 1;
        }
        ++data;
    }
}

// Replace all the text in the pane with new text. This optimizes updating
// in that it doesn't repaint chars when the new char is identical to the old.
// But for this to work right, line endings must be \r\n.
// Also, the optimization isn't currently implemented for non-printed chars
// (which are shown as inverse hexadecimal). Also, this doesn't handle scrolling,
// so don't give it more text than will fit.
void TerminalPane::SetText(const char *data, size_t size)
{
    _cursorColumn = _cursorLine = 0;
    while (size--)
    {
        if (*data == '\r')
        {
            // Clear to the end of the line
            while (_cursorColumn < _chars[_cursorLine].size())
            {
                if (_chars[_cursorLine][_cursorColumn].ch != ' ' ||
                    _chars[_cursorLine][_cursorColumn].inverse)
                {
                    _chars[_cursorLine][_cursorColumn].ch = ' ';
                    _chars[_cursorLine][_cursorColumn].dirty = true;
                    _chars[_cursorLine][_cursorColumn].inverse = false;
                }
                ++_cursorColumn;
            }
            _cursorColumn = 0;
        }
        
        else if (*data == '\n')
            ++_cursorLine;
        
        else if (*data >= ' ' && *data <= '\x7e')
        {
            if (_chars[_cursorLine][_cursorColumn].ch != *data ||
                _chars[_cursorLine][_cursorColumn].inverse)
            {
                _chars[_cursorLine][_cursorColumn].ch = *data;
                _chars[_cursorLine][_cursorColumn].inverse = false;
                _chars[_cursorLine][_cursorColumn].dirty = true;
            }
            if (++_cursorColumn >= _chars[_cursorLine].size())
            {
                _cursorColumn = 0;
                ++_cursorLine;
            }
        }
        
        ++data;
    }
    
    // Clear the rest of the pane
    int clearLine = _cursorLine, clearColumn = _cursorColumn;
    do
    {
        while (clearColumn < _chars[clearLine].size())
        {
            if (_chars[clearLine][clearColumn].ch != ' ' ||
                _chars[clearLine][clearColumn].inverse)
            {
                _chars[clearLine][clearColumn].ch = ' ';
                _chars[clearLine][clearColumn].dirty = true;
                _chars[clearLine][clearColumn].inverse = false;
            }
            ++clearColumn;
        }
        clearColumn = 0;
    } while (++clearLine < _chars.size());
    
    Update();
}

void TerminalPane::Update()
{
    if (!laWidget_GetVisible(GetWidget()))
    {
        // Repaint everything next time we're visible
        _dirtyAll = true;
        return;
    }

    GFX_Begin();
    
    int32_t y = _topMargin;
    for (auto &row : _chars)
    {
        char toDraw[row.size() + 1];
        size_t toDrawCount = 0;
        int32_t toDrawX;
        bool toDrawInverse;
        
        int32_t x = _leftMargin;
        for (auto &col : row)
        {
            // If we need to repaint this character
            if (col.dirty || _dirtyAll)
            {
                // If we've already accumulated chars to redraw
                if (toDrawCount)
                {
                    // If they're a different color
                    if (toDrawInverse != col.inverse)
                    {
                        // Draw what we've accumulated
                        toDraw[toDrawCount] = '\0';
                        Draw(toDraw, toDrawX, y, toDrawInverse);
                        
                        // Start a new string
                        toDrawCount = 0;
                        toDrawInverse = col.inverse;
                        toDrawX = x;
                    }
                }
                
                // Else (starting a new string)
                else
                {
                    toDrawInverse = col.inverse;
                    toDrawX = x;
                }

                toDraw[toDrawCount++] = col.ch;
                col.dirty = false;
            }
            
            // Else (char doesn't need to be painted)
            else
            {
                // Draw what we've accumulated
                if (toDrawCount)
                {
                    // Draw what we've accumulated
                    toDraw[toDrawCount] = '\0';
                    Draw(toDraw, toDrawX, y, toDrawInverse);
                    toDrawCount = 0;
                }
            }
            
            x += _colWidth;
        }

        // Draw what we've accumulated
        if (toDrawCount)
        {
            toDraw[toDrawCount] = '\0';
            Draw(toDraw, toDrawX, y, toDrawInverse);
        }

        y += _rowHeight;
    }
    GFX_End();
    
    _dirtyAll = false;
}

void TerminalPane::Draw(const char *text, int32_t x, int32_t y, bool inverse)
{
    // We sometimes shrink the height of this pane (e.g. in ToolGPS when we show the
    // output instead of the map). So check whether we're painting within bounds
    int32_t height = laWidget_GetHeight(GetWidget());
    if (y >= height)
        return;

    int32_t offsetX = laWidget_GetX(GetWidget()), offsetY = laWidget_GetY(GetWidget());
        laString str = laString_CreateFromCharBuffer(text, &MonoFont);
    int len = strlen(text);

    GFX_Set(GFXF_DRAW_COLOR, inverse ? _inverseScheme->background : _normalScheme->background);
    GFX_DrawRect(x + offsetX, y + offsetY, _colWidth * len, _rowHeight);
    GFX_Set(GFXF_DRAW_COLOR, inverse ? _inverseScheme->text : _normalScheme->text);

    laString_DrawClipped(&str, x + offsetX, y + offsetY, 
        _colWidth * len, _rowHeight,
        x + offsetX, y + offsetY, NULL);
    laString_Destroy(&str);
}
