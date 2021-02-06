#include "WPGFX/WPGFX.h"

void WPGFX::drawStatusBar()
{
    int w = tft->width();
    tft->setFont(&electrolize9pt7b);
    tft->fillRect(0, 0, w, 30, 0x2104);
    tft->setTextColor(ILI9341_WHITE, 0x2104);
    tft->setCursor(w / 2 - 20, 20);
    String currentTime = getCurrentTime();
    tft->print(currentTime);
    lastShownTime = currentTime;

    tft->setCursor(8, 20);
    String signalStrength = getSignalStrength();
    tft->print(signalStrength);
    lastSignalString = signalStrength;

    tft->setCursor(w - 42, 20);
    String temperatureString = getTemperature();
    tft->print(temperatureString);
    tft->print("C");
    lastTemperature = temperatureString;

    // reset to default values
    tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft->setFont(&FreeSans9pt7b);
}

String WPGFX::getTemperature()
{
    String temperatureString = "";
    if (bme280 != NULL && bme280->isStarted())
    {
        temperatureString = String(bme280->getTemperature());
        temperatureString.remove(2);
    }
    return temperatureString;
}

String WPGFX::getSignalStrength()
{
    String signalString;
    int rssi = WiFi.RSSI();
    if (rssi < -100)
    {
        signalString = "|---";
    }
    else if (rssi < -90)
    {
        signalString = "||--";
    }
    else if (rssi < -70)
    {
        signalString = "|||-";
    }
    else
    {
        signalString = "||||";
    }
    return signalString;
}

String WPGFX::getCurrentTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
#ifdef _debug
        Serial.println("Failed to get current time!");
        return "XX:XX";
#endif
    }
    char buff[5];
    strftime(buff, 20, "%H:%M ", &timeinfo);
    return buff;
}
