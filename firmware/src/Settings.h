/* 
 * File:   Settings.h
 * Author: Bob
 *
 * Created on July 27, 2020, 6:16 PM
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

#include <stdint.h>
#include <time.h>
#include "fixed.h"

enum class TriggerMode : uint8_t {Auto, Normal, Single};
enum class TriggerEdge : uint8_t {Rising, Falling, Either};

struct Settings
{
    // 1 is valid; any other value is invalid
    uint8_t validity = 1;
    
    // System
    uint32_t screenDimMinutes = 5;
    uint8_t screenBrightness = 50; // 1..100; lower numbers are brighter
    
    // GPS
    // Greenwich, in 10,000ths of a degree
    int32_t gpsLatitude = 514934, gpsLongitude = 0;
    time_t gpsTime = 1577836800; // Jan. 1, 2020 00:00
    uint32_t gpsBaud = 9600;
    
    // PWM
    long long pwmHertz = 1000 * fixed::scale, pwmDuty = 50 * fixed::scale;
    
    // Servo
    long long servoDuty = (long long) (1.5 / 20 * 100 * fixed::scale);
    
    // SPI
    uint8_t spiPolarity = 0, spiPhase = 0, spiUseSelect = 0;
    uint8_t spiWidth = 8;
    
    // UART In
    bool uartAutobaud = true;
    uint32_t uartBaud = 110;
    
    // UART Out
    uint32_t uartOutBaud = 9600;
    char uartOutFile[SYS_FS_FILE_NAME_LEN + 1];
    
    // Logic Analyzer
    TriggerMode triggerMode = TriggerMode::Auto;
    uint8_t enabledChannels = 1; // bitN = channelN
    uint8_t triggerChannel = 0; // Index 1, 2, or 3. 0 for off
    TriggerEdge triggerEdge = TriggerEdge::Rising;
    uint32_t triggerPosition = 50; // Percentage of the acquisition buffer before trigger
    uint32_t sampleFreq = 1000000;
    
};

void SettingsInitialize();
void SettingsOnIdle();
void SettingsModified();
void SettingsDump();

// How many ms to wait before writing modified settings to flash
#define SettingsWriteDelayMS 5000

extern Settings settings;

#endif	/* SETTINGS_H */

