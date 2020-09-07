/* 
 * File:   HWScrollPane.h
 * Author: Bob
 *
 * Created on May 13, 2020, 1:11 PM
 */

#ifndef HWSCROLLPANE_H
#define	HWSCROLLPANE_H

struct laWidget_t;
typedef laWidget_t laWidget;

class HWScroller
{
public:
    HWScroller(laWidget *widget);
    ~HWScroller() {DisableScrolling();}
  
    void EnableScrolling();
    void DisableScrolling() {ScrollTo(0);}
    void ScrollTo(int32_t pos);
    void ScrollBy(int32_t delta);

private:
    HWScroller(const HWScroller& orig);
    
    laWidget *_widget;
    int32_t _scrollPos;
};

#endif	/* HWSCROLLPANE_H */

