#include "WPGFX/Pages/SwitchesPage.h"

SwitchesPage::SwitchesPage(Adafruit_ILI9341 *screen, XPT2046_Touchscreen *touchSensor, ScreenUtils *screenUtils)
{
    tft = screen;
    touch = touchSensor;
    utils = screenUtils;
}

void SwitchesPage::setSwitchState(String switchName, String state)
{
    bool result = NVS.setString(switchName, state);
#ifdef _debug
    if (!result)
    {
        Serial.print("[NVS] write failed! k=");
        Serial.print(switchName);
        Serial.print(", v=");
        Serial.println(state);
    }
#endif
}

void SwitchesPage::drawSwitchesPage()
{
    if (currentSwitchId >= (sizeof(switches) / sizeof(switches[0])))
    {
#ifdef _debug
        Serial.println("[Switch] Resetting currentSwitchId to 0!");
#endif
        currentSwitchId = 0;
    }

    MqttSwitch currentSwitch = switches[currentSwitchId];
    drawSwitchButton(currentSwitch);
    drawNextButton();
    drawPrevButton();
}

void SwitchesPage::drawNextButton()
{
    utils->drawButton((tft->width() - 72), (tft->height() / 2) - 32, 64, 64, "->", [&]() -> void {
        if(!holdingNextButton) {
            holdingNextButton = true;
            currentSwitchId++;
#ifdef _debug
        Serial.println("[Switch] Navigating to next switch!");
#endif
        } }, [&]() -> void {
        if (holdingNextButton) {
            holdingNextButton = false;
        } });
}

void SwitchesPage::drawPrevButton()
{
    utils->drawButton(
        8, (tft->height() / 2) - 32, 64, 64, "<-", [&]() -> void {
        if (!holdingPrevButton)
        {
            holdingPrevButton = true;
            if (currentSwitchId == 0)
            {
                currentSwitchId = (sizeof(switches) / sizeof(switches[0])) - 1;
            }
            else
            {
                currentSwitchId--;
            }
#ifdef _debug
            Serial.println("[Switch] Navigating to previous switch!");
#endif
        } }, [&]() -> void {
        if (holdingPrevButton) {
            holdingPrevButton = false;
        } });
}

void SwitchesPage::drawSwitchButton(MqttSwitch currentSwitch)
{
    uint16_t buttonColor;
    uint16_t fontColor;

    int x1 = 95;
    int y1 = 56;
    int btnWidth = 128;
    int btnHeight = 128;
    int x2 = x1 + btnWidth;
    int y2 = y1 + btnHeight;

    String currentState = NVS.getString(String("s/" + currentSwitch.switchName));
    bool buttonPressed = touch->touched() && utils->touchedInBounds(x1, y1, x2, y2);

    if (currentState == currentSwitch.powerOnValue || buttonPressed)
    {
        buttonColor = 0xC618; // lightgrey
        fontColor = 0x0000;
    }
    else
    {
        buttonColor = 0x2104; // very dark grey
        fontColor = 0xFFFF;
    }

    // draw settings button
    tft->fillRect(x1, y1, btnWidth, btnHeight, buttonColor);
    tft->setTextColor(fontColor, buttonColor);
    tft->setCursor((x1 + (btnWidth / 2)) - (currentSwitch.switchName.length() * 5), y1 + (btnHeight / 2) + 6);

    String buttonText;
    if (currentState == "unknown")
    {
        buttonText = String(currentSwitch.switchName + " ?");
    }
    else
    {
        buttonText = currentSwitch.switchName;
    }
    tft->print(buttonText);

    if (buttonPressed && !holdingSwitch)
    {
        holdingSwitch = true;
        if (currentState == currentSwitch.powerOnValue)
        {
            setSwitchState(currentSwitch.switchName, currentSwitch.powerOffValue);
        }
        else if (currentState == currentSwitch.powerOffValue)
        {
            setSwitchState(currentSwitch.switchName, currentSwitch.powerOnValue);
        }
        else
        {
            setSwitchState(currentSwitch.switchName, currentSwitch.powerOnValue);
#ifdef _debug
            Serial.println("[Switch] Current state of delock unknown, sending ON!");
#endif
        }
    }
    else if (!buttonPressed && holdingSwitch)
    {
        holdingSwitch = false;
    }
}
