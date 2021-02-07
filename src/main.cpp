#include "debug.h"
#include <config.h>
#include <Esp.h>
#include <SPI.h>
#include <time.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <WPGFX/WPGFX.h>
#include <WPMQTT/WPMQTT.h>
#include <WPBME280/WPBME280.h>
#include <ArduinoNvs.h>
#include <WebServer.h>
#include "Helpers/InitialSetup.h"
#include "Helpers/Telemetry.h"
#include "Helpers/Storage.h"
#include "Helpers/Boot.h"

// helper objects
Storage storage;
WPBME280 wpbme = WPBME280();
WPMQTT wpmqtt = WPMQTT(&wpbme);
WPGFX wpgfx = WPGFX(&wpbme);
Telemetry telemetry = Telemetry(&wpmqtt, &wpbme);
InitialSetup initialSetup = InitialSetup(&wpgfx);
Boot boot = Boot(&wpgfx, &storage, &wpbme, &wpmqtt);

// values
int photoresistorVal;
unsigned long lastTelemetryReport = 0;
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

// method that is run at program start
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

  if (NVS.getInt("setupDone") != 100)
  {
    initialSetup.runInitialSetup();
  }
  else
  {
    boot.boot();
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

// main program loop
void loop()
{
  // read light level
  photoresistorVal = analogRead(PHOTORESISTOR_PIN);

  // handle mqtt messages
  wpmqtt.loop();

  // send mqtt telemetry
  if (millis() - lastTelemetryReport > MQTT_TELEMETRY_INTERVAL_MS)
  {
    telemetry.publishTelemetry(photoresistorVal);
    lastTelemetryReport = millis();
  }
}
