#include "WPMQTT.h"

WPMQTT::WPMQTT(WPBME280 *bme)
{
    client = new MQTTClient();
    bme280 = bme;
}

void onMessage(String &topic, String &payload)
{
#ifdef _debug
    Serial.print("[MQTT] Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");

    Serial.println(payload);
#endif

    if (String(topic) == MQTT_COMMAND_TOPIC)
    {
#ifdef _debug
        Serial.print("[MQTT] Command received: ");
        Serial.println(payload);
#endif

        if (payload == "version")
        {
            // TODO: according to the new lib, publishing has to be done outside of this handler!
            // client->publish(MQTT_COMMAND_OUTPUT_TOPIC, WP_VERSION);
        }
    }
}

void WPMQTT::begin(WiFiClientSecure wifi)
{
    client->begin(MQTT_SERVER_IP, MQTT_SERVER_PORT, wifi);
    client->onMessage(onMessage);

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

    while (!client->subscribe(MQTT_COMMAND_TOPIC, 1))
    {
#ifdef _debug
        Serial.println("[MQTT] Subscription failed! Retrying...");
#endif
        delay(400);
    }

#ifdef _debug
    Serial.print("[MQTT] Subscribed to ");
    Serial.print(MQTT_COMMAND_TOPIC);
    Serial.println("!");
#endif
}

void WPMQTT::publishMessage(String topic, String message)
{
    if (!client->connected())
    {
        reconnect();
    }
    client->publish(topic.c_str(), message.c_str());
}
