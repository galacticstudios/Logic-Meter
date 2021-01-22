/* 
 * File:   TerminalPainter.cpp
 * Author: Bob
 * 
 * Created on September 15, 2020, 7:37 PM
 */

extern "C" 
{
#include "definitions.h"
#include "gfx/libaria/inc/libaria_utils.h"
}
#include "TerminalPainter.h"
#include "SurfaceWrapper.h"
#include "Display.h"

bool TerminalPainter::OnDraw(SurfaceWrapper *surface, GFX_Rect *bounds)
{
    int32_t y = _top;

    auto line = _firstDisplayLine;
    for (uint32_t i = 0; i < _lineCount && line != _chars.end(); ++i, ++line)
    {
        char toDraw[line->size() + 1];
        size_t toDrawCount = 0;
        int32_t toDrawX = _left;
        PaintInstructions accumulatedInstructions = PaintNormal; // only care about Normal/not Normal
        
        // Go through each character in the line
        for (auto &col : *line)
        {
            PaintInstructions charInstructions;
            volatile char charToDraw = col;
            charToDraw = DecodeInverse(charToDraw, &charInstructions);
            // If we've already accumulated chars to draw
            if (toDrawCount)
            {
                // If they're a different color
                if ((charInstructions == PaintNormal) != (accumulatedInstructions == PaintNormal))
                {
                    // Draw what we've accumulated
                    toDraw[toDrawCount] = '\0';
                    toDrawX += Draw(surface, toDraw, toDrawX, y, accumulatedInstructions != PaintNormal);

                    // Start a new string
                    toDrawCount = 0;
                    accumulatedInstructions = charInstructions;
                }
            }

            // Else (starting a new string)
            else
            {
                accumulatedInstructions = charInstructions;
            }

            toDraw[toDrawCount++] = charToDraw;
        }

        // Draw what we've accumulated
        if (toDrawCount)
        {
            toDraw[toDrawCount] = '\0';
            Draw(surface, toDraw, toDrawX, y, accumulatedInstructions != PaintNormal);
        }

        y += _rowHeight;
    }
    
    return true;
}

// Returns length in pixels of text drawn
int32_t TerminalPainter::Draw(SurfaceWrapper *surface, char *text, int32_t x, int32_t y, bool inverse)
{
    // Get the absolute location of the drawing surface
    GFX_Rect rect = laWidget_RectToScreenSpace(surface->GetSurface());

    // We sometimes shrink the height of this pane (e.g. in ToolGPS when we show the
    // output instead of the map). So check whether we're painting within bounds
    int32_t height = rect.height;
    if (y > height)
    {
        return 0;
    }

    int len = strlen(text);

    // Trim the trailing spaces from the string
    while (len && text[len - 1] == ' ')
        --len;

    int32_t clipX = x + rect.x, clipY = y + rect.y;
    for (int i = 0; i < len; ++i)
    {
        // Erase the background
        // If we're drawing inverse (i.e. two hex digits
        if (inverse)
        {
            // Erase two character background at a time
            if ((i & 1) == 0)
            {
                GFX_Set(GFXF_DRAW_COLOR, _inverseScheme->background);
                GFX_DrawRect(clipX, clipY, _columnWidth * 2 - 1, _rowHeight);
                GFX_Set(GFXF_DRAW_COLOR, _inverseScheme->text);
            }
        }
        // Else (not inverse) white background
        else
        {
            GFX_Set(GFXF_DRAW_COLOR, _normalScheme->background);
            GFX_DrawRect(clipX, clipY, _columnWidth, _rowHeight);
            GFX_Set(GFXF_DRAW_COLOR, _normalScheme->text);
        }

        GFXU_DrawGlyph(&MonoFont, text[i], clipX, clipY, _columnWidth, _rowHeight, clipX, clipY);
        clipX += _columnWidth;
    }
    
    return _columnWidth * len;
}
