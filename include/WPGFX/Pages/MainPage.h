#include "debug.h"
#include "config.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include "WPGFX/Fonts/FreeSans9pt7b.h"
#include "WPGFX/Fonts/FreeMono9pt7b.h"
#include "WPGFX/ScreenUtils.h"
#include "WPGFX/Pages/SettingsPage.h"
#include "WPGFX/Pages/SwitchesPage.h"
#include "WPMQTT/WPMQTT.h"
#include <ArduinoNvs.h>

#ifndef WPGFX_MAIN_PAGE_H
#define WPGFX_MAIN_PAGE_H

class MainPage
{
    Adafruit_ILI9341 *tft;
    XPT2046_Touchscreen *touch;
    ScreenUtils *utils;
    SettingsPage *settingsPage;
    SwitchesPage *switchesPage;

    // current page (0 means main menu)
    int currentPage = 0;

public:
    MainPage(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor, ScreenUtils *screenUtils);
    void drawMainPage();
};

#endif
