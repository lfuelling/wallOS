#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <debug.h>
#include <WiFi.h>
#include <XPT2046_Touchscreen.h>
#include <fonts/FreeSans9pt7b.h>
#include <fonts/FreeMono9pt7b.h>
#include "../config.h"
#include "../WPBME280/WPBME280.h"
#include "Gidugu10pt7b.h"
#include "Electrolize9pt7b.h"

#ifndef WPGFX_H
#define WPGFX_H

// defines for touchscreen
#define ILI9341_ULTRA_DARKGREY 0x632C
#define TS_MINX 230
#define TS_MINY 350
#define TS_MAXX 3700
#define TS_MAXY 3900
#define HAVE_TOUCHPAD

class WPGFX
{
    Adafruit_ILI9341 *tft;
    XPT2046_Touchscreen *touch;
    WPBME280 *bme280;

    // current location
    int tsx, tsy, tsxraw, tsyraw;
    // current page (0 means main menu)
    int currentPage = 0;
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

    void drawStatusBar();
    String getCurrentTime();
    String getSignalStrength();
    String getTemperature();

public:
    WPGFX(WPBME280 *bme280);
    void begin();
    void display_right(int X, int Y, String Val);
    void drawposition2(uint16_t xraw, uint16_t yraw, uint16_t X, uint16_t Y, bool Down);
    void drawposition1(uint16_t xraw, uint16_t yraw, uint16_t X, uint16_t Y, bool Down);
    void draw_screen(uint8_t red);
    void drawBootScreen();
    void handleGraphics();
    void printError(String error);
    void setBootStatus(String status);
};

#endif