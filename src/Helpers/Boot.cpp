#include "Helpers/Boot.h"

Boot::Boot(WPGFX *gfx, Storage *s, WPBME280 *bme, WPMQTT *mqtt)
{
    wpgfx = gfx;
    storage = s;
    wpbme = bme;
    wpmqtt = mqtt;
}

bool Boot::bootSuccessful()
{
    wpgfx->setBootStatus("Initializing storage...");
    storage->initializeStorage();

    // init bme
    wpgfx->setBootStatus("Loading BME280...");
    wpbme->begin();

    wpgfx->setBootStatus("Connecting WiFi...");
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
    String wifiSsid = NVS.getString("conf/ssid");
    String wifiPass = NVS.getString("conf/pass");
    String wifiHost = NVS.getString("conf/host");
    WiFi.begin(wifiSsid.c_str(), wifiPass.c_str());
    WiFi.setHostname(wifiHost.c_str());
    int wifiConnectCount = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
#ifdef _debug
        Serial.print("[WiFi] Connecting to ");
        Serial.print(wifiSsid);
        Serial.println("...");
#endif
        delay(800);
        wifiConnectCount++;
        if (wifiConnectCount > 200)
        {
            return false;
        }
    }

    wpgfx->setBootStatus("Syncing NTP...");
    configTime(NTP_UTC_OFFSET_SECONDS, NTP_DAYLIGHT_OFFSET_SECONDS, NTP_SERVER_HOSTNAME);

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
#ifdef _debug
        Serial.println("[NTP] Failed to get time using NTP!");
#endif
        wpgfx->showFatalError("Error syncing NTP!", true); // TLS won't work without NTP, we can as well just GOTO FAIL :)
    }
#ifdef _debug
    Serial.print("[NTP] Synced: ");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
#endif

    wpgfx->setBootStatus("Connecting MQTT...");
    if (!wpmqtt->begin())
    {
        return false;
    }

    wpgfx->setBootStatus("Startup complete!");

    return true;
}

void Boot::boot()
{
    if (!bootSuccessful())
    {
#ifdef _debug
        Serial.println("Boot failed!");
#endif
        wpgfx->showFatalError("Boot failed! Launching Setup...", false);
#ifdef _debug
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
}