#include "debug.h"
#include "config.h"
#include <SPI.h>
#include <time.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WPGFX/WPGFX.h>
#include <WPMQTT/WPMQTT.h>
#include <WPBME280/WPBME280.h>

// bme280
WPBME280 wpbme = WPBME280();

// WPGFX
WPGFX wpgfx = WPGFX(&wpbme);
TaskHandle_t graphicsTask;

// WPMQTT
WiFiClientSecure wifiClient;
WPMQTT wpmqtt = WPMQTT(&wpbme);

// loop method that runs the graphics on the other core
void graphicsLoop(void *parameter)
{
  wpgfx.draw_screen(1);
  while (true)
  {
    wpgfx.handleGraphics();
    delay(100);
  }
}

// Prepare
void setup()
{
#ifdef _debug
  Serial.begin(115200);
#endif

  // show bootscreen
  wpgfx.begin();
  wpgfx.drawBootScreen();
  delay(100);

  // init bme
  wpgfx.setBootStatus("Loading BME280...");
  wpbme.begin();

  wpgfx.setBootStatus("Connecting WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
#ifdef _debug
    Serial.print("[WiFi] Connecting to ");
    Serial.print(WIFI_SSID);
    Serial.println("...");
    delay(800);
#endif
  }

  wpgfx.setBootStatus("Syncing NTP...");
  configTime(NTP_UTC_OFFSET_SECONDS, NTP_DAYLIGHT_OFFSET_SECONDS, NTP_SERVER_HOSTNAME);

#ifdef _debug
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("[NTP] Failed to get time using NTP!");
    return;
  }
  Serial.print("[NTP] Synced: ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
#endif

  wpgfx.setBootStatus("Connecting MQTT...");
  wpmqtt.begin(wifiClient);

  wpgfx.setBootStatus("Startup complete!");

  // start gfx loop
  xTaskCreatePinnedToCore(
      graphicsLoop,   /* Function to implement the task */
      "graphicsTask", /* Name of the task */
      10000,          /* Stack size in words */
      NULL,           /* Task input parameter */
      0,              /* Priority of the task */
      &graphicsTask,  /* Task handle. */
      0);             /* Core where the task should run */
}

void loop()
{
  
}
