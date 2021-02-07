#include "WPGFX/Pages/SettingsPage.h"

int resetPressCount = 0;
bool holdingResetSwitch = false;
bool holdingFrameSwitch = false;

SettingsPage::SettingsPage(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor, ScreenUtils *screenUtils)
{
    tft = screen;
    touch = touchSensor;
    utils = screenUtils;
}

void SettingsPage::resetStorage()
{
#ifdef _debug
    Serial.println("Reset requested!");
    Serial.println("Erasing NVS...");
#endif
    NVS.eraseAll(true);
    delay(20);
#ifdef _debug
    Serial.println("Closing NVS...");
#endif
    NVS.close();
    delay(250);
#ifdef _debug
    Serial.println("Rebooting...");
#endif
    delay(20);
    ESP.restart();
}

void SettingsPage::resetSettingsPage()
{
    holdingResetSwitch = false;
    resetPressCount = 0;
}

void SettingsPage::drawFrameButton()
{
    String resetText = String("Frames");

    utils->drawButton(
        8, tft->height() - 180, 90, 82, resetText,
        [&]() -> void {
            if (!holdingResetSwitch)
            {
                holdingResetSwitch = true;
                if (NVS.getInt("conf/ui/frames") != 1)
                {
                    NVS.setInt("conf/ui/frames", 1);
                }
                else
                {
                    NVS.setInt("conf/ui/frames", 0);
                }
            }
        },
        [&]() -> void {
            if (holdingFrameSwitch)
            {
                holdingFrameSwitch = false;
            }
        });
}

void SettingsPage::drawResetButton()
{
    String resetText = String("Reset (" + String(-resetPressCount + 10) + ")");

    utils->drawButton(
        8, tft->height() - 90, 90, 82, resetText, [&]() -> void {
        if(resetPressCount >= 10) {
            resetStorage();
        } else if(!holdingResetSwitch) {
            resetPressCount++;
        } },
        [&]() -> void {
            if (holdingResetSwitch)
            {
                holdingResetSwitch = false;
            }
        });
}

void SettingsPage::drawSettingsPage()
{
    tft->setFont(&FreeSans9pt7b);
    drawResetButton();
    drawFrameButton();
}