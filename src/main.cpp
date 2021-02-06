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

// Photoresistor
int photoresistorVal;

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

  for (int i = 0; i < (sizeof(switches) / sizeof(switches[0])); i++)
  {
    MqttSwitch s = switches[i];
    bool rs1 = NVS.setString(String("s/" + s.switchName), "unknown");
    bool rs2 = NVS.setString(s.switchName, "unknown");
#ifdef _debug
    if (!rs1)
    {
      Serial.print("[NVS] write failed! k=");
      Serial.print("s/" + s.switchName);
      Serial.println(", v=unknown");
    }
    if (!rs2)
    {
      Serial.print("[NVS] write failed! k=");
      Serial.print(s.switchName);
      Serial.println(", v=unknown");
    }
#endif
  }

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

void publishTelemetry() {
  #ifdef _debug
    Serial.println("[MQTT] sending telemetry...");
#endif
    // sensor values
    wpmqtt.publishMessage("tele/wandpanel/temp", String(wpbme.getTemperature()));
    wpmqtt.publishMessage("tele/wandpanel/hum", String(wpbme.getHumidity()));
    wpmqtt.publishMessage("tele/wandpanel/press", String(wpbme.getPressure()));
    wpmqtt.publishMessage("tele/wandpanel/rssi", String(WiFi.RSSI()));
    wpmqtt.publishMessage("tele/wandpanel/photo", String(photoresistorVal));

    // ESP internal values
    wpmqtt.publishMessage("tele/wandpanel/heap/free", String(ESP.getFreeHeap()));
    wpmqtt.publishMessage("tele/wandpanel/heap/total", String(ESP.getHeapSize()));
    wpmqtt.publishMessage("tele/wandpanel/heap/min-free", String(ESP.getMinFreeHeap()));
    wpmqtt.publishMessage("tele/wandpanel/heap/max-alloc", String(ESP.getMaxAllocHeap()));
    wpmqtt.publishMessage("tele/wandpanel/psram/free", String(ESP.getFreePsram()));
    wpmqtt.publishMessage("tele/wandpanel/psram/total", String(ESP.getPsramSize()));
    wpmqtt.publishMessage("tele/wandpanel/psram/min-free", String(ESP.getMinFreePsram()));
    wpmqtt.publishMessage("tele/wandpanel/psram/max-alloc", String(ESP.getMaxAllocPsram()));
    wpmqtt.publishMessage("tele/wandpanel/chip-revision", String(ESP.getChipRevision()));
    wpmqtt.publishMessage("tele/wandpanel/cpu-freq", String(ESP.getCpuFreqMHz()));
    wpmqtt.publishMessage("tele/wandpanel/flash/size", String(ESP.getFlashChipSize()));
    wpmqtt.publishMessage("tele/wandpanel/flash/speed", String(ESP.getFlashChipSpeed()));
    wpmqtt.publishMessage("tele/wandpanel/sketch/size", String(ESP.getSketchSize()));
    wpmqtt.publishMessage("tele/wandpanel/sketch/free", String(ESP.getFreeSketchSpace()));
    wpmqtt.publishMessage("tele/wandpanel/sketch/md5", String(ESP.getSketchMD5()));
    lastTelemetryReport = millis();
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

  // init storage
  wpgfx.setBootStatus("Opening storage...");
  if (!NVS.begin())
  {
#ifdef _debug
    Serial.println("Error while initializing NVS!");
#endif
    wpgfx.showFatalError("Error accessing storage!", true);
  }

  wpgfx.setBootStatus("Initializing storage...");
  initializeStorage();

  // init photoresistor
  wpgfx.setBootStatus("Photoresistor...");
  photoresistorVal = analogRead(PHOTORESISTOR_PIN);

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

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
#ifdef _debug
    Serial.println("[NTP] Failed to get time using NTP!");
#endif
    wpgfx.showFatalError("Error syncing NTP!", true); // without NTP, TLS won't work, we can as well just GOTO FAIL :)
  }
#ifdef _debug
  Serial.print("[NTP] Synced: ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
#endif

  wpgfx.setBootStatus("Connecting MQTT...");
  wpmqtt.begin();

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

void loop()
{
  // read light level
  photoresistorVal = analogRead(PHOTORESISTOR_PIN);

  // handle mqtt messages
  wpmqtt.loop();

  // send mqtt telemetry
  if (millis() - lastTelemetryReport > MQTT_TELEMETRY_INTERVAL_MS)
  {
    publishTelemetry();
  }
}
