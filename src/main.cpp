#include "debug.h"
#include <config.h>
#include <Esp.h>
#include <SPI.h>
#include <time.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WPGFX/WPGFX.h>
#include <WPMQTT/WPMQTT.h>
#include <WPBME280/WPBME280.h>
#include <ArduinoNvs.h>
#include <WebServer.h>

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

void publishTelemetry()
{
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

bool continueBoot()
{
  wpgfx.setBootStatus("Initializing storage...");
  initializeStorage();

  // init photoresistor
  wpgfx.setBootStatus("Photoresistor...");
  photoresistorVal = analogRead(PHOTORESISTOR_PIN);

  // init bme
  wpgfx.setBootStatus("Loading BME280...");
  wpbme.begin();

  wpgfx.setBootStatus("Connecting WiFi...");
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

  wpgfx.setBootStatus("Syncing NTP...");
  configTime(NTP_UTC_OFFSET_SECONDS, NTP_DAYLIGHT_OFFSET_SECONDS, NTP_SERVER_HOSTNAME);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
#ifdef _debug
    Serial.println("[NTP] Failed to get time using NTP!");
#endif
    wpgfx.showFatalError("Error syncing NTP!", true); // TLS won't work without NTP, we can as well just GOTO FAIL :)
  }
#ifdef _debug
  Serial.print("[NTP] Synced: ");
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
#endif

  wpgfx.setBootStatus("Connecting MQTT...");
  if (!wpmqtt.begin())
  {
    return false;
  }

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
  return true;
}

String getRandomString(int length)
{
  char randomString[length];
  // Generate random key
  memset(randomString, '\0', sizeof(randomString));
  uint8_t cnt = 0;
  while (cnt != sizeof(randomString) - 1)
  {
    randomString[cnt] = random(0, 0x7F);
    if (randomString[cnt] == 0)
    {
      break;
    }
    if (isAlphaNumeric(randomString[cnt]) == true)
    {
      cnt++;
    }
    else
    {
      randomString[cnt] = '\0';
    }
  }
  return String(randomString);
}

String getHtmlString(String content)
{
  String htmlHead = String("<!DOCTYPE html><html>\n<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n<style>html,body { font-family: sans-serif; color: #fff; background-color: #000;}body {display:flex; flex-direction:column; width: 100%; height: 100vh; justify-content: center; align-items: center;}h1, p, form, input, button {display:block;}form { width: 256px; }input, button { margin-top: 8px; margin-bottom: 8px; width: 100%; margin-left: 0px; margin-right: 0px; padding: 4px; background-color: transparent; border: 1px solid white; color: white; border-radius: 5px; }</style></head>");
  return String(htmlHead + String(content) + "</html>");
}

void runInitialSetup()
{
  wpgfx.setBootStatus("Erasing storage...");
  NVS.eraseAll();
  NVS.commit();
  wpgfx.setBootStatus("Starting AP...");
  String ssid = "WP-1234";
  String password = getRandomString(10);
  WiFi.softAP(ssid.c_str(), password.c_str());
  bool setupDone = false;
  WebServer server(80);
  String header;

  String pageHeader = "<body><h1>Initial Setup</h1>\n";
  String formIntro = "<p>\nPlease fill out the form below to finish the setup process.</p>";
  String formStart = "<form action=\"/save\" method=\"post\">\n";
  String formWifiInputs = "<input type=\"text\" name=\"ssid\" id=\"ssid\" placeholder=\"WiFi SSID\"/>\n<input type=\"password\" name=\"password\" id=\"password\" placeholder=\"WiFi Password\"/>\n";
  String formMqttInputs = "<input type=\"text\" name=\"mqttServer\" id=\"mqttServer\" placeholder=\"mqtt.example.com\"/>\n<input type=\"text\" name=\"mqttPort\" id=\"mqttPort\" placeholder=\"1883\"/>\n<input type=\"text\" name=\"mqttUser\" id=\"mqttUser\" placeholder=\"\"/>\n<input type=\"password\" name=\"mqttPasswort\" id=\"mqttPassword\" placeholder=\"MQTT Pass\"/>\n";
  String formInputs = String("<input type=\"text\" name=\"hostname\" id=\"hostname\" placeholder=\"Hostname\"/>\n" + formWifiInputs + formMqttInputs);
  String formEnd = "<button type=\"submit\">Finish Setup</button>\n</form>\n</body>";

  server.on("/", [&]() {
    String html = getHtmlString(String(pageHeader + formIntro + formStart + formInputs + formEnd));
    server.send(200, "text/html", html);
  });

  server.on("/save", HTTP_POST, [&]() {
    if (server.args() == 7)
    {
      String receivedHostname = server.arg("hostname");
      String receivedSsid = server.arg("ssid");
      String receivedPassword = server.arg("password");
      String receivedMqttServer = server.arg("mqttServer");
      String receivedMqttPort = server.arg("mqttPort");
      String receivedMqttUser = server.arg("mqttUser");
      String receivedMqttPassword = server.arg("mqttPassword");

      int mqttPortInt = receivedMqttPort.toInt();
      if (mqttPortInt <= 0 || mqttPortInt > 65535)
      {
        String html = getHtmlString(String(pageHeader + "<p>MQTT Port must be a valid port number!</p>\n</body>"));
        server.send(403, "text/html", html);
      }

#ifdef _debug
      Serial.print("[Setup] Received ssid=");
      Serial.print(receivedSsid);
      Serial.print(", pass=");
      Serial.println(receivedPassword);
#endif

      bool nvsResSsid = NVS.setString("conf/ssid", receivedSsid);
      bool nvsResPass = NVS.setString("conf/pass", receivedPassword);
      bool nvsResHost = NVS.setString("conf/host", receivedHostname);
      bool nvsResMqttServer = NVS.setString("conf/mqtt/srv", receivedMqttServer);
      bool nvsResMqttPort = NVS.setInt("conf/mqtt/prt", mqttPortInt);
      bool nvsResMqttUser = NVS.setString("conf/mqtt/usr", receivedMqttUser);
      bool nvsResMqttPass = NVS.setString("conf/mqtt/psw", receivedMqttPassword);
      bool nvsResSertupDone = NVS.setInt("setupDone", 100);

      if (nvsResSsid && nvsResPass && nvsResHost && nvsResMqttServer && nvsResMqttPort && nvsResMqttUser && nvsResMqttPass && nvsResSertupDone)
      {
        String html = getHtmlString(String(pageHeader + "<p>The system will reboot now!\n</body>"));
        server.send(200, "text/html", html);
      }
      else
      {
        String html = getHtmlString(String(pageHeader + "<p>The input was valid but there was an error saving!</p>\n<p>The System will reboot bow, please try again.</p>\n</body>"));
        server.send(500, "text/html", html);
        NVS.eraseAll();
      }
      NVS.commit();
      delay(10);
      NVS.close();
      delay(100);
      ESP.restart();
    }
    else
    {
#ifdef _debug
      Serial.print("[Setup] Received ");
      Serial.print(String(server.args()));
      Serial.println(" args, returning");
#endif
      String html = getHtmlString(String(pageHeader + "<p>All fields are required!</p>\n</body>"));
      server.send(403, "text/html", html);
    }
  });

  server.onNotFound([&]() {
    String html = getHtmlString(String(pageHeader + "<p>Nothing found for this URL!</p>\n</body>"));
    server.send(404, "text/html", html);
  });

  server.begin();

  String connectionString = String(ssid + ":" + password);
#ifdef _debug
  Serial.print("[Setup] WiFi data: ");
  Serial.println(connectionString);
#endif
  wpgfx.drawInitialSetupScreen(ssid, password, WiFi.softAPIP().toString());

  while (!setupDone)
  {
    server.handleClient();
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
    runInitialSetup();
  }
  else
  {
    bool bootSuccessFul = continueBoot();
    if (!bootSuccessFul)
    {
#ifdef _debug
      Serial.println("Boot failed!");
#endif
      wpgfx.showFatalError("Boot failed! Launching Setup...", false);
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
