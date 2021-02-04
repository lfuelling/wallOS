#include "WPMQTT.h"
#include "ArduinoNvs.h"

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

    if (topic == "stat/delock/POWER")
    {
        NVS.setString("delock/POWER", payload);
        requestRedraw();
    }
    else if (topic == "stat/delock2/POWER")
    {
        NVS.setString("delock2/POWER", payload);
        requestRedraw();
    }
}

void WPMQTT::begin()
{
    client->begin(MQTT_SERVER_IP, MQTT_SERVER_PORT, wifi);
    client->onMessage(onMessage);

    reconnect();
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
        reconnect();
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

    handleDelock("c/delock/POWER", "cmnd/delock/POWER");
    handleDelock("c/delock2/POWER", "cmnd/delock2/POWER");
}

void WPMQTT::handleDelock(String nvsKey, String topic)
{
    String delockCommand = NVS.getString(nvsKey);
    if (delockCommand != "none")
    {
#ifdef _debug
        Serial.print("[MQTT] delock command: k=");
        Serial.print(topic);
        Serial.print(", v=");
        Serial.println(delockCommand);
#endif
        publishMessage(topic, delockCommand);
        NVS.setString(nvsKey, "none");
    }
}

void WPMQTT::reconnect()
{
    if (!client->connected())
    {
        while (!client->connect("Wandpanel", MQTT_USERAME, MQTT_PASSWORD, false))
        {
#ifdef _debug
            Serial.println("[MQTT] Connect failed! Retrying...");
#endif
            delay(800);
        }

#ifdef _debug
        Serial.println("[MQTT] Connected!");
#endif

        subscribe(MQTT_COMMAND_TOPIC);
        subscribe("stat/delock/POWER");
        subscribe("stat/delock2/POWER");
    }
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
    bool result = NVS.setString("lastCommand", "none");
#ifdef _debug
    if (!result)
    {
        Serial.println("[NVS] write failed! k=lastCommand, v=none");
    }
#endif
}
