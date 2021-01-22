/* 
 * File:   LEDTestPane.h
 * Author: Bob
 *
 * Created on June 2, 2020, 5:54 PM
 */

#ifndef LEDTESTPANE_H
#define	LEDTESTPANE_H

#include "Pane.h"

class LEDTestPane : public Pane
{
public:
    LEDTestPane(int32_t xMax, int32_t yMax);
    virtual ~LEDTestPane();
    
    struct XY
    {
        int32_t X;
        int32_t Y;
        static int XCompare(const XY &xy1, const XY &xy2)
        {
            int32_t diff = xy1.X - xy2.X;
            if (diff == 0)
                diff = xy1.Y - xy2.Y;
            return diff;
        }
        static bool XLessThan(const XY &xy1, const XY &xy2)
        {
            return XCompare(xy1, xy2) < 0;
        }
        static int YCompare(const XY &xy1, const XY &xy2)
        {
            int32_t diff = xy1.Y - xy2.Y;
            if (diff == 0)
                diff = xy1.X - xy2.X;
            return diff;
        }
        static bool YLessThan(const XY &xy1, const XY &xy2)
        {
            return YCompare(xy1, xy2) < 0;
        }
    };
    void SetData(const std::vector<XY> &data);
    void SetCurrentLevel(XY &data);
    
    void Update();
    
protected:
    virtual laWidget *GetWidget() const;
    
private:
    LEDTestPane(const LEDTestPane& orig);
    
    static laBool SGraphAreaPaint(laDrawSurfaceWidget *sfc, GFX_Rect *bounds);
    laBool GraphAreaPaint(laDrawSurfaceWidget *sfc, GFX_Rect *bounds);
    
    uint32_t _xMax, _yMax;
    std::vector<XY> _data;
    XY _current, _oldCurrent;
    
    static LEDTestPane *_this;
    static laString *_cursor;
    static GFX_Rect _cursorRect;
};

#endif	/* LEDTESTPANE_H */

