#include "config.h"
#include <WiFi.h>
#include <MQTT.h>
#include <debug.h>
#include <WPGFX/WPGFX.h>
#include <WPBME280/WPBME280.h>
#include <ArduinoNvs.h>
#include <Esp.h>

#ifndef TELEMETRY_H
#define TELEMETRY_H

class Telemetry
{
    WPMQTT *wpmqtt;
    WPBME280 *wpbme;

public:
    Telemetry(WPMQTT *mqtt, WPBME280 *bme);
    void publishTelemetry(int photoresistorVal);
};

#endif
