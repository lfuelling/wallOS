#include "MainPage.h"

int currentSwitchId = 0;

void setSwitchState(String switchName, String state)
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

void MainPage::drawSwitchButton(MqttSwitch currentSwitch)
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
    bool buttonPressed = touch->touched() && utils.touchedInBounds(tft, touch, x1, y1, x2, y2);

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

    if (buttonPressed)
    {
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
}

void MainPage::drawSwitchesPage()
{
    if (currentSwitchId >= (sizeof(switches) / sizeof(switches[0])))
    {
#ifdef _debug
        Serial.println("[Switch] Resetting currentSwitchId to 0!");
#endif
        currentSwitchId = 0;
    }

    MqttSwitch currentSwitch = switches[currentSwitchId];
    drawPreviousSwitchButton();
    drawSwitchButton(currentSwitch);
    drawNextSwitchButton();
}

void MainPage::drawPreviousSwitchButton()
{
    uint16_t buttonColor;
    uint16_t fontColor;

    int btnWidth = 32;
    int btnHeight = 32;
    int x1 = 10;
    int y1 = (tft->height() / 2) - (btnHeight / 2);
    int x2 = x1 + btnWidth;
    int y2 = y1 + btnHeight;

    bool buttonPressed = touch->touched() && utils.touchedInBounds(tft, touch, x1, y1, x2, y2);

    if (buttonPressed)
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
    tft->setCursor(x1 + (btnWidth / 2) - 8, y1 + (btnHeight / 2) + 6);
    tft->print("<");

    if (buttonPressed)
    {
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
    }
}

void MainPage::drawNextSwitchButton()
{
    uint16_t buttonColor;
    uint16_t fontColor;

    int btnWidth = 32;
    int btnHeight = 32;
    int x1 = (tft->width() - (btnWidth + 8));
    int y1 = (tft->height() / 2) - (btnHeight / 2);
    int x2 = x1 + btnWidth;
    int y2 = y1 + btnHeight;

    bool buttonPressed = touch->touched() && utils.touchedInBounds(tft, touch, x1, y1, x2, y2);

    if (buttonPressed)
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
    tft->setCursor(x1 + (btnWidth / 2) - 8, y1 + (btnHeight / 2) + 6);
    tft->print(">");

    if (buttonPressed)
    {
        currentSwitchId++;
#ifdef _debug
        Serial.println("[Switch] Navigating to next switch!");
#endif
    }
}
