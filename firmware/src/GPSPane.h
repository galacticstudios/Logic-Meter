/* 
 * File:   GPSPane.h
 * Author: Bob
 *
 * Created on May 21, 2020, 9:08 PM
 */

#ifndef GPSPANE_H
#define	GPSPANE_H

#include <stdint.h>
#include <time.h>
#include "Utility.h"
#include "GPSCoordSpinWidget.h"
#include "DateTimeSpinWidget.h"
#include "Pane.h"
#include "TerminalPane.h"

struct laWidget_t;
typedef struct laWidget_t laWidget;
struct GFX_Point_t;
typedef struct GFX_Point_t GFX_Point;

class GPSPane : public Pane
{
public:
    GPSPane(int32_t latitude, int32_t longitude, time_t tim);
    virtual ~GPSPane();
    
    void Update();
    
    void EditLatitude()
    {
        SetEditingWidget(&_latWidget);
        _latWidget.StartEditing(5);
    }
    void EditLongitude()
    {
        SetEditingWidget(&_longWidget);
        _longWidget.StartEditing(6);
    }
    void EditTime()
    {
        SetEditingWidget(&_timeWidget);
        _timeWidget.StartEditing(1);
    }
    
    void SetTime(time_t newTime);
    
    time_t GetTime() const {return _time;}
    int32_t GetLatitude() const {return _latitude;}
    int32_t GetLongitude() const {return _longitude;}
    
    void ShowMap();
    void ShowTerminal();
    
    void SetOutputText(const std::string &text) {_terminal.SetText(text.c_str(), text.size());}
    
protected:
    virtual laWidget *GetWidget() const;
    
private:
    GPSPane(const GPSPane& orig);
    GFX_Point LocationToMap(int32_t latitude, int32_t longitude) const;
    
    typedef enum {X = 0, Y = 1} InterpolationCoord;
    int32_t RobinsonInterpolation(int32_t value, InterpolationCoord coord) const;
    
    int32_t _latitude, _longitude;
    time_t _time;
    GPSCoordSpinWidget _latWidget;
    GPSCoordSpinWidget _longWidget;
    DateTimeSpinWidget _timeWidget;
    
    TerminalPane _terminal;
    Rect _oldTerminalRect;
};

#endif	/* GPSPANE_H */

