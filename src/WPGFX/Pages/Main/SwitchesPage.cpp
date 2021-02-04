#include "MainPage.h"

void MainPage::drawSwitchesPage()
{
    drawFirstDelockSwitch();
    drawSecondDelockSwitch();
}

void setDelockPowerCommand(String delockKey, String state)
{
    String topic = String("c/" + delockKey + "/POWER");
    bool result = NVS.setString(topic, state);
#ifdef _debug
    if (!result)
    {
        Serial.print("[NVS] write failed! k=");
        Serial.print(topic);
        Serial.print(", v=");
        Serial.println(state);
    }
#endif
}

void MainPage::drawFirstDelockSwitch()
{
    int w = tft->width();
    int h = tft->height();
    uint16_t buttonColor;

    int x1 = 8;
    int y1 = h - 90;
    int btnWidth = 90;
    int btnHeight = 82;
    int x2 = x1 + btnWidth;
    int y2 = y1 + btnHeight;

    String currentState = NVS.getString("delock/POWER");
    bool buttonPressed = touch->touched() && utils.touchedInBounds(tft, touch, x1, y1, x2, y2);

    if (currentState == "ON" || buttonPressed)
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
    tft->setCursor(x1 + (btnWidth / 2) - 34, y1 + (btnHeight / 2) + 6);

    String buttonText;
    if (currentState == "unknown")
    {
        buttonText = "Flur ?";
    }
    else
    {
        buttonText = "Flur";
    }
    tft->print(buttonText);

    if (buttonPressed)
    {
        if (currentState == "ON")
        {
            setDelockPowerCommand("delock", "OFF");
        }
        else if (currentState == "OFF")
        {
            setDelockPowerCommand("delock", "ON");
        }
        else
        {
            setDelockPowerCommand("delock", "ON");
#ifdef _debug
            Serial.println("[Switch] Current state of delock unknown, sending ON!");
#endif
        }
    }
}

void MainPage::drawSecondDelockSwitch()
{
    int w = tft->width();
    int h = tft->height();
    uint16_t buttonColor;

    int x1 = 8;
    int y1 = h - 180; // 8px above first button
    int btnWidth = 90;
    int btnHeight = 82;
    int x2 = x1 + btnWidth;
    int y2 = y1 + btnHeight;

    String currentState = NVS.getString("delock2/POWER");
    bool buttonPressed = touch->touched() && utils.touchedInBounds(tft, touch, x1, y1, x2, y2);

    if (currentState == "ON" || buttonPressed)
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
    tft->setCursor(x1 + (btnWidth / 2) - 34, y1 + (btnHeight / 2) + 6);

    String buttonText;
    if (currentState == "unknown")
    {
        buttonText = "Wohn ?";
    }
    else
    {
        buttonText = "Wohn";
    }
    tft->print(buttonText);

    if (buttonPressed)
    {
        if (currentState == "ON")
        {
            setDelockPowerCommand("delock2", "OFF");
        }
        else if (currentState == "OFF")
        {
            setDelockPowerCommand("delock2", "ON");
        }
        else
        {
            setDelockPowerCommand("delock2", "ON");
#ifdef _debug
            Serial.println("[Switch] Current state of delock2 unknown, sending ON!");
#endif
        }
    }
}
