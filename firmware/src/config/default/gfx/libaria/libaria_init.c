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
  MPLAB Harmony Graphics Composer Generated Implementation File

  File Name:
    libaria_init.c

  Summary:
    Build-time generated implementation from the MPLAB Harmony
    Graphics Composer.

  Description:
    Build-time generated implementation from the MPLAB Harmony
    Graphics Composer.

    Created with MPLAB Harmony Version 3.0
*******************************************************************************/

#include "gfx/libaria/libaria_init.h"
#include "gfx/libaria/inc/libaria_utils.h"

laScheme defaultScheme;
laScheme InverseTextScheme;
laScheme MenuScheme;
laScheme NormalTextScheme;
laScheme PanelScheme;
laScheme TitleBarScheme;
laScheme WorldMapCursorScheme;
laKeyPadWidget* MenuKeypad;
laLabelWidget* TitleLabel;
laLabelWidget* StatusLabel;
laLineWidget* TitleBarLine;
laWidget* SquareWavePanel;
laDrawSurfaceWidget* SquareWaveFrequency;
laDrawSurfaceWidget* SquareWavePulse;
laDrawSurfaceWidget* SquareWave;
laDrawSurfaceWidget* CycleCaliper;
laDrawSurfaceWidget* PulseCaliper;
laWidget* GPSPanel;
laImageWidget* WorldMap;
laLabelWidget* WorldMapCursor;
laDrawSurfaceWidget* GPSLatitude;
laDrawSurfaceWidget* GPSLongitude;
laDrawSurfaceWidget* GPSTime;
laWidget* LEDTestPanel;
laWidget* IVPanel;
laLabelWidget* mA10;
laLabelWidget* mA20;
laLabelWidget* mA30;
laLabelWidget* mALabel;
laLabelWidget* Vf1_0;
laLabelWidget* Vf2_0;
laLabelWidget* Vf3_0;
laLabelWidget* VfLabel;
laLabelWidget* ZeroLabel;
laLabelWidget* IColonLabel;
laLabelWidget* VfColonLabel;
laLabelWidget* VfSetting;
laLabelWidget* mASetting;
laWidget* TerminalPanel;
laWidget* HelpPanel;
laLabelWidget* HelpTextWidget;


static void ScreenCreate_default(laScreen* screen);


int32_t libaria_initialize(void)
{
    laScreen* screen;

    laScheme_Initialize(&defaultScheme, GFX_COLOR_MODE_RGB_565);
    defaultScheme.base = 0xFFFF;
    defaultScheme.highlight = 0xC67A;
    defaultScheme.highlightLight = 0xFFFF;
    defaultScheme.shadow = 0x8410;
    defaultScheme.shadowDark = 0x4208;
    defaultScheme.foreground = 0x0;
    defaultScheme.foregroundInactive = 0xD71C;
    defaultScheme.foregroundDisabled = 0x8410;
    defaultScheme.background = 0xFFFF;
    defaultScheme.backgroundInactive = 0xD71C;
    defaultScheme.backgroundDisabled = 0xC67A;
    defaultScheme.text = 0x0;
    defaultScheme.textHighlight = 0x1F;
    defaultScheme.textHighlightText = 0xFFFF;
    defaultScheme.textInactive = 0xD71C;
    defaultScheme.textDisabled = 0x8C92;

    laScheme_Initialize(&InverseTextScheme, GFX_COLOR_MODE_RGB_565);
    InverseTextScheme.base = 0x0;
    InverseTextScheme.highlight = 0xC67A;
    InverseTextScheme.highlightLight = 0xFFFF;
    InverseTextScheme.shadow = 0x8410;
    InverseTextScheme.shadowDark = 0x4208;
    InverseTextScheme.foreground = 0x0;
    InverseTextScheme.foregroundInactive = 0xD71C;
    InverseTextScheme.foregroundDisabled = 0x8410;
    InverseTextScheme.background = 0x0;
    InverseTextScheme.backgroundInactive = 0xD71C;
    InverseTextScheme.backgroundDisabled = 0xC67A;
    InverseTextScheme.text = 0xFFFF;
    InverseTextScheme.textHighlight = 0x1F;
    InverseTextScheme.textHighlightText = 0xFFFF;
    InverseTextScheme.textInactive = 0xD71C;
    InverseTextScheme.textDisabled = 0x8C92;

    laScheme_Initialize(&MenuScheme, GFX_COLOR_MODE_RGB_565);
    MenuScheme.base = 0xFFFF;
    MenuScheme.highlight = 0xC67A;
    MenuScheme.highlightLight = 0xFFFF;
    MenuScheme.shadow = 0x8410;
    MenuScheme.shadowDark = 0x4208;
    MenuScheme.foreground = 0xFFFF;
    MenuScheme.foregroundInactive = 0xD71C;
    MenuScheme.foregroundDisabled = 0x8410;
    MenuScheme.background = 0xFFFF;
    MenuScheme.backgroundInactive = 0xD71C;
    MenuScheme.backgroundDisabled = 0xC67A;
    MenuScheme.text = 0x0;
    MenuScheme.textHighlight = 0x1F;
    MenuScheme.textHighlightText = 0xFFFF;
    MenuScheme.textInactive = 0xD71C;
    MenuScheme.textDisabled = 0x8C92;

    laScheme_Initialize(&NormalTextScheme, GFX_COLOR_MODE_RGB_565);
    NormalTextScheme.base = 0xFFFF;
    NormalTextScheme.highlight = 0xC67A;
    NormalTextScheme.highlightLight = 0xFFFF;
    NormalTextScheme.shadow = 0x8410;
    NormalTextScheme.shadowDark = 0x4208;
    NormalTextScheme.foreground = 0x0;
    NormalTextScheme.foregroundInactive = 0xD71C;
    NormalTextScheme.foregroundDisabled = 0x8410;
    NormalTextScheme.background = 0xFFFF;
    NormalTextScheme.backgroundInactive = 0xD71C;
    NormalTextScheme.backgroundDisabled = 0xC67A;
    NormalTextScheme.text = 0x0;
    NormalTextScheme.textHighlight = 0x1F;
    NormalTextScheme.textHighlightText = 0xFFFF;
    NormalTextScheme.textInactive = 0xD71C;
    NormalTextScheme.textDisabled = 0x8C92;

    laScheme_Initialize(&PanelScheme, GFX_COLOR_MODE_RGB_565);
    PanelScheme.base = 0xFFFF;
    PanelScheme.highlight = 0xFFFF;
    PanelScheme.highlightLight = 0xFFFF;
    PanelScheme.shadow = 0x8410;
    PanelScheme.shadowDark = 0x4208;
    PanelScheme.foreground = 0x0;
    PanelScheme.foregroundInactive = 0xD71C;
    PanelScheme.foregroundDisabled = 0x8410;
    PanelScheme.background = 0xFFFF;
    PanelScheme.backgroundInactive = 0xD71C;
    PanelScheme.backgroundDisabled = 0xC67A;
    PanelScheme.text = 0x0;
    PanelScheme.textHighlight = 0x1F;
    PanelScheme.textHighlightText = 0xFFFF;
    PanelScheme.textInactive = 0xD71C;
    PanelScheme.textDisabled = 0x8C92;

    laScheme_Initialize(&TitleBarScheme, GFX_COLOR_MODE_RGB_565);
    TitleBarScheme.base = 0xFFFF;
    TitleBarScheme.highlight = 0xC67A;
    TitleBarScheme.highlightLight = 0xFFFF;
    TitleBarScheme.shadow = 0x8410;
    TitleBarScheme.shadowDark = 0x4208;
    TitleBarScheme.foreground = 0x0;
    TitleBarScheme.foregroundInactive = 0xD71C;
    TitleBarScheme.foregroundDisabled = 0x8410;
    TitleBarScheme.background = 0xFFFF;
    TitleBarScheme.backgroundInactive = 0xD71C;
    TitleBarScheme.backgroundDisabled = 0xC67A;
    TitleBarScheme.text = 0x0;
    TitleBarScheme.textHighlight = 0x1F;
    TitleBarScheme.textHighlightText = 0xFFFF;
    TitleBarScheme.textInactive = 0xD71C;
    TitleBarScheme.textDisabled = 0x8C92;

    laScheme_Initialize(&WorldMapCursorScheme, GFX_COLOR_MODE_RGB_565);
    WorldMapCursorScheme.base = 0xC67A;
    WorldMapCursorScheme.highlight = 0xC67A;
    WorldMapCursorScheme.highlightLight = 0xFFFF;
    WorldMapCursorScheme.shadow = 0x8410;
    WorldMapCursorScheme.shadowDark = 0x4208;
    WorldMapCursorScheme.foreground = 0xF800;
    WorldMapCursorScheme.foregroundInactive = 0xD71C;
    WorldMapCursorScheme.foregroundDisabled = 0x8410;
    WorldMapCursorScheme.background = 0xF800;
    WorldMapCursorScheme.backgroundInactive = 0xD71C;
    WorldMapCursorScheme.backgroundDisabled = 0xC67A;
    WorldMapCursorScheme.text = 0xF800;
    WorldMapCursorScheme.textHighlight = 0x1F;
    WorldMapCursorScheme.textHighlightText = 0xFFFF;
    WorldMapCursorScheme.textInactive = 0xD71C;
    WorldMapCursorScheme.textDisabled = 0x8C92;

    GFX_Set(GFXF_DRAW_PIPELINE_MODE, GFX_PIPELINE_GCUGPU);
    laContext_SetStringTable(&stringTable);

    screen = laScreen_New(LA_FALSE, LA_FALSE, &ScreenCreate_default);
    laContext_AddScreen(screen);

    laContext_SetActiveScreen(0);

	return 0;
}

static void ScreenCreate_default(laScreen* screen)
{
    laLayer* layer0;

    layer0 = laLayer_New();
    laWidget_SetPosition((laWidget*)layer0, 0, 0);
    laWidget_SetSize((laWidget*)layer0, 320, 240);
    laWidget_SetBackgroundType((laWidget*)layer0, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetScheme((laWidget*)layer0, &defaultScheme);
    laLayer_SetBufferCount(layer0, 1);

    laScreen_SetLayer(screen, 0, layer0);

    MenuKeypad = laKeyPadWidget_New(1, 5);
    laWidget_SetPosition((laWidget*)MenuKeypad, 0, 220);
    laWidget_SetSize((laWidget*)MenuKeypad, 320, 20);
    laWidget_SetOptimizationFlags((laWidget*)MenuKeypad, LA_WIDGET_OPT_LOCAL_REDRAW | LA_WIDGET_OPT_DRAW_ONCE | LA_WIDGET_OPT_OPAQUE);
    laWidget_SetScheme((laWidget*)MenuKeypad, &MenuScheme);
    laWidget_SetBackgroundType((laWidget*)MenuKeypad, LA_WIDGET_BACKGROUND_NONE);
    laWidget_SetBorderType((laWidget*)MenuKeypad, LA_WIDGET_BORDER_LINE);
    laWidget_SetMargins((laWidget*)MenuKeypad, 0, 0, 0, 0);
    laKeyPadWidget_SetKeyAction(MenuKeypad, 0, 0, LA_KEYPAD_CELL_ACTION_NONE);
    laKeyPadWidget_SetKeyImagePosition(MenuKeypad, 0, 0, LA_RELATIVE_POSITION_BEHIND);
    laKeyPadWidget_SetKeyAction(MenuKeypad, 0, 1, LA_KEYPAD_CELL_ACTION_NONE);
    laKeyPadWidget_SetKeyImagePosition(MenuKeypad, 0, 1, LA_RELATIVE_POSITION_BEHIND);
    laKeyPadWidget_SetKeyAction(MenuKeypad, 0, 2, LA_KEYPAD_CELL_ACTION_NONE);
    laKeyPadWidget_SetKeyImagePosition(MenuKeypad, 0, 2, LA_RELATIVE_POSITION_BEHIND);
    laKeyPadWidget_SetKeyAction(MenuKeypad, 0, 3, LA_KEYPAD_CELL_ACTION_NONE);
    laKeyPadWidget_SetKeyImagePosition(MenuKeypad, 0, 3, LA_RELATIVE_POSITION_BEHIND);
    laKeyPadWidget_SetKeyAction(MenuKeypad, 0, 4, LA_KEYPAD_CELL_ACTION_NONE);
    laKeyPadWidget_SetKeyImagePosition(MenuKeypad, 0, 4, LA_RELATIVE_POSITION_BEHIND);
    laWidget_AddChild((laWidget*)layer0, (laWidget*)MenuKeypad);

    TitleLabel = laLabelWidget_New();
    laWidget_SetSize((laWidget*)TitleLabel, 299, 25);
    laWidget_SetScheme((laWidget*)TitleLabel, &TitleBarScheme);
    laWidget_SetBackgroundType((laWidget*)TitleLabel, LA_WIDGET_BACKGROUND_NONE);
    laWidget_SetBorderType((laWidget*)TitleLabel, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetHAlignment(TitleLabel, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)layer0, (laWidget*)TitleLabel);

    StatusLabel = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)StatusLabel, 31, 0);
    laWidget_SetSize((laWidget*)StatusLabel, 289, 25);
    laWidget_SetScheme((laWidget*)StatusLabel, &TitleBarScheme);
    laWidget_SetBackgroundType((laWidget*)StatusLabel, LA_WIDGET_BACKGROUND_NONE);
    laWidget_SetBorderType((laWidget*)StatusLabel, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetHAlignment(StatusLabel, LA_HALIGN_RIGHT);
    laWidget_AddChild((laWidget*)layer0, (laWidget*)StatusLabel);

    TitleBarLine = laLineWidget_New();
    laWidget_SetPosition((laWidget*)TitleBarLine, 0, 25);
    laWidget_SetSize((laWidget*)TitleBarLine, 320, 1);
    laWidget_SetScheme((laWidget*)TitleBarLine, &defaultScheme);
    laWidget_SetBackgroundType((laWidget*)TitleBarLine, LA_WIDGET_BACKGROUND_NONE);
    laWidget_SetBorderType((laWidget*)TitleBarLine, LA_WIDGET_BORDER_NONE);
    laLineWidget_SetStartPoint(TitleBarLine, 0, 0);
    laLineWidget_SetEndPoint(TitleBarLine, 320, 0);
    laWidget_AddChild((laWidget*)layer0, (laWidget*)TitleBarLine);

    SquareWavePanel = laWidget_New();
    laWidget_SetPosition((laWidget*)SquareWavePanel, 0, 26);
    laWidget_SetSize((laWidget*)SquareWavePanel, 320, 194);
    laWidget_SetOptimizationFlags((laWidget*)SquareWavePanel, LA_WIDGET_OPT_OPAQUE);
    laWidget_SetScheme((laWidget*)SquareWavePanel, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)SquareWavePanel, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)SquareWavePanel, LA_WIDGET_BORDER_NONE);
    laWidget_AddChild((laWidget*)layer0, SquareWavePanel);

    SquareWaveFrequency = laDrawSurfaceWidget_New();
    laWidget_SetPosition((laWidget*)SquareWaveFrequency, 38, 25);
    laWidget_SetSize((laWidget*)SquareWaveFrequency, 245, 15);
    laWidget_SetScheme((laWidget*)SquareWaveFrequency, &defaultScheme);
    laWidget_SetBackgroundType((laWidget*)SquareWaveFrequency, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)SquareWaveFrequency, LA_WIDGET_BORDER_NONE);
    laDrawSurfaceWidget_SetDrawCallback(SquareWaveFrequency, &SquareWaveFrequency_DrawNotificationEvent);

    laWidget_AddChild((laWidget*)SquareWavePanel, (laWidget*)SquareWaveFrequency);

    SquareWavePulse = laDrawSurfaceWidget_New();
    laWidget_SetPosition((laWidget*)SquareWavePulse, 38, 150);
    laWidget_SetSize((laWidget*)SquareWavePulse, 245, 20);
    laWidget_SetScheme((laWidget*)SquareWavePulse, &defaultScheme);
    laWidget_SetBackgroundType((laWidget*)SquareWavePulse, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)SquareWavePulse, LA_WIDGET_BORDER_NONE);
    laDrawSurfaceWidget_SetDrawCallback(SquareWavePulse, &SquareWavePulse_DrawNotificationEvent);

    laWidget_AddChild((laWidget*)SquareWavePanel, (laWidget*)SquareWavePulse);

    SquareWave = laDrawSurfaceWidget_New();
    laWidget_SetPosition((laWidget*)SquareWave, 10, 69);
    laWidget_SetSize((laWidget*)SquareWave, 300, 53);
    laWidget_SetScheme((laWidget*)SquareWave, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)SquareWave, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)SquareWave, LA_WIDGET_BORDER_NONE);
    laDrawSurfaceWidget_SetDrawCallback(SquareWave, &SquareWave_DrawNotificationEvent);

    laWidget_AddChild((laWidget*)SquareWavePanel, (laWidget*)SquareWave);

    CycleCaliper = laDrawSurfaceWidget_New();
    laWidget_SetPosition((laWidget*)CycleCaliper, 10, 45);
    laWidget_SetSize((laWidget*)CycleCaliper, 300, 20);
    laWidget_SetScheme((laWidget*)CycleCaliper, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)CycleCaliper, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)CycleCaliper, LA_WIDGET_BORDER_NONE);
    laDrawSurfaceWidget_SetDrawCallback(CycleCaliper, &CycleCaliper_DrawNotificationEvent);

    laWidget_AddChild((laWidget*)SquareWavePanel, (laWidget*)CycleCaliper);

    PulseCaliper = laDrawSurfaceWidget_New();
    laWidget_SetPosition((laWidget*)PulseCaliper, 10, 126);
    laWidget_SetSize((laWidget*)PulseCaliper, 300, 20);
    laWidget_SetScheme((laWidget*)PulseCaliper, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)PulseCaliper, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)PulseCaliper, LA_WIDGET_BORDER_NONE);
    laDrawSurfaceWidget_SetDrawCallback(PulseCaliper, &PulseCaliper_DrawNotificationEvent);

    laWidget_AddChild((laWidget*)SquareWavePanel, (laWidget*)PulseCaliper);

    GPSPanel = laWidget_New();
    laWidget_SetPosition((laWidget*)GPSPanel, 0, 26);
    laWidget_SetSize((laWidget*)GPSPanel, 320, 194);
    laWidget_SetScheme((laWidget*)GPSPanel, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)GPSPanel, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)GPSPanel, LA_WIDGET_BORDER_NONE);
    laWidget_AddChild((laWidget*)layer0, GPSPanel);

    WorldMap = laImageWidget_New();
    laWidget_SetPosition((laWidget*)WorldMap, 10, 1);
    laWidget_SetSize((laWidget*)WorldMap, 300, 154);
    laWidget_SetOptimizationFlags((laWidget*)WorldMap, LA_WIDGET_OPT_LOCAL_REDRAW | LA_WIDGET_OPT_OPAQUE);
    laWidget_SetScheme((laWidget*)WorldMap, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)WorldMap, LA_WIDGET_BACKGROUND_NONE);
    laWidget_SetBorderType((laWidget*)WorldMap, LA_WIDGET_BORDER_NONE);
    laImageWidget_SetImage(WorldMap, &RobinsonMap);
    laWidget_AddChild((laWidget*)GPSPanel, (laWidget*)WorldMap);

    WorldMapCursor = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)WorldMapCursor, 95, 105);
    laWidget_SetSize((laWidget*)WorldMapCursor, 20, 20);
    laWidget_SetScheme((laWidget*)WorldMapCursor, &WorldMapCursorScheme);
    laWidget_SetBackgroundType((laWidget*)WorldMapCursor, LA_WIDGET_BACKGROUND_NONE);
    laWidget_SetBorderType((laWidget*)WorldMapCursor, LA_WIDGET_BORDER_NONE);
    laWidget_SetMargins((laWidget*)WorldMapCursor, 0, 0, 0, 1);
    laLabelWidget_SetText(WorldMapCursor, laString_CreateFromID(string_WorldMapCursorChar));
    laLabelWidget_SetVAlignment(WorldMapCursor, LA_VALIGN_BOTTOM);
    laWidget_AddChild((laWidget*)WorldMap, (laWidget*)WorldMapCursor);

    GPSLatitude = laDrawSurfaceWidget_New();
    laWidget_SetPosition((laWidget*)GPSLatitude, 5, 159);
    laWidget_SetSize((laWidget*)GPSLatitude, 75, 20);
    laWidget_SetScheme((laWidget*)GPSLatitude, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)GPSLatitude, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)GPSLatitude, LA_WIDGET_BORDER_NONE);
    laDrawSurfaceWidget_SetDrawCallback(GPSLatitude, &GPSLatitude_DrawNotificationEvent);

    laWidget_AddChild((laWidget*)GPSPanel, (laWidget*)GPSLatitude);

    GPSLongitude = laDrawSurfaceWidget_New();
    laWidget_SetPosition((laWidget*)GPSLongitude, 80, 159);
    laWidget_SetSize((laWidget*)GPSLongitude, 75, 20);
    laWidget_SetScheme((laWidget*)GPSLongitude, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)GPSLongitude, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)GPSLongitude, LA_WIDGET_BORDER_NONE);
    laDrawSurfaceWidget_SetDrawCallback(GPSLongitude, &GPSLongitude_DrawNotificationEvent);

    laWidget_AddChild((laWidget*)GPSPanel, (laWidget*)GPSLongitude);

    GPSTime = laDrawSurfaceWidget_New();
    laWidget_SetPosition((laWidget*)GPSTime, 195, 159);
    laWidget_SetSize((laWidget*)GPSTime, 120, 20);
    laWidget_SetScheme((laWidget*)GPSTime, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)GPSTime, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)GPSTime, LA_WIDGET_BORDER_NONE);
    laDrawSurfaceWidget_SetDrawCallback(GPSTime, &GPSTime_DrawNotificationEvent);

    laWidget_AddChild((laWidget*)GPSPanel, (laWidget*)GPSTime);

    LEDTestPanel = laWidget_New();
    laWidget_SetPosition((laWidget*)LEDTestPanel, 0, 26);
    laWidget_SetSize((laWidget*)LEDTestPanel, 320, 194);
    laWidget_SetScheme((laWidget*)LEDTestPanel, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)LEDTestPanel, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)LEDTestPanel, LA_WIDGET_BORDER_NONE);
    laWidget_AddChild((laWidget*)layer0, LEDTestPanel);

    IVPanel = laWidget_New();
    laWidget_SetPosition((laWidget*)IVPanel, 131, 10);
    laWidget_SetSize((laWidget*)IVPanel, 170, 140);
    laWidget_SetScheme((laWidget*)IVPanel, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)IVPanel, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)IVPanel, LA_WIDGET_BORDER_LINE);
    laWidget_AddChild((laWidget*)LEDTestPanel, IVPanel);

    mA10 = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)mA10, 110, 93);
    laWidget_SetSize((laWidget*)mA10, 20, 25);
    laWidget_SetScheme((laWidget*)mA10, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)mA10, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)mA10, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetText(mA10, laString_CreateFromID(string_n10String));
    laLabelWidget_SetHAlignment(mA10, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)mA10);

    mA20 = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)mA20, 110, 47);
    laWidget_SetSize((laWidget*)mA20, 20, 25);
    laWidget_SetScheme((laWidget*)mA20, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)mA20, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)mA20, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetText(mA20, laString_CreateFromID(string_n20String));
    laLabelWidget_SetHAlignment(mA20, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)mA20);

    mA30 = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)mA30, 110, 0);
    laWidget_SetSize((laWidget*)mA30, 20, 25);
    laWidget_SetScheme((laWidget*)mA30, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)mA30, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)mA30, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetText(mA30, laString_CreateFromID(string_n30String));
    laLabelWidget_SetHAlignment(mA30, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)mA30);

    mALabel = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)mALabel, 93, 70);
    laWidget_SetSize((laWidget*)mALabel, 19, 25);
    laWidget_SetScheme((laWidget*)mALabel, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)mALabel, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)mALabel, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetText(mALabel, laString_CreateFromID(string_mAString));
    laLabelWidget_SetHAlignment(mALabel, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)mALabel);

    Vf1_0 = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)Vf1_0, 175, 150);
    laWidget_SetSize((laWidget*)Vf1_0, 28, 25);
    laWidget_SetScheme((laWidget*)Vf1_0, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)Vf1_0, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)Vf1_0, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetText(Vf1_0, laString_CreateFromID(string_n1_0String));
    laLabelWidget_SetHAlignment(Vf1_0, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)Vf1_0);

    Vf2_0 = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)Vf2_0, 232, 150);
    laWidget_SetSize((laWidget*)Vf2_0, 28, 25);
    laWidget_SetScheme((laWidget*)Vf2_0, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)Vf2_0, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)Vf2_0, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetText(Vf2_0, laString_CreateFromID(string_n2_0String));
    laLabelWidget_SetHAlignment(Vf2_0, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)Vf2_0);

    Vf3_0 = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)Vf3_0, 289, 150);
    laWidget_SetSize((laWidget*)Vf3_0, 28, 25);
    laWidget_SetScheme((laWidget*)Vf3_0, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)Vf3_0, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)Vf3_0, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetText(Vf3_0, laString_CreateFromID(string_n3_0String));
    laLabelWidget_SetHAlignment(Vf3_0, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)Vf3_0);

    VfLabel = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)VfLabel, 209, 166);
    laWidget_SetSize((laWidget*)VfLabel, 23, 25);
    laWidget_SetScheme((laWidget*)VfLabel, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)VfLabel, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)VfLabel, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetText(VfLabel, laString_CreateFromID(string_VfString));
    laLabelWidget_SetHAlignment(VfLabel, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)VfLabel);

    ZeroLabel = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)ZeroLabel, 120, 150);
    laWidget_SetSize((laWidget*)ZeroLabel, 16, 25);
    laWidget_SetScheme((laWidget*)ZeroLabel, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)ZeroLabel, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)ZeroLabel, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetText(ZeroLabel, laString_CreateFromID(string_n0String));
    laLabelWidget_SetHAlignment(ZeroLabel, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)ZeroLabel);

    IColonLabel = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)IColonLabel, 9, 56);
    laWidget_SetSize((laWidget*)IColonLabel, 29, 25);
    laWidget_SetScheme((laWidget*)IColonLabel, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)IColonLabel, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)IColonLabel, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetText(IColonLabel, laString_CreateFromID(string_IColonString));
    laLabelWidget_SetHAlignment(IColonLabel, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)IColonLabel);

    VfColonLabel = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)VfColonLabel, 9, 86);
    laWidget_SetSize((laWidget*)VfColonLabel, 29, 25);
    laWidget_SetScheme((laWidget*)VfColonLabel, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)VfColonLabel, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)VfColonLabel, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetText(VfColonLabel, laString_CreateFromID(string_VfColonString));
    laLabelWidget_SetHAlignment(VfColonLabel, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)VfColonLabel);

    VfSetting = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)VfSetting, 39, 86);
    laWidget_SetSize((laWidget*)VfSetting, 51, 25);
    laWidget_SetScheme((laWidget*)VfSetting, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)VfSetting, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)VfSetting, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetHAlignment(VfSetting, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)VfSetting);

    mASetting = laLabelWidget_New();
    laWidget_SetPosition((laWidget*)mASetting, 39, 56);
    laWidget_SetSize((laWidget*)mASetting, 51, 25);
    laWidget_SetScheme((laWidget*)mASetting, &InverseTextScheme);
    laWidget_SetBackgroundType((laWidget*)mASetting, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)mASetting, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetHAlignment(mASetting, LA_HALIGN_LEFT);
    laWidget_AddChild((laWidget*)LEDTestPanel, (laWidget*)mASetting);

    TerminalPanel = laWidget_New();
    laWidget_SetPosition((laWidget*)TerminalPanel, 0, 26);
    laWidget_SetSize((laWidget*)TerminalPanel, 320, 194);
    laWidget_SetScheme((laWidget*)TerminalPanel, &PanelScheme);
    laWidget_SetBackgroundType((laWidget*)TerminalPanel, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)TerminalPanel, LA_WIDGET_BORDER_NONE);
    laWidget_AddChild((laWidget*)layer0, TerminalPanel);

    HelpPanel = laWidget_New();
    laWidget_SetPosition((laWidget*)HelpPanel, 0, 26);
    laWidget_SetSize((laWidget*)HelpPanel, 320, 194);
    laWidget_SetBackgroundType((laWidget*)HelpPanel, LA_WIDGET_BACKGROUND_NONE);
    laWidget_SetBorderType((laWidget*)HelpPanel, LA_WIDGET_BORDER_NONE);
    laWidget_AddChild((laWidget*)layer0, HelpPanel);

    HelpTextWidget = laLabelWidget_New();
    laWidget_SetSize((laWidget*)HelpTextWidget, 320, 194);
    laWidget_SetScheme((laWidget*)HelpTextWidget, &NormalTextScheme);
    laWidget_SetBackgroundType((laWidget*)HelpTextWidget, LA_WIDGET_BACKGROUND_FILL);
    laWidget_SetBorderType((laWidget*)HelpTextWidget, LA_WIDGET_BORDER_NONE);
    laLabelWidget_SetHAlignment(HelpTextWidget, LA_HALIGN_LEFT);
    laLabelWidget_SetVAlignment(HelpTextWidget, LA_VALIGN_TOP);
    laWidget_AddChild((laWidget*)HelpPanel, (laWidget*)HelpTextWidget);

}




