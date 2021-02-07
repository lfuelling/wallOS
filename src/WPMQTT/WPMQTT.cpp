#include "WPMQTT/WPMQTT.h"
#include "ArduinoNvs.h"
#include <Esp.h>

WPMQTT::WPMQTT(WPBME280 *bme)
{
    client = new MQTTClient();
    bme280 = bme;
}

void requestRedraw()
{
    bool result = NVS.setInt("shouldRedraw", 1);
#ifdef _debug
    if (!result)
    {
        Serial.println("[NVS] write failed! k=shouldRedraw, v=1");
    }
#endif
}

void onMessage(String &topic, String &payload)
{
#ifdef _debug
    Serial.print("[MQTT] Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");

    Serial.println(payload);
#endif

    if (topic == MQTT_COMMAND_TOPIC)
    {
#ifdef _debug
        Serial.print("[MQTT] Command received: ");
        Serial.println(payload);
#endif

        bool result = NVS.setString("lastCommand", payload);
#ifdef _debug
        if (!result)
        {
            Serial.print("[NVS] write failed! k=lastCommand, v=");
            Serial.println(payload);
        }
#endif
    }

    for (int i = 0; i < (sizeof(switches) / sizeof(switches[0])); i++)
    {
        MqttSwitch s = switches[i];
        if (topic == s.statusTopic)
        {
            NVS.setString(String("s/" + s.switchName), payload);
            requestRedraw();
        }
    }
}

bool WPMQTT::begin()
{
    String mqttServer = NVS.getString("conf/mqtt/srv");
    int mqttPort = NVS.getInt("conf/mqtt/prt");

    client->begin(mqttServer.c_str(), mqttPort, wifi);
    client->onMessage(onMessage);

    return reconnect();
}

void WPMQTT::publishMessage(String topic, String message)
{
    if (!client->connected())
    {
#ifdef _debug
        Serial.println("[MQTT] Pre-push: Not connected!");
#endif
        reconnect();
    }
    client->publish(topic, message);
}

void WPMQTT::loop()
{
    client->loop();

    if (!client->connected())
    {
        // TODO: maybe some error handling might be a good idea here
        bool res = reconnect();
    }

    // Handle MQTT command
    String command = NVS.getString("lastCommand");
    if (command != "none")
    {
#ifdef _debug
        Serial.print("[MQTT] Last command: ");
        Serial.println(command);
#endif
        handleCommand(command);
    }

    for (int i = 0; i < (sizeof(switches) / sizeof(switches[0])); i++)
    {
        MqttSwitch s = switches[i];
        handleSwitch(s.switchName, s.commandTopic);
    }
}

void WPMQTT::handleSwitch(String nvsKey, String topic)
{
    String delockCommand = NVS.getString(nvsKey);
    if (delockCommand != "none")
    {
#ifdef _debug
        Serial.print("[MQTT] switch command: k=");
        Serial.print(topic);
        Serial.print(", v=");
        Serial.println(delockCommand);
#endif
        publishMessage(topic, delockCommand);
        NVS.setString(nvsKey, "none");
    }
}

bool WPMQTT::reconnect()
{
    if (!client->connected())
    {
        int retries = 0;
        String mqttUser = NVS.getString("conf/mqtt/usr");
        String mqttPass = NVS.getString("conf/mqtt/pwd");
        #ifdef _debug
            Serial.print("[MQTT] Connecting as: ");
            Serial.print(mqttUser);
            Serial.println("...");
#endif
        while (!client->connect("Wandpanel", mqttUser.c_str(), mqttPass.c_str(), false))
        {
#ifdef _debug
            Serial.println("[MQTT] Connect failed! Retrying...");
#endif
            delay(800);
            retries++;
            if (retries > 100)
            {
                return false;
            }
        }

#ifdef _debug
        Serial.println("[MQTT] Connected!");
#endif

        subscribe(MQTT_COMMAND_TOPIC);

        for (int i = 0; i < (sizeof(switches) / sizeof(switches[0])); i++)
        {
            MqttSwitch s = switches[i];
            subscribe(s.statusTopic);
        }
    }
    return true;
}

void WPMQTT::subscribe(String topic)
{
    while (!client->subscribe(topic, MQTT_QOS_LEVEL))
    {
#ifdef _debug
        Serial.print("[MQTT] Subscription to '");
        Serial.print(topic);
        Serial.println("' failed! Retrying...");
#endif
        delay(400);
    }
#ifdef _debug
    Serial.print("[MQTT] Subscribed to ");
    Serial.print(topic);
    Serial.println("!");
#endif
}

void WPMQTT::handleCommand(String command)
{
    if (command == "version")
    {
        client->publish(MQTT_COMMAND_OUTPUT_TOPIC, WP_VERSION);
    }
    else if (command == "reboot")
    {
        ESP.restart();
    }
    bool result = NVS.setString("lastCommand", "none");
#ifdef _debug
    if (!result)
    {
        Serial.println("[NVS] write failed! k=lastCommand, v=none");
    }
#endif
}
