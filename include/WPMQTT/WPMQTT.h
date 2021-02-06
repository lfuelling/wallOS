#include "../config.h"
#include <WiFi.h>
#include "../WPBME280/WPBME280.h"
#include <MQTT.h>
#include <debug.h>
#include <WiFiClientSecure.h>

#ifndef WPMQTT_H
#define WPMQTT_H

class WPMQTT
{
    MQTTClient *client;
    WPBME280 *bme280;
    WiFiClientSecure wifi;

    void reconnect();
    void callback(char *topic, byte *message, unsigned int length);
    void handleCommand(String command);
    void subscribe(String topic);

    void handleSwitch(String nvsKey, String topic);

public:
    WPMQTT(WPBME280 *bme280);
    void begin();
    void publishMessage(String topic, String message);
    void loop();
};

#endif