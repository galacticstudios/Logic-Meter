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

struct Settings
{
    bool valid = true;
    
    // System
    uint32_t screenDimMinutes = 5;
    
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
    
    // UART
    bool uartAutobaud = true;
    uint32_t uartBaud = 110;
};

void SettingsInitialize();
void SettingsOnIdle();
void SettingsModified();

// How many ms to wait before writing modified settings to flash
#define SettingsWriteDelayMS 5000

extern Settings settings;

#endif	/* SETTINGS_H */

