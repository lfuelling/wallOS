#include <WiFi.h>
#include <Esp.h>
#include <debug.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeMono9pt7b.h"
#include "Fonts/Gidugu10pt7b.h"
#include "Fonts/Electrolize9pt7b.h"
#include "../config.h"
#include "../WPBME280/WPBME280.h"
#include "../WPMQTT/WPMQTT.h"
#include "Pages/MainPage.h"

#ifndef WPGFX_H
#define WPGFX_H

// defines for touchscreen
#define ILI9341_ULTRA_DARKGREY 0x632C
#define HAVE_TOUCHPAD

class WPGFX
{
    MainPage *mainPage;

    // current location
    int tsx, tsy, tsxraw, tsyraw;
    // current touch state
    bool tsdown = false;
    // current screen orientation
    uint8_t rotation = 1;
    // boot status string
    String bootStatus = "Booting...";

    // last shown time
    String lastShownTime = "";
    // last wifi signal string
    String lastSignalString = "----";
    // last shown temperature string
    String lastTemperature = "";

    void redrawIfNecessary();
    void drawStatusBar();
    String getCurrentTime();
    String getSignalStrength();
    String getTemperature();

public:
    Adafruit_ILI9341 *tft;
    XPT2046_Touchscreen *touch;
    WPBME280 *bme280;

    WPGFX(WPBME280 *bme280);
    void begin();
    void display_right(int X, int Y, String Val);
    void drawposition2(uint16_t xraw, uint16_t yraw, uint16_t X, uint16_t Y, bool Down);
    void drawposition1(uint16_t xraw, uint16_t yraw, uint16_t X, uint16_t Y, bool Down);
    void draw_screen(uint8_t red);
    void drawBootScreen();
    void handleGraphics();
    void showFatalError(String error, bool reboot);
    void setBootStatus(String status);
    void drawInitialSetupScreen(String ssid, String password, String ip);
};

#endif
