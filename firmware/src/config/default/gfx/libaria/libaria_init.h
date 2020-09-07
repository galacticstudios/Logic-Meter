// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
// DOM-IGNORE-END

/*******************************************************************************
  MPLAB Harmony Graphics Composer Generated Definitions Header

  File Name:
    libaria_macros.h

  Summary:
    Build-time generated definitions header based on output by the MPLAB Harmony
    Graphics Composer.

  Description:
    Build-time generated definitions header based on output by the MPLAB Harmony
    Graphics Composer.

    Created with MPLAB Harmony Version 3.0
*******************************************************************************/


#ifndef _LIBARIA_INIT_H
#define _LIBARIA_INIT_H

#include "gfx/libaria/libaria.h"
#include "gfx/libaria/libaria_events.h"

#include "gfx/gfx_assets.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END 

#define LIBARIA_SCREEN_COUNT   1

// reference IDs for generated libaria screens
// screen "default"
#define default_ID    0



extern laScheme defaultScheme;
extern laScheme InverseTextScheme;
extern laScheme MenuScheme;
extern laScheme NormalTextScheme;
extern laScheme PanelScheme;
extern laScheme TitleBarScheme;
extern laScheme WorldMapCursorScheme;
extern laKeyPadWidget* MenuKeypad;
extern laLabelWidget* TitleLabel;
extern laLabelWidget* StatusLabel;
extern laLineWidget* TitleBarLine;
extern laWidget* SquareWavePanel;
extern laDrawSurfaceWidget* SquareWaveFrequency;
extern laDrawSurfaceWidget* SquareWavePulse;
extern laDrawSurfaceWidget* SquareWave;
extern laDrawSurfaceWidget* CycleCaliper;
extern laDrawSurfaceWidget* PulseCaliper;
extern laWidget* GPSPanel;
extern laImageWidget* WorldMap;
extern laLabelWidget* WorldMapCursor;
extern laDrawSurfaceWidget* GPSLatitude;
extern laDrawSurfaceWidget* GPSLongitude;
extern laDrawSurfaceWidget* GPSTime;
extern laWidget* LEDTestPanel;
extern laWidget* IVPanel;
extern laLabelWidget* mA10;
extern laLabelWidget* mA20;
extern laLabelWidget* mA30;
extern laLabelWidget* mALabel;
extern laLabelWidget* Vf1_0;
extern laLabelWidget* Vf2_0;
extern laLabelWidget* Vf3_0;
extern laLabelWidget* VfLabel;
extern laLabelWidget* ZeroLabel;
extern laLabelWidget* IColonLabel;
extern laLabelWidget* VfColonLabel;
extern laLabelWidget* VfSetting;
extern laLabelWidget* mASetting;
extern laWidget* TerminalPanel;
extern laWidget* HelpPanel;
extern laLabelWidget* HelpTextWidget;





int32_t libaria_initialize(void);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // _LIBARIA_INIT_H
/*******************************************************************************
 End of File
*/
