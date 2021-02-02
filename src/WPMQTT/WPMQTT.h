#include "../config.h"
#include <WiFi.h>
#include "../WPBME280/WPBME280.h"
#include <PubSubClient.h>
#include <debug.h>

#ifndef WPMQTT_H
#define WPMQTT_H

class WPMQTT
{
    PubSubClient *client;
    WPBME280 *bme280;

    void reconnect();
    void callback(char *topic, byte *message, unsigned int length);

public:
    WPMQTT(WiFiClient wifi, WPBME280 *bme280);
    void begin();
    void publishMessage(String topic, String message);
};

#endif