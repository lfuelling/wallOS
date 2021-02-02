#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <debug.h>
#include "../config.h"

#ifndef WPBME280_H
#define WPBME280_H

class WPBME280
{
    Adafruit_BME280 *bme;
    TwoWire *I2Cbme;

    // bme280 metrics
    float temperature, humidity, pressure, altitude;

    bool started = false;

public:
    WPBME280();
    void begin();
    void debugLoop();
    bool isStarted();
    float getTemperature();
    float getHumidity();
    float getPressure();
    float getAltitude();
};

#endif