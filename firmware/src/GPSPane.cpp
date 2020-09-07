/* 
 * File:   GPSPane.cpp
 * Author: Bob
 * 
 * Created on May 21, 2020, 9:08 PM
 */

#include <stdio.h>
#include <time.h>
#include <vector>
extern "C" 
{
#include "definitions.h"
}
#include "Display.h"
#include "GPSPane.h"
#include "SurfaceWrapper.h"
#include "RichTextPainter.h"

GPSPane::GPSPane(int32_t latitude, int32_t longitude, time_t tim) : 
    _latitude(latitude), _longitude(longitude), _time(tim),
    _latWidget(GPSLatitude, &_latitude, GPSCoordSpinWidget::Latitude),
    _longWidget(GPSLongitude, &_longitude, GPSCoordSpinWidget::Longitude),
    _timeWidget(GPSTime, &_time)
{
    _terminal.SetVisible(false);
    // Resize the terminal to match the map
    _oldTerminalRect = _terminal.GetPosition();
    // Make the terminal pane the same height as the map
    Rect newTerminalRect = _oldTerminalRect;
    newTerminalRect.height = laWidget_GetHeight((laWidget *) WorldMap);
    _terminal.SetPosition(newTerminalRect);
    
    laWidget_SetVisible((laWidget *) WorldMap, LA_TRUE);
}

GPSPane::~GPSPane() 
{
    // Restore the position of the underlying Aria panel
    _terminal.SetPosition(_oldTerminalRect);
    _terminal.SetVisible(false);
}

void GPSPane::Update()
{
    // Position the marker on the map
    if (laWidget_GetVisible((laWidget *) WorldMap))
    {
        GFX_Point mapCoords = LocationToMap(_latitude, _longitude);
        int32_t cursorDiameter = laWidget_GetWidth((laWidget *) WorldMapCursor);
        laWidget_SetPosition((laWidget *) WorldMapCursor, 
            mapCoords.x - cursorDiameter / 2, mapCoords.y - cursorDiameter / 2);
    }
}

void GPSPane::SetTime(time_t newTime) 
{
    uint32_t oldMinutes = _time / 60, newMinutes = newTime / 60;
    _time = newTime; 
    if (oldMinutes != newMinutes)
        _timeWidget.UpdateText();
}

laWidget *GPSPane::GetWidget() const
{
    return GPSPanel;
}

// From the latitude and longitude, figure out the map position
// Lat. and long. are 10,000ths of a degree
GFX_Point GPSPane::LocationToMap(int32_t latitude, int32_t longitude) const
{
    int32_t mapWidth = laWidget_GetWidth((laWidget *) WorldMap);
    int32_t xR = mapWidth / 2;
    GFX_Point result;
    
    // Calculate longitude in 10,000ths of a radian
    // The map we have is offset by 17 degrees
    longitude -= 170000;
    if (longitude < -1800000)
        longitude += 3600000;
    int32_t longPixels = (int64_t) RobinsonInterpolation(latitude, X) * xR * longitude 
        / 1800000 / 10000;
    
    int32_t mapHeight = laWidget_GetHeight((laWidget *) WorldMap);
    int32_t yR = mapHeight / 2;
    int32_t latPixels = RobinsonInterpolation(latitude, Y) * yR / 10000;
    
    result.x = longPixels + xR;
    result.y = yR - latPixels;
    
    return result;
}

// value is latitude or longitude in 10,000ths of a degree
int32_t GPSPane::RobinsonInterpolation(int32_t latitude, InterpolationCoord coord) const
{
    static const  int32_t robinsonMapParameters[][2] = {
    // Values have been multiplied by 10,000
    /*  0° */ {10000,     0},
    /*  5° */ { 9986,   620},
    /* 10° */ { 9954,  1240},
    /* 15° */ { 9900,  1860},
    /* 20° */ { 9822,  2480},
    /* 25° */ { 9730,  3100},
    /* 30° */ { 9600,  3720},
    /* 35° */ { 9427,  4340},
    /* 40° */ { 9216,  4958},
    /* 45° */ { 8962,  5571},
    /* 50° */ { 8679,  6176},
    /* 55° */ { 8350,  6769},
    /* 60° */ { 7986,  7346},
    /* 65° */ { 7597,  7903},
    /* 70° */ { 7186,  8435},
    /* 75° */ { 6732,  8936},
    /* 80° */ { 6213,  9394},
    /* 85° */ { 5722,  9761},
    /* 90° */ { 5322, 10000}
    };
    
    int32_t interpolated;
    int32_t absLatitude = (latitude < 0 ? -latitude : latitude);
    int index = absLatitude / 50000;
    int32_t low = robinsonMapParameters[index][coord];
    int32_t floored = index * 50000;
    if (floored == absLatitude)
        interpolated = low;
    else
    {
        int32_t high = robinsonMapParameters[index + 1][coord];
        interpolated = (absLatitude - floored) * (high - low) / 50000 + low;
    }
    
    if (coord == Y && latitude < 0)
        interpolated = -interpolated;
    return interpolated;
}

void GPSPane::ShowMap()
{
    // Hide the terminal and show the map
    _terminal.SetVisible(false);
    // It might be a bug in Aria, but hiding the world map erases the Lat., Long.,
    // and Time, so we need to re-show them
    laWidget_SetVisible((laWidget *) GPSLatitude, LA_FALSE);
    laWidget_SetVisible((laWidget *) GPSLatitude, LA_TRUE);
    laWidget_SetVisible((laWidget *) GPSLongitude, LA_FALSE);
    laWidget_SetVisible((laWidget *) GPSLongitude, LA_TRUE);
    laWidget_SetVisible((laWidget *) GPSTime, LA_FALSE);
    laWidget_SetVisible((laWidget *) GPSTime, LA_TRUE);
    laWidget_SetVisible((laWidget *) WorldMap, LA_TRUE);
    Update();
}

void GPSPane::ShowTerminal()
{
    // Hide the map and show the terminal
    laWidget_SetVisible((laWidget *) WorldMap, LA_FALSE);
    // It might be a bug in Aria, but hiding the world map erases the Lat., Long.,
    // and Time, so we need to re-show them
    laWidget_SetVisible((laWidget *) GPSLatitude, LA_FALSE);
    laWidget_SetVisible((laWidget *) GPSLatitude, LA_TRUE);
    laWidget_SetVisible((laWidget *) GPSLongitude, LA_FALSE);
    laWidget_SetVisible((laWidget *) GPSLongitude, LA_TRUE);
    laWidget_SetVisible((laWidget *) GPSTime, LA_FALSE);
    laWidget_SetVisible((laWidget *) GPSTime, LA_TRUE);
    _terminal.SetVisible(true);
}
