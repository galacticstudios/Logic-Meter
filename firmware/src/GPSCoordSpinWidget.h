/* 
 * File:   GPSCoordSpinWidget.h
 * Author: Bob
 *
 * Created on May 25, 2020, 9:58 AM
 */

#ifndef GPSCOORDSPINWIDGET_H
#define	GPSCOORDSPINWIDGET_H

#include "SpinWidget.h"

class GPSCoordSpinWidget : public SpinWidget
{
public:
    typedef enum {Latitude, Longitude} Coord;
    GPSCoordSpinWidget(laDrawSurfaceWidget *ds, int32_t *coord, Coord coordType) : 
        SpinWidget(ds, (coordType == Latitude) ? 6 : 7), _coord(coord), _latLong(coordType) 
    {
        UpdateText();
    }
    virtual ~GPSCoordSpinWidget() {}

    std::string ToString(int cursorPos) const;
    void Increment(int cursorPos, bool increment);

private:
    int32_t *_coord;
    Coord _latLong;
    
    static int LeadingChars(char *b, int32_t val, int place, char leadingChar);

    GPSCoordSpinWidget(const GPSCoordSpinWidget& orig);
};

#endif	/* GPSCOORDSPINWIDGET_H */

