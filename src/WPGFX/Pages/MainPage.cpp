#include "WPGFX/Pages/MainPage.h"

MainPage::MainPage(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor, ScreenUtils *screenUtils)
{
    tft = screen;
    touch = touchSensor;
    utils = screenUtils;

    settingsPage = new SettingsPage(tft, touch, utils);
    switchesPage = new SwitchesPage(tft, touch, utils);
}

void MainPage::drawMainPage()
{
    int w = tft->width();
    int h = tft->height();

    switch (currentPage)
    {
    case 2:
        switchesPage->drawSwitchesPage();
        break;
    case 1:
        settingsPage->drawSettingsPage();
        break;
    case 0:
    default:
        // draw main menu
        tft->setFont(&FreeSans9pt7b);
        utils->drawButton(8, h - 90, 90, 82, "Settings", [&]() -> void { currentPage = 1; });
        utils->drawButton(8, h - 180, 90, 82, "Switches", [&]() -> void { currentPage = 2; });
        break;
    }

    // reset to default values
    tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft->setFont(&FreeSans9pt7b);

    if (currentPage > 0)
    {
        utils->drawButton(w - 38, h - 38, 30, 30, "<-", [&]() -> void {
            if (currentPage == 1)
            {
                settingsPage->resetSettingsPage();
            }
            currentPage = 0;
        });
    }
}
