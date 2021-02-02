#include "MainPage.h"

MainPage::MainPage(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor)
{
    tft = screen;
    touch = touchSensor;
    utils = ScreenUtils();
}

void MainPage::drawMainPage()
{
    int w = tft->width();
    int h = tft->height();

    tft->setFont(&FreeSans9pt7b);

    drawSettingsButton(w, h);

    // reset to default values
    tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft->setFont(&FreeSans9pt7b);
}

void MainPage::drawSettingsButton(int w, int h)
{
    uint16_t buttonColor;

    int x1 = 8;
    int y1 = h - 90;
    int btnWidth = 90;
    int btnHeight = 70;
    int x2 = x1 + btnWidth;
    int y2 = y1 + btnHeight;

    if (touch->touched() && utils.touchedInBounds(tft, touch, x1, y1, x2, y2))
    {
        buttonColor = 0xC618; // lightgrey
    }
    else
    {
        buttonColor = 0x2104; // very dark grey
    }

    // draw settings button
    tft->fillRect(x1, y1, btnWidth, btnHeight, buttonColor);
    tft->setTextColor(ILI9341_WHITE, buttonColor);
    tft->setCursor(18, h - 50);
    tft->print("Settings");
}
