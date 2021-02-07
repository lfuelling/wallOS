#include "Helpers/InitialSetup.h"

InitialSetup::InitialSetup(WPGFX *gfx) {
    wpgfx = gfx;
}

String InitialSetup::getRandomString(int length)
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

String InitialSetup::getHtmlString(String content)
{
  String htmlHead = String("<!DOCTYPE html><html>\n<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n<style>html,body { font-family: sans-serif; color: #fff; background-color: #000;}body {display:flex; flex-direction:column; width: 100%; height: 100vh; justify-content: center; align-items: center;}h1, p, form, input, button {display:block;}form { width: 256px; }input, button { margin-top: 8px; margin-bottom: 8px; width: 100%; margin-left: 0px; margin-right: 0px; padding: 4px; background-color: transparent; border: 1px solid white; color: white; border-radius: 5px; }</style></head>");
  return String(htmlHead + String(content) + "</html>");
}

void InitialSetup::runInitialSetup()
{
  wpgfx->setBootStatus("Erasing storage...");
  NVS.eraseAll();
  NVS.commit();
  wpgfx->setBootStatus("Starting AP...");
  String macPart = WiFi.macAddress();
  macPart.remove(0, 12);
  macPart.remove(2, 1);
  String ssid = String("WP-" + macPart);
  String password = getRandomString(10);
  WiFi.softAP(ssid.c_str(), password.c_str());
  bool setupDone = false;
  WebServer server(80);
  String header;

  String pageHeader = "<body><h1>Initial Setup</h1>\n";
  String formIntro = "<p>\nPlease fill out the form below to finish the setup process.</p>";
  String formStart = "<form action=\"/save\" method=\"post\">\n";
  String formWifiInputs = "<input type=\"text\" name=\"ssid\" id=\"ssid\" placeholder=\"WiFi SSID\"/>\n<input type=\"password\" name=\"password\" id=\"password\" placeholder=\"WiFi Password\"/>\n";
  String formMqttInputs = "<input type=\"text\" name=\"mqttServer\" id=\"mqttServer\" placeholder=\"mqtt.example.com\"/>\n<input type=\"text\" name=\"mqttPort\" id=\"mqttPort\" placeholder=\"1883\"/>\n<input type=\"text\" name=\"mqttUser\" id=\"mqttUser\" placeholder=\"MQTT Username\"/>\n<input type=\"password\" name=\"mqttPassword\" id=\"mqttPassword\" placeholder=\"MQTT Pass\"/>\n";
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
      Serial.print(", password=");
      Serial.print(receivedPassword);
      Serial.print(", hostname=");
      Serial.print(receivedHostname);
      Serial.print(", mqttUser=");
      Serial.print(receivedMqttUser);
      Serial.print(", mqttPassword=");
      Serial.print(receivedMqttPassword);
      Serial.print(", mqttServer=");
      Serial.print(receivedMqttServer);
      Serial.print(", mqttPort=");
      Serial.println(receivedMqttPort);
#endif

      bool nvsResSsid = NVS.setString("conf/ssid", receivedSsid);
      bool nvsResPass = NVS.setString("conf/pass", receivedPassword);
      bool nvsResHost = NVS.setString("conf/host", receivedHostname);
      bool nvsResMqttServer = NVS.setString("conf/mqtt/srv", receivedMqttServer);
      bool nvsResMqttPort = NVS.setInt("conf/mqtt/prt", mqttPortInt);
      bool nvsResMqttUser = NVS.setString("conf/mqtt/usr", receivedMqttUser);
      bool nvsResMqttPass = NVS.setString("conf/mqtt/pwd", receivedMqttPassword);
      bool nvsResSertupDone = NVS.setInt("setupDone", 100);

      if (nvsResSsid && nvsResPass && nvsResHost && nvsResMqttServer && nvsResMqttPort && nvsResMqttUser && nvsResMqttPass && nvsResSertupDone)
      {
#ifdef _debug
        Serial.print("[Setup] Writes successful!");
#endif
        String html = getHtmlString(String(pageHeader + "<p>The system will reboot now!\n</body>"));
        server.send(200, "text/html", html);
        wpgfx->setBootStatus("Rebooting...");
      }
      else
      {
        String html = getHtmlString(String(pageHeader + "<p>The input was valid but there was an error saving!</p>\n<p>The System will reboot bow, please try again.</p>\n</body>"));
        server.send(500, "text/html", html);
        if(NVS.eraseAll()) {
#ifdef _debug
          Serial.print("[Setup] Erase successful!");
#endif
        } else {
#ifdef _debug
          Serial.print("[Setup] Erase failed!");
#endif
        }
        wpgfx->setBootStatus("Rebooting...");
      }
      NVS.commit();
      delay(20);
      NVS.close();
      delay(1000);
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
  wpgfx->drawInitialSetupScreen(ssid, password, WiFi.softAPIP().toString());

  while (!setupDone)
  {
    server.handleClient();
  }
}