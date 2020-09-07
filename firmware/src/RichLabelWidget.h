/* 
 * File:   RichLabelWidget.h
 * Author: Bob
 *
 * Created on May 24, 2020, 11:22 AM
 */

#ifndef RICHLABELWIDGET_H
#define	RICHLABELWIDGET_H

#include "SurfaceWrapper.h"
#include "RichTextPainter.h"

#define NORMAL_TEXT "\x1"
#define INVERSE_TEXT "\x2"
#define LAST_SYMBOL INVERSE_TEXT

class RichLabelWidget
{
public:
    RichLabelWidget(laDrawSurfaceWidget *ds);
    ~RichLabelWidget();
    
    // Text is UTF-8.
    void SetText(const char *text);
    
private:
    RichLabelWidget(const RichLabelWidget& orig);
    
    SurfaceWrapper _surface;
    RichTextPainter _painter;
};

#endif	/* RICHLABELWIDGET_H */

