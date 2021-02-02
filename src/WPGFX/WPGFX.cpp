#include "WPGFX.h"

WPGFX::WPGFX(WPBME280 *bme)
{
    bme280 = bme;
    touch = new XPT2046_Touchscreen(TOUCH_CS, TOUCH_IRQ);
    tft = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
    mainPage = new MainPage(tft, touch);
}

void WPGFX::begin()
{
    pinMode(TFT_LED, OUTPUT);
    // turn on display lighting
    digitalWrite(TFT_LED, LOW);
    // Start driver
    tft->begin();
    touch->begin();
#ifdef _debug
    // Resolution of the display
    Serial.print("tftx =");
    Serial.print(tft->width());
    Serial.print("tfty =");
    Serial.println(tft->height());
#endif
    tsx = 0;
    tsy = 0;
    tsxraw = 0;
    tsyraw = 0;
    tsdown = false;
    rotation = 1;
}

void WPGFX::printError(String error)
{
    //TODO: implement
}

// current position and contact state
// from the touchscreen
void WPGFX::handleGraphics()
{
    TS_Point p;
    p = touch->getPoint();
    // read current data
    tsxraw = p.x;

    //x and y as raw values
    tsyraw = p.y;
    delay(1);

    // determine the orientation screen
    uint8_t red = tft->getRotation();
    // relative screen positions depending on the orientation
    // determine
    switch (red)
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

    // update touch state
    if (touch->touched() != tsdown)
    {
        tsdown = touch->touched();
    }

    if (lastShownTime != getCurrentTime()          // if time changed
        || lastSignalString != getSignalStrength() // OR the signal strength changed
        || touch->touched() != tsdown)             // OR the touch state changed
    {
        // redraw the screen
        draw_screen(rotation);
    }
}

// Display the main screen
void WPGFX::draw_screen(uint8_t red)
{
    tft->setRotation(red);
    tft->fillScreen(ILI9341_BLACK);
    tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft->setFont(&FreeSans9pt7b);

    drawStatusBar();

    switch (currentPage)
    {
    case 1:
        //TODO: draw settings
        break;
    case 0:
    default:
        mainPage->drawMainPage();
        break;
    }
}

// Display the boot screen
void WPGFX::drawBootScreen()
{
    uint16_t W, h;
    // Select the colors and font orientation
    tft->setRotation(1);
    tft->fillScreen(ILI9341_BLACK);
    tft->setTextColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
    tft->setFont(&FreeMono9pt7b);
    W = tft->width();
    h = tft->height();

    tft->setCursor(W / 2 - 64, h / 2 + 9);
    tft->println("-         -");
    tft->setCursor(W / 2 - 64, h / 2 + 18);
    tft->println(" \\_(*_*)_/ ");

    tft->setFont(&gidugu10pt7b);
    tft->setCursor(W / 2 - 56, h / 2 + 40);
    tft->println(bootStatus);
    tft->setFont(&FreeMono9pt7b);
}

void WPGFX::setBootStatus(String status)
{
    bootStatus = status;
    drawBootScreen();
}