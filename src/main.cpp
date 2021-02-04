#include "debug.h"
#include "config.h"
#include <Esp.h>
#include <SPI.h>
#include <time.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WPGFX/WPGFX.h>
#include <WPMQTT/WPMQTT.h>
#include <WPBME280/WPBME280.h>
#include <ArduinoNvs.h>

// bme280
WPBME280 wpbme = WPBME280();

// WPMQTT
WPMQTT wpmqtt = WPMQTT(&wpbme);
unsigned long lastTelemetryReport = 0;

// WPGFX
WPGFX wpgfx = WPGFX(&wpbme);
TaskHandle_t graphicsTask;

// loop method that runs the graphics on the other core
void graphicsLoop(void *parameter)
{
  wpgfx.draw_screen(1);
  while (true)
  {
    wpgfx.handleGraphics();
    delay(8);
  }
}

void initializeStorage()
{
#ifdef _debug
  Serial.println("[NVS] Initializing storage...");
#endif
  bool r1 = NVS.setString("lastCommand", "none");
#ifdef _debug
  if (!r1)
  {
    Serial.println("[NVS] write failed! k=lastCommand, v=none");
  }
#endif

  bool r2 = NVS.setString("delock/POWER", "unknown");
#ifdef _debug
  if (!r2)
  {
    Serial.println("[NVS] write failed! k=delock/POWER, v=unknown");
  }
#endif

  bool r3 = NVS.setString("delock2/POWER", "unknown");
#ifdef _debug
  if (!r3)
  {
    Serial.println("[NVS] write failed! k=delock2/POWER, v=unknown");
  }
#endif

  bool r4 = NVS.setString("c/delock/POWER", "none");
#ifdef _debug
  if (!r4)
  {
    Serial.println("[NVS] write failed! k=c/delock/POWER, v=none");
  }
#endif

  bool r5 = NVS.setString("c/delock2/POWER", "none");
#ifdef _debug
  if (!r5)
  {
    Serial.println("[NVS] write failed! k=c/delock2/POWER, v=none");
  }
#endif

  bool r6 = NVS.setInt("shouldRedraw", 0);
#ifdef _debug
  if (!r6)
  {
    Serial.println("[NVS] write failed! k=shouldRedraw, v=0");
  }
#endif
}

// Prepare
void setup()
{
#ifdef _debug
  Serial.begin(115200);
#endif
  bool bootFailed = false;

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
#endif
    delay(800);
  }

  wpgfx.setBootStatus("Syncing NTP...");
  configTime(NTP_UTC_OFFSET_SECONDS, NTP_DAYLIGHT_OFFSET_SECONDS, NTP_SERVER_HOSTNAME);

#ifdef _debug
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("[NTP] Failed to get time using NTP!");
    bootFailed = true; // without NTP, TLS won't work, we can as well just GOTO FAIL :)
  }
  Serial.print("[NTP] Synced: ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
#endif

  wpgfx.setBootStatus("Connecting MQTT...");
  wpmqtt.begin();

  wpgfx.setBootStatus("Opening storage...");
  NVS.begin();

  wpgfx.setBootStatus("Initializing storage...");
  initializeStorage();

  if (bootFailed)
  {
    wpgfx.setBootStatus("Boot failed!");
#ifdef _debug
    Serial.println("BOOT FAILED!");
#endif
    delay(3000);
    ESP.restart();
  }
  else
  {
    wpgfx.setBootStatus("Startup complete!");

    // start gfx loop
    xTaskCreatePinnedToCore(
        graphicsLoop,   /* Function to implement the task */
        "graphicsTask", /* Name of the task */
        10000,          /* Stack size in words */
        NULL,           /* Task input parameter */
        2,              /* Priority of the task */
        &graphicsTask,  /* Task handle. */
        0);             /* Core where the task should run */
  }
}

void loop()
{
  wpmqtt.loop();
  if (millis() - lastTelemetryReport > MQTT_TELEMETRY_INTERVAL_MS)
  {
#ifdef _debug
    Serial.println("[MQTT] sending telemetry...");
#endif
    wpmqtt.publishMessage("tele/wandpanel/temp", String(wpbme.getTemperature()));
    wpmqtt.publishMessage("tele/wandpanel/hum", String(wpbme.getHumidity()));
    wpmqtt.publishMessage("tele/wandpanel/press", String(wpbme.getPressure()));
    wpmqtt.publishMessage("tele/wandpanel/rssi", String(WiFi.RSSI()));
    lastTelemetryReport = millis();
  }
}
