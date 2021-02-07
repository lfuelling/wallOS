#include "WPGFX/ScreenUtils.h"

ScreenUtils::ScreenUtils(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor)
{
    tft = screen;
    touch = touchSensor;
}

TS_Point ScreenUtils::getTouchPosition()
{
    TS_Point point = touch->getPoint();

    // read current data
    int tsxraw = point.x;
    int tsyraw = point.y;

    int tsx = 0;
    int tsy = 0;

    switch (tft->getRotation())
    {
    case 0:
        tsx = map(tsyraw, TS_MINY, TS_MAXY, 240, 0);
        tsy = map(tsxraw, TS_MINX, TS_MAXX, 0, 320);
        break;
    case 1:
        tsx = map(tsxraw, TS_MINX, TS_MAXX, 0, 320);
        tsy = map(tsyraw, TS_MINY, TS_MAXX, 0, 240);
        break;
    case 2:
        tsx = map(tsyraw, TS_MINY, TS_MAXY, 0, 240);
        tsy = map(tsxraw, TS_MINX, TS_MAXX, 320, 0);
        break;
    case 3:
        tsx = map(tsxraw, TS_MINX, TS_MAXX, 320, 0);
        tsy = map(tsyraw, TS_MINY, TS_MAXY, 240, 0);
        break;
    }

    tsx = (tsx - tft->width()) * -1;
    tsy = (tsy - tft->height()) * -1;

    return TS_Point(tsx, tsy, point.z);
}

bool ScreenUtils::inRange(int low, int x, int high)
{
    return ((x - high) * (x - low) <= 0);
}

bool ScreenUtils::touchedInBounds(int x1, int y1, int x2, int y2)
{
    TS_Point point = getTouchPosition();

    bool inBoundsX = false;
    bool inBoundsY = false;

    if (x1 > x2)
    {
        inBoundsX = inRange(x2, point.x, x1);
    }
    else
    {
        inBoundsX = inRange(x1, point.x, x2);
    }
    if (y1 > y2)
    {
        inBoundsY = inRange(y2, point.y, y1);
    }
    else
    {
        inBoundsY = inRange(y1, point.y, y2);
    }

#ifdef _debug
    Serial.print("[Touch] Bounds: x1=");
    Serial.print(String(x1));
    Serial.print(",y1=");
    Serial.print(String(y1));
    Serial.print(",x2=");
    Serial.print(String(x2));
    Serial.print(",y2=");
    Serial.println(String(y2));

    Serial.print("[Touch] Touch: x=");
    Serial.print(String(point.x));
    Serial.print(", y=");
    Serial.println(String(point.y));

    Serial.print("[Touch] Result: inBoundsX=");
    Serial.print(String(inBoundsX));
    Serial.print(",inBoundsY=");
    Serial.println(String(inBoundsY));
#endif

    return inBoundsX && inBoundsY;
}

void ScreenUtils::drawButton(int x, int y, int width, int height, String text, std::function<void()> onTouch, std::function<void()> onNoTouch)
{
    uint16_t buttonColor;
    uint16_t fontColor;

    int x1 = x;
    int y1 = y;
    int btnWidth = width;
    int btnHeight = height;
    int x2 = x1 + btnWidth;
    int y2 = y1 + btnHeight;

    bool buttonTouched = touch->touched() && touchedInBounds(x1, y1, x2, y2);

    if (buttonTouched)
    {
        buttonColor = 0xC618; // lightgrey
        fontColor = 0x0000;
    }
    else
    {
        buttonColor = 0x2104; // very dark grey
        fontColor = 0xFFFF;
    }

    tft->fillRect(x1, y1, btnWidth, btnHeight, buttonColor);
    tft->setTextColor(fontColor, buttonColor);
    tft->setCursor((x1 + (btnWidth / 2)) - ((text.length() * 4) + 3), y1 + (btnHeight / 2) + 6);
    tft->print(text);

    if (buttonTouched)
    {
        onTouch();
    }
    else
    {
        onNoTouch();
    }
}