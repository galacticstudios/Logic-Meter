/* 
 * File:   ToolGPS.cpp
 * Author: Bob
 * 
 * Created on May 21, 2020, 9:05 PM
 */

extern "C" 
{
#include "definitions.h"
}
#include "Settings.h"
#include "ToolGPS.h"
#include "GPSPane.h"
#include "Menu.h"
#include "PPS.h"
#include "Display.h"
#include "TerminalPane.h"

extern "C" int32_t la_strcmp(GFXU_CHAR* str1, const GFXU_CHAR* str2);

static const Help help(NULL, "GPS UART Data", NULL, 
        "Outputs simulated NMEA sentences.");

static const MenuItem modifyMenuItems[5] = {
    MenuItem(UTF8_UPARROW, MenuType::NoChange, nullptr, CB(&ToolGPS::Up)), 
    MenuItem(UTF8_DOWNARROW, MenuType::NoChange, nullptr, CB(&ToolGPS::Down)), 
    MenuItem(UTF8_LEFTARROW, MenuType::NoChange, nullptr, CB(&ToolGPS::Left)), 
    MenuItem(UTF8_RIGHTARROW, MenuType::NoChange, nullptr, CB(&ToolGPS::Right)), 
    MenuItem("Done", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Done))};

static const Menu modifyMenu(modifyMenuItems);

static const MenuItem coordsMenuItems[5] = {
    MenuItem("Lat.", MenuType::ChildMenu, &modifyMenu, CB(&ToolGPS::EditLatitude)), 
    MenuItem("Long.", MenuType::ChildMenu, &modifyMenu, CB(&ToolGPS::EditLongitude)), 
    MenuItem(), 
    MenuItem(), 
    MenuItem("Done", MenuType::ParentMenu)};

static const Menu coordsMenu(coordsMenuItems);

extern const Menu toolGPSMenu;

extern const Menu gpsBaud1;

static const MenuItem baud3Items[5] = {
    MenuItem("38400", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Baud38400Selected)), 
    MenuItem("57600", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Baud57600Selected)), 
    MenuItem("115200", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Baud115200Selected)), 
    MenuItem(), 
    MenuItem("More" UTF8_RIGHTARROW, MenuType::SiblingMenu, &gpsBaud1)};

static const Menu gpsBaud3(baud3Items);

static const MenuItem baud2Items[5] = {
    MenuItem("4800", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Baud4800Selected)), 
    MenuItem("9600", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Baud9600Selected)), 
    MenuItem("14400", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Baud14400Selected)), 
    MenuItem("19200", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Baud19200Selected)), 
    MenuItem("More" UTF8_RIGHTARROW, MenuType::SiblingMenu, &gpsBaud3)};

static const Menu gpsBaud2(baud2Items);

static const MenuItem baud1Items[5] = {
    MenuItem("110", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Baud110Selected)), 
    MenuItem("300", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Baud300Selected)), 
    MenuItem("1200", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Baud1200Selected)), 
    MenuItem("2400", MenuType::ParentMenu, nullptr, CB(&ToolGPS::Baud2400Selected)), 
    MenuItem("More" UTF8_RIGHTARROW, MenuType::SiblingMenu, &gpsBaud2)};

const Menu gpsBaud1(baud1Items);

static const MenuItem outputItems[5] = {
    MenuItem("Coords", MenuType::ChildMenu, &coordsMenu), 
    MenuItem("Time", MenuType::ChildMenu, &modifyMenu, CB(&ToolGPS::EditTime)), 
    MenuItem("Baud", MenuType::ChildMenu, &gpsBaud1),
    MenuItem("Map", MenuType::SiblingMenu, &toolGPSMenu, CB(&ToolGPS::ShowMap)),
    MenuItem()};

static const Menu outputMenu(outputItems);

static const MenuItem menuItems[5] = {
    MenuItem("Coords", MenuType::ChildMenu, &coordsMenu), 
    MenuItem("Time", MenuType::ChildMenu, &modifyMenu, CB(&ToolGPS::EditTime)), 
    MenuItem("Baud", MenuType::ChildMenu, &gpsBaud1),
    MenuItem("Output", MenuType::SiblingMenu, &outputMenu, CB(&ToolGPS::ShowOutput)),
    MenuItem()};

const Menu toolGPSMenu(menuItems);

static int baudRates[] = {2400, 4800, 9600, 19200,38400, 57600, 115200};

ToolGPS::ToolGPS() :
    Tool("GPS Sim", new GPSPane(settings.gpsLatitude, settings.gpsLongitude, settings.gpsTime), toolGPSMenu, help),
    _nextMessage(GGA), _lastMessageTime(SYS_TIME_CounterGet()),
    _timeOffset(0), _timeRunning(true)
{
    _uart.RegisterWriteCallback(&ToolGPS::SReadyToWrite, this);
    _uart.SetInterruptPriorities();
    _uart.Initialize();
    UARTSerialSetup uss = {settings.gpsBaud, UARTSerialSetup::UART8BitParityNone, 1};
    _uart.SerialSetup(&uss, 0);
    SetBaudRate(settings.gpsBaud);
    RPD10RPPS = PPSGroup1Outputs::U3TX;
    TRISDbits.TRISD10 = 0;
}

ToolGPS::~ToolGPS() 
{
    _uart.DisableTXInterrupt();
    _uart.Disable();
    RPD10RPPS = PPSGroup1Outputs::O1OFF;
    TRISDbits.TRISD10 = 1;
    _uart.UnregisterWriteCallback();
}

void ToolGPS::ShowOutput()
{
    GetPane()->ShowTerminal();
}

void ToolGPS::ShowMap()
{
    GetPane()->ShowMap();
}

void ToolGPS::ReadyToWrite()
{
    char data;
    _uart.EnableTXInterrupt();
    while (_uart.TXReady() && _transmitQueue.read(&data, 1))
        _uart.TXData(data);
    if (_transmitQueue.empty())
        _uart.DisableTXInterrupt();
}

void ToolGPS::OnIdle()
{
    // If we're going to send the first NMEA message
    if (_nextMessage == FirstNMEAType)
    {
        // Make sure a second has passed since the last time
        uint32_t now = SYS_TIME_CounterGet();
        if (now - _lastMessageTime < SYS_TIME_MSToCount(1000))
            return;
        // Else (a second has passed since we last started a block of NMEA sentences)
        else
        {
            _lastMessageTime = now;
            ++_timeOffset;
            GetPane()->SetTime(settings.gpsTime + _timeOffset);
        }
    }

    // Send the next message
    std::string sentence = NMEASentence(_nextMessage);
    for (auto ch : sentence)
    {
        while (_transmitQueue.full()) {}
        _transmitQueue.write(ch);
    }
    
    // Show the sentences in the output window
    _sentences[(int) _nextMessage] = sentence;
    std::string outputText = "\r\n";
    for (auto &s : _sentences)
    {
        outputText += s + "\r\n";
    }
    GetPane()->SetOutputText(outputText);
    

    _nextMessage = NMEA(int(_nextMessage) + 1);
    if (_nextMessage == MaxNMEAType)
    {
        _nextMessage = FirstNMEAType;
    }

    // Kickstart the transmitting
    ReadyToWrite();
}

void ToolGPS::Update()
{
    settings.gpsLatitude = GetPane()->GetLatitude();
    settings.gpsLongitude = GetPane()->GetLongitude();
    settings.gpsTime = GetPane()->GetTime();
    SettingsModified();
}

void ToolGPS::EditLatitude()
{
    GetPane()->EditLatitude();
}

void ToolGPS::EditLongitude()
{
    GetPane()->EditLongitude();
}

void ToolGPS::EditTime()
{
    _timeRunning = false;
    GetPane()->EditTime();
}

void ToolGPS::Up()
{
    GetPane()->Up();
    Update();
}

void ToolGPS::Down()
{
    GetPane()->Down();
    Update();
}

void ToolGPS::Left()
{
    GetPane()->Left();
}

void ToolGPS::Right()
{
    GetPane()->Right();
}

void ToolGPS::Done()
{
    GetPane()->Done();
    _timeRunning = true;
}

void ToolGPS::SetBaudRate(int baud)
{
    settings.gpsBaud = baud;
    SettingsModified();
    UARTSerialSetup setup = {settings.gpsBaud, UARTSerialSetup::UART8BitParityNone, 1};
    _uart.SerialSetup(&setup, 0);
    
    char buf[10];
    sprintf(buf, "%d", settings.gpsBaud);
    SetStatusText(buf);
}

void ToolGPS::BaudSelected(int baud)
{
    SetBaudRate(baud);
}

std::string ToolGPS::NMEASentence(NMEA sentence)
{
    char buf[100], *output = buf;
    struct tm *t;
    time_t time = settings.gpsTime + _timeOffset;
    int32_t lat = settings.gpsLatitude, lng = settings.gpsLongitude;
    int32_t absLat, absLng, latMinutes, lngMinutes;
    ldiv_t latDiv, lngDiv, latMinutesDiv, lngMinutesDiv;
    
    t = gmtime(&time);
    absLat = labs(lat);
    latDiv = ldiv(absLat, 10000);
    latMinutes = latDiv.rem * 60;
    latMinutesDiv = ldiv(latMinutes, 10000);
    absLng = labs(lng);
    lngDiv = ldiv(absLng, 10000);
    lngMinutes = lngDiv.rem * 60;
    lngMinutesDiv = ldiv(lngMinutes, 10000);
    
    switch (sentence)
    {
        // $GPGGA,hhmmss.sss,llll.lll,a,nnnnn.nnn,b,t,uu,v.v,w.w,M,x.x,M,y.y,zzzz*hh<CR><LF>
        // http://aprs.gids.nl/nmea/#gga
        case GGA :
            sprintf(output, "$GPGGA,"
                // hour, minute, second,
                "%02d%02d%02d,"
                // latitude degrees, minutes, number of decimals in decimal minutes, decimal minutes, N or S hemisphere
                "%02d%02d.%04d,%c,"
                // longitude degrees, minutes, number of decimals in decimal minutes, decimal minutes, E or W hemisphere
                "%03d%02d.%04d,%c," 
                // GPS quality, 1 = No GPS, 1 = GPS, 2 = DGPS
                "%d,"
                // Number of satellites
                "5,"
                // Horizontal Dilution of Precision (HDOP)
                "0.9,"
                // Antenna altitude in meters
                "%d.%01d,M,"
                // Geoidal Separation in Meters
                "%d.%01d,M,"
                // Age of differential GPS data
                ","
                // Differential reference station ID (0000-1023)
                ",",
                t->tm_hour, t->tm_min, t->tm_sec, // time
                (int) latDiv.quot, (int) latMinutesDiv.quot, (int) latMinutesDiv.rem, lat >= 0 ? 'N' : 'S', // latitude
                (int) lngDiv.quot, (int) lngMinutesDiv.quot, (int) lngMinutesDiv.rem, lng < 0 ? 'W' : 'E', // longitude
                8, // quality - simulated
                280, 2,  // altitude
                -34, 1 // separation
            );
            break;
            
        case RMC :
            sprintf(output, "$GPRMC,"
                // hour, minute, second,
                "%02d%02d%02d,"
                "A,"
                // latitude degrees, minutes, number of decimals in decimal minutes, decimal minutes, N or S hemisphere
                "%02d%02d.%04d,%c,"
                // longitude degrees, minutes, number of decimals in decimal minutes, decimal minutes, E or W hemisphere
                "%03d%02d.%04d,%c," 
                // speed over the ground in knots
                "%d.%03d,"
                // track angle in degrees True
                "%d.%d,"
                // date DDMMYY
                "%02d%02d%02d,"
                // magnetic variation
                ",", // "%d.%d,%c",
                t->tm_hour, t->tm_min, t->tm_sec, // time
                (int) latDiv.quot, (int) latMinutesDiv.quot, (int) latMinutesDiv.rem, lat >= 0 ? 'N' : 'S', // latitude
                (int) lngDiv.quot, (int) lngMinutesDiv.quot, (int) lngMinutesDiv.rem, lng >= 0 ? 'W' : 'E', // longitude
                // speed
                0, 0,
                // track angle
                0, 0,
                // date
                t->tm_mday, t->tm_mon + 1, t->tm_year % 100
                // magnetic variation
            );
            break;
            
        default :
            *output = '\0';
            break;
    }
    
    // Calculate the checksum
    int c = 0;
    char *s = output;
    if (*s == '$')
    {
        ++s;
        while (*s)
            c ^= *s++;
        sprintf(s, "*%02X\r\n", c);
    }
    
    return buf;
}

