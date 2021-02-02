#include "WPMQTT.h"

WPMQTT::WPMQTT(WiFiClient wifi, WPBME280 *bme)
{
    client = new PubSubClient(wifi);
    bme280 = bme;
}

void WPMQTT::begin()
{
    client->setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
    client->setCallback([&](char *topic, byte *message, unsigned int length) {
#ifdef _debug
        Serial.print("[MQTT] Message arrived on topic: ");
        Serial.print(topic);
        Serial.print(". Message: ");

        String messageTemp;

        for (int i = 0; i < length; i++)
        {
            Serial.print((char)message[i]);
            messageTemp += (char)message[i];
        }
        Serial.println();
#endif

        if (String(topic) == MQTT_COMMAND_TOPIC)
        {
#ifdef _debug
            Serial.print("[MQTT] Command received: ");
            Serial.println(messageTemp);
#endif

            if (messageTemp == "version")
            {
                client->publish(MQTT_COMMAND_OUTPUT_TOPIC, WP_VERSION);
            }
        }
    });
}

void WPMQTT::reconnect()
{
    while (!client->connected())
    {
        if (client->connect("Wandpanel"))
        {
#ifdef _debug
            Serial.println("[MQTT] (Re-)Connected!");
#endif
            client->subscribe(MQTT_COMMAND_TOPIC);
        }
        else
        {
#ifdef _debug
            Serial.print("[MQTT] Connect failed, rc=");
            Serial.print(client->state());
            Serial.println(" trying again in 5 seconds...");
#endif
            delay(MQTT_CONNECT_COOLDOWN);
        }
    }
}

void WPMQTT::publishMessage(String topic, String message)
{
    if (!client->connected())
    {
        reconnect();
    }
    client->publish(topic.c_str(), message.c_str());
}