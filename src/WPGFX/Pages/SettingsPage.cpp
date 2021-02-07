#include "WPGFX/Pages/SettingsPage.h"

int resetPressCount = 0;
bool holdingResetSwitch = false;

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

void SettingsPage::drawSettingsPage()
{
    tft->setFont(&FreeSans9pt7b);

    String resetText = String("Reset (" + String(-resetPressCount + 10) + ")");

    utils->drawButton(
        8, tft->height() - 90, 90, 82, resetText, [&]() -> void {
        if(resetPressCount >= 10) {
            resetStorage();
        } else if(!holdingResetSwitch) {
            resetPressCount++;
        } }, [&]() -> void { holdingResetSwitch = false; });
}