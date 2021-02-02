#include <debug.h>
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include "../config.h"

#ifndef SCREENUTILS_H
#define SCREENUTILS_H

// Touch adapter bounds
#define TS_MINX 230
#define TS_MINY 350
#define TS_MAXX 3700
#define TS_MAXY 3900

class ScreenUtils
{
    bool inRange(int low, int x, int high);

public:
    ScreenUtils();
    TS_Point getTouchPosition(Adafruit_ILI9341 *tft, XPT2046_Touchscreen *touch);
    bool touchedInBounds(Adafruit_ILI9341 *tft, XPT2046_Touchscreen *touch, int x1, int y1, int x2, int y2);
};

#endif
