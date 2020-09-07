/* 
 * File:   ToolGPS.h
 * Author: Bob
 *
 * Created on May 21, 2020, 9:05 PM
 */

#ifndef TOOLGPS_H
#define	TOOLGPS_H

#include <stdint.h>
#include <time.h>
#include <string>
#include "Utility.h"
#include "Tool.h"

class GPSPane;
class TerminalPane;

class ToolGPS : public Tool
{
public:
    static Tool *Factory() {return new ToolGPS;}
    
    ToolGPS();
    virtual ~ToolGPS();
    
    virtual void OnIdle();
    
    virtual void Update();
     
    void EditLatitude();
    void EditLongitude();
    void EditTime();
    
    void Up();
    void Down();
    void Left();
    void Right();
    void Done();

    void Baud110Selected() {BaudSelected(110);}
    void Baud300Selected() {BaudSelected(300);}
    void Baud1200Selected() {BaudSelected(1200);}
    void Baud2400Selected() {BaudSelected(2400);}
    void Baud4800Selected() {BaudSelected(4800);}
    void Baud9600Selected() {BaudSelected(9600);}
    void Baud14400Selected() {BaudSelected(14400);}
    void Baud19200Selected() {BaudSelected(19200);}
    void Baud38400Selected() {BaudSelected(38400);}
    void Baud57600Selected() {BaudSelected(57600);}
    void Baud115200Selected() {BaudSelected(115200);}
    
    void ShowOutput();
    void ShowMap();
    
private:

    typedef enum {FirstNMEAType, GGA = FirstNMEAType, RMC, MaxNMEAType} NMEA;
    std::string NMEASentence(NMEA sentence);
    NMEA _nextMessage;
    uint32_t _lastMessageTime;
    std::string _sentences[(int) MaxNMEAType - (int) FirstNMEAType];
    
    static void SReadyToWrite(void *context) {((ToolGPS *) context)->ReadyToWrite();}
    void ReadyToWrite();
    
    void BaudSelected(int baud);
    void SetBaudRate(int baud);
    
    ToolGPS(const ToolGPS& orig);
    
    GPSPane *GetPane() {return (GPSPane *) Tool::GetPane();}

    NMEA _currentNEMAMessage = GGA;
    uint32_t _timeOffset;
    bool _timeRunning;
    
    CQueue<char, 200> _transmitQueue;
};

#endif	/* TOOLGPS_H */

