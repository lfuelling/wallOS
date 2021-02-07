#include "debug.h"
#include "config.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include "WPGFX/Fonts/FreeSans9pt7b.h"
#include "WPGFX/Fonts/FreeMono9pt7b.h"
#include "WPGFX/ScreenUtils.h"
#include "WPMQTT/WPMQTT.h"
#include <ArduinoNvs.h>

#ifndef WPGFX_SETTINGS_PAGE_H
#define WPGFX_SETTINGS_PAGE_H

class SettingsPage
{
    Adafruit_ILI9341 *tft;
    XPT2046_Touchscreen *touch;
    ScreenUtils *utils;

    void resetStorage();

public:
    SettingsPage(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor, ScreenUtils *screenUtils);
    void drawSettingsPage();
    void resetSettingsPage();
};

#endif
