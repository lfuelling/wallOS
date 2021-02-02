#include "WPMQTT.h"

WPMQTT::WPMQTT(WiFiClient wifi, WPBME280 *bme280)
{
    client = new PubSubClient(wifi);
}

void WPMQTT::begin()
{
    client->setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
    client->setCallback([&](char *topic, byte *message, unsigned int length) {
#ifdef _debug
        Serial.print("MQTT message arrived on topic: ");
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
            Serial.print("MQTT command received: ");
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
            Serial.println("MQTT (re-)connected!");
#endif
            client->subscribe(MQTT_COMMAND_TOPIC);
        }
        else
        {
#ifdef _debug
            Serial.print("MQTT failed, rc=");
            Serial.print(client->state());
            Serial.println(" try again in 5 seconds");
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