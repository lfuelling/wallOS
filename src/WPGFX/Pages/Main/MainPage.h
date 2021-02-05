#include "../../../debug.h"
#include "../../../config.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include "../../Fonts/FreeSans9pt7b.h"
#include "../../Fonts/FreeMono9pt7b.h"
#include "../../ScreenUtils.h"
#include "../../../WPMQTT/WPMQTT.h"
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

    void drawSettingsButton(int w, int h);
    void drawSwitchesButton(int w, int h);
    void drawBackButton(int w, int h);
    
    void drawSwitchesPage();
    void drawSwitchButton(MqttSwitch currentSwitch);
    void drawNextSwitchButton();
    void drawPreviousSwitchButton();

public:
    MainPage(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor);
    void drawMainPage();
};

#endif
