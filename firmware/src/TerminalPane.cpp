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

TerminalPane::TerminalPane(bool forceBinary, laDrawSurfaceWidget *terminalDrawSurface) : 
    _forceBinary(forceBinary), _cursorColumn(0), _terminalPainter(_chars),
    _terminalWidget(terminalDrawSurface)
{
    // Figure out the number of columns and lines
    int32_t panelWidth = laWidget_GetWidth(_terminalWidget.GetSurface());
    int32_t panelHeight = laWidget_GetHeight(_terminalWidget.GetSurface());
    laString em = laString_CreateFromCharBuffer("M", &MonoFont);
    GFX_Rect emRect;
    laString_GetRect(&em, &emRect);
    laString_Destroy(&em);
    
    uint32_t rowHeight = emRect.height;
    uint32_t colWidth = emRect.width + 1;
    _displayLines = panelHeight / rowHeight;
    _displayColumns = panelWidth / colWidth;
    _topMargin = (panelHeight - _displayLines * rowHeight) / 2;
    _leftMargin = (panelWidth - _displayColumns * colWidth) / 2;
    
    // Create the first line for the buffer
    _chars.push_back(std::vector<char>(_displayColumns, ' '));
    
    _terminalPainter.SetMargins(_topMargin, _leftMargin);
    _terminalPainter.SetLineCount(_displayLines);
    _terminalPainter.SetLineHeight(rowHeight);
    _terminalPainter.SetColumnWidth(colWidth);
    _firstDisplayLine = _chars.begin();
    _terminalPainter.SetFirstDisplayLine(_firstDisplayLine);
    
    _terminalWidget.Register(&_terminalPainter);
}

TerminalPane::~TerminalPane() 
{
}


laWidget *TerminalPane::GetWidget() const
{
    return _terminalWidget.GetSurface()->parent;
}

void TerminalPane::AddText(const char *data, size_t size)
{
    while (size--)
    {
        if (*data == '\r' && !_forceBinary)
            _cursorColumn = 0;
        
        else if (*data == '\n' && !_forceBinary)
            NewLine();
        
        else if (*data >= ' ' && *data <= '\x7e' && !_forceBinary)
        {
            _chars.back()[_cursorColumn] = *data;
            if (++_cursorColumn >= _chars.back().size())
            {
                _cursorColumn = 0;
                NewLine();
            }
        }
        
        // Else (show the character as hexadecimal)
        else
        {
            // If the hex character won't fit
            if (_cursorColumn + 2 >= _chars.back().size())
            {
                // Go to the next line. Back up the data, which will let us fall through,
                // handle any scrolling that needs to be done, and re-handle this character
                // on the next loop iteration
                NewLine();
                _cursorColumn = 0;
                --data;
                ++size;
            }
            // Else (the hex character will fit)
            else
            {
                static const char *hexDigits = "0123456789ABCDEF";
                _chars.back()[_cursorColumn++] = TerminalPainter::InverseHexChar(hexDigits[(*data >> 4) & 0xf]);
                _chars.back()[_cursorColumn++] = TerminalPainter::InverseHexChar(hexDigits[*data & 0xf]);
            }
        }

        ++data;
    }
    
    _terminalWidget.Invalidate();
}

void TerminalPane::Clear()
{
    _cursorColumn = 0;
    _chars.clear();
    _chars.push_back(std::vector<char>(_displayColumns, ' '));
    _firstDisplayLine = _chars.begin();
    _terminalPainter.SetFirstDisplayLine(_firstDisplayLine);
    _terminalWidget.Invalidate();
}

// Replace all the text in the pane with new text.
void TerminalPane::SetText(const char *data, size_t size)
{
    Clear();
    AddText(data, size);
    
    Update();
}

void TerminalPane::NewLine()
{
    bool showingLastLine = ShowingLastLine();
    if (_chars.size() < BUFFER_LINES)
    {
        _chars.push_back(std::vector<char>(_displayColumns, ' '));
    }
    else
    {
        // If we're displaying the top line in the buffer
        if (_firstDisplayLine == _chars.begin())
        {
            ScrollDown();
        }
        // Move the top line in the buffer to the bottom
        _chars.splice(_chars.end(), _chars, _chars.begin());
        // And clear it
        memset(_chars.back().data(), ' ', _chars.back().size());
    }
    if (showingLastLine && _chars.size() > _displayLines)
    {
        _terminalPainter.SetFirstDisplayLine(++_firstDisplayLine);
        _terminalWidget.Invalidate();
    }
}

void TerminalPane::ScrollUp()
{
    if (_firstDisplayLine != _chars.begin())
    {
        _terminalPainter.SetFirstDisplayLine(--_firstDisplayLine);
        _terminalWidget.Invalidate();
    }
}

void TerminalPane::ScrollDown()
{
    if (!ShowingLastLine())
    {
        // Scroll down
        _terminalPainter.SetFirstDisplayLine(++_firstDisplayLine);
        _terminalWidget.Invalidate();
    }
}

bool TerminalPane::ShowingLastLine() const
{
    // Figure out if we're currently displaying the last line(s) of the buffer
    auto line = _chars.end();
    int countBack = _displayLines;
    while (countBack--)
    {
        if (--line == _firstDisplayLine)
            return true;
    }
    
    return false;
}

void TerminalPane::Update()
{
}
