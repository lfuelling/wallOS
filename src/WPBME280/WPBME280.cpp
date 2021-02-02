#include "WPBME280.h"

WPBME280::WPBME280()
{
    bme = new Adafruit_BME280();
    I2Cbme = new TwoWire(1);
}

void WPBME280::begin()
{
    // Initialize BME
    I2Cbme->begin(BME280_SDA, BME280_SCL, 100000);
    if (!bme->begin(0x76, I2Cbme))
    {
#ifdef _debug
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (true)
            ; // block further execution
#endif
    }
}

float WPBME280::getTemperature()
{
    return bme->readTemperature();
}

float WPBME280::getHumidity()
{
    return bme->readHumidity();
}

float WPBME280::getPressure()
{
    return bme->readPressure();
}

float WPBME280::getAltitude()
{
    return bme->readAltitude(SEALEVELPRESSURE_HPA);
}

void WPBME280::debugLoop()
{
    temperature = bme->readTemperature();
    humidity = bme->readHumidity();
    pressure = bme->readPressure() / 100.0F;
    altitude = bme->readAltitude(SEALEVELPRESSURE_HPA);
    Serial.print("BME: temp(");
    Serial.print(temperature);
    Serial.print("), hum(");
    Serial.print(humidity);
    Serial.print("), pres(");
    Serial.print(pressure);
    Serial.print("), alt(");
    Serial.print(altitude);
    Serial.println(");");
    delay(800);
}