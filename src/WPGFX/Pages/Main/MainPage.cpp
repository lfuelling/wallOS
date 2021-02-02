#include "MainPage.h"

MainPage::MainPage(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor)
{
    tft = screen;
    touch = touchSensor;
}

void MainPage::drawMainPage()
{
    int w = tft->width();
    int h = tft->height();

    tft->setFont(&FreeSans9pt7b);

    // draw settings button
    tft->fillRect(8, h - 90, 90, 70, 0x2104);
    tft->setTextColor(ILI9341_WHITE, 0x2104);
    tft->setCursor(18, h - 50);
    tft->print("Settings");

    // reset to default values
    tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft->setFont(&FreeSans9pt7b);
}
