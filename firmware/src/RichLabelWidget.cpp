/* 
 * File:   RichLabelWidget.cpp
 * Author: Bob
 * 
 * Created on May 24, 2020, 11:22 AM
 */

extern "C" 
{
#include "definitions.h"
}
#include "Display.h"
#include "RichLabelWidget.h"

RichLabelWidget::RichLabelWidget(laDrawSurfaceWidget *ds) :
    _surface(ds)
{
    _surface.Register(&_painter);
}

RichLabelWidget::~RichLabelWidget() 
{
    _surface.Unregister(&_painter);
}

void RichLabelWidget::SetText(const char *text)
{
    auto &rtb = _painter.RichTextBlocks();
    rtb.clear();
    
    uint16_t fg = 0, bg = 0xffff;
    char blockText[100], *bt = blockText;
    while (*text)
    {
        while ((unsigned char) *text > *LAST_SYMBOL)
        {
            *bt++ = *text++;
        }
        *bt = '\0';
        if (bt != blockText)
            rtb.push_back({blockText, &MonoFont, fg, bg, 0});
        
        if (*text == *NORMAL_TEXT)
        {
            fg = 0;
            bg = 0xffff;
            ++text;
        }
        else if (*text == *INVERSE_TEXT)
        {
            fg = 0xffff;
            bg = 0;
            ++text;
        }
                
        bt = blockText;
    }
    
    _surface.Invalidate();
}
