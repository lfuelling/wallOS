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

#ifndef WPGFX_MAIN_PAGE_H
#define WPGFX_MAIN_PAGE_H

class MainPage
{
    Adafruit_ILI9341 *tft;
    XPT2046_Touchscreen *touch;
    ScreenUtils utils;

    // current page (0 means main menu)
    int currentPage = 0;

    void drawSwitchesPage();
    void drawSettingsPage();
    void resetSettingsPage();
    void drawSwitchButton(MqttSwitch currentSwitch);

    void drawButton(
        int x, int y, int width, int height, String text,
        std::function<void()> onTouch,
        std::function<void()> onNoTouch = []() -> void {});

public:
    MainPage(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor);
    void drawMainPage();
};

#endif
