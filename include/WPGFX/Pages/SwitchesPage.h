#include "debug.h"
#include "config.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include "WPGFX/Fonts/FreeSans9pt7b.h"
#include "WPGFX/Fonts/FreeMono9pt7b.h"
#include "WPGFX/ScreenUtils.h"
#include <ArduinoNvs.h>

#ifndef WPGFX_SWITCHES_PAGE_H
#define WPGFX_SWITCHES_PAGE_H

class SwitchesPage
{
    Adafruit_ILI9341 *tft;
    XPT2046_Touchscreen *touch;
    ScreenUtils *utils;

    int currentSwitchId = 0;
    bool holdingSwitch = false;
    bool holdingNextButton = false;
    bool holdingPrevButton = false;

    void setSwitchState(String switchName, String state);
    void drawSwitchButton(MqttSwitch currentSwitch);
    void drawNextButton();
    void drawPrevButton();

public:
    SwitchesPage(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor, ScreenUtils *screenUtils);
    void drawSwitchesPage();
    void resetSettingsPage();
};

#endif
