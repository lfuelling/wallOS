#include "WPGFX/Pages/MainPage.h"

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
        drawSettingsPage();
        break;
    case 0:
    default:
        // draw main menu
        tft->setFont(&FreeSans9pt7b);
        drawButton(8, h - 90, 90, 82, "Settings", [&]() -> void { currentPage = 1; });
        drawButton(8, h - 180, 90, 82, "Switches", [&]() -> void { currentPage = 2; });
        break;
    }

    // reset to default values
    tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft->setFont(&FreeSans9pt7b);

    if (currentPage > 0)
    {
        drawButton(w - 38, h - 38, 30, 30, "<-", [&]() -> void {
            if (currentPage == 1)
            {
                resetSettingsPage();
            }
            currentPage = 0;
        });
    }
}

void MainPage::drawButton(int x, int y, int width, int height, String text, std::function<void()> onTouch, std::function<void()> onNoTouch)
{
    uint16_t buttonColor;
    uint16_t fontColor;

    int x1 = x;
    int y1 = y;
    int btnWidth = width;
    int btnHeight = height;
    int x2 = x1 + btnWidth;
    int y2 = y1 + btnHeight;

    bool buttonTouched = touch->touched() && utils.touchedInBounds(tft, touch, x1, y1, x2, y2);

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
