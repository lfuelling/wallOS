#include "WPGFX/Pages/MainPage.h"

int resetPressCount = 0;
bool holdingResetSwitch = false;

void resetStorage()
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

void MainPage::resetSettingsPage()
{
    holdingResetSwitch = false;
    resetPressCount = 0;
}

void MainPage::drawSettingsPage()
{
    tft->setFont(&FreeSans9pt7b);

    String resetText = String("Reset (" + String(-resetPressCount + 10) + ")");

    drawButton(
        8, tft->height() - 90, 90, 82, resetText, [&]() -> void {
        if(resetPressCount >= 10) {
            resetStorage();
        } else if(!holdingResetSwitch) {
            resetPressCount++;
        } }, [&]() -> void { holdingResetSwitch = false; });
}