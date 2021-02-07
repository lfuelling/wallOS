#include <debug.h>
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include "../config.h"
#include <Arduino.h>
#include <functional>

#ifndef SCREENUTILS_H
#define SCREENUTILS_H

// Touch adapter bounds
#define TS_MINX 230
#define TS_MINY 350
#define TS_MAXX 3700
#define TS_MAXY 3900

class ScreenUtils
{
    Adafruit_ILI9341 *tft;
    XPT2046_Touchscreen *touch;

    bool inRange(int low, int x, int high);

public:
    ScreenUtils(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor);
    TS_Point getTouchPosition();
    bool touchedInBounds(int x1, int y1, int x2, int y2);
    void drawButton(
        int x, int y, int width, int height, String text,
        std::function<void()> onTouch,
        std::function<void()> onNoTouch = []() -> void {});
};

#endif
