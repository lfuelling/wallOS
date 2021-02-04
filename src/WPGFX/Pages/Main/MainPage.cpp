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

    switch (currentPage)
    {
    case 2:
        drawSwitchesPage();
        break;
    case 1:
        // TODO: draw settings page
        break;
    case 0:
    default:
        // draw main menu
        tft->setFont(&FreeSans9pt7b);
        drawSettingsButton(w, h);
        drawSwitchesButton(w, h);
        break;
    }

    // reset to default values
    tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft->setFont(&FreeSans9pt7b);

    if (currentPage > 0)
    {
        drawBackButton(w, h);
    }
}

void MainPage::drawBackButton(int w, int h)
{
    uint16_t buttonColor;

    int x1 = w - 38;
    int y1 = h - 38;
    int btnWidth = 30;
    int btnHeight = 30;
    int x2 = x1 + btnWidth;
    int y2 = y1 + btnHeight;

    if (touch->touched() && utils.touchedInBounds(tft, touch, x1, y1, x2, y2))
    {
        buttonColor = 0xC618; // lightgrey
        currentPage = 0;      // set current page to main
    }
    else
    {
        buttonColor = 0x2104; // very dark grey
    }

    // draw settings button
    tft->fillRect(x1, y1, btnWidth, btnHeight, buttonColor);
    tft->setTextColor(ILI9341_WHITE, buttonColor);
    tft->setCursor(x1 + (btnWidth / 2) - 8, y1 + (btnHeight / 2) + 4);
    tft->print("<-");
}

void MainPage::drawSettingsButton(int w, int h)
{
    uint16_t buttonColor;

    int x1 = 8;
    int y1 = h - 90;
    int btnWidth = 90;
    int btnHeight = 82;
    int x2 = x1 + btnWidth;
    int y2 = y1 + btnHeight;

    if (touch->touched() && utils.touchedInBounds(tft, touch, x1, y1, x2, y2))
    {
        buttonColor = 0xC618; // lightgrey
        currentPage = 1;      // set current page to settings
    }
    else
    {
        buttonColor = 0x2104; // very dark grey
    }

    // draw settings button
    tft->fillRect(x1, y1, btnWidth, btnHeight, buttonColor);
    tft->setTextColor(ILI9341_WHITE, buttonColor);
    tft->setCursor(x1 + (btnWidth / 2) - 34, y1 + (btnHeight / 2) + 6);
    tft->print("Settings");
}

void MainPage::drawSwitchesButton(int w, int h)
{
    uint16_t buttonColor;

    int x1 = 8;
    int y1 = h - 180; // 8px above settings button
    int btnWidth = 90;
    int btnHeight = 82;
    int x2 = x1 + btnWidth;
    int y2 = y1 + btnHeight;

    if (touch->touched() && utils.touchedInBounds(tft, touch, x1, y1, x2, y2))
    {
        buttonColor = 0xC618; // lightgrey
        currentPage = 2;      // set current page to switches
    }
    else
    {
        buttonColor = 0x2104; // very dark grey
    }

    // draw settings button
    tft->fillRect(x1, y1, btnWidth, btnHeight, buttonColor);
    tft->setTextColor(ILI9341_WHITE, buttonColor);
    tft->setCursor(x1 + (btnWidth / 2) - 37, y1 + (btnHeight / 2) + 6);
    tft->print("Switches");
}
