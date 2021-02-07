#include "Helpers/Telemetry.h"

Telemetry::Telemetry(WPMQTT *mqtt, WPBME280 *bme)
{
    wpmqtt = mqtt;
    wpbme = bme;
}

void Telemetry::publishTelemetry(int photoresistorVal)
{
    String hostname = NVS.getString("conf/host");

#ifdef _debug
    Serial.println(String("[MQTT] sending telemetry for" + hostname + "..."));
#endif

    String mqttTopic = String("tele/" + hostname + "/");

    // sensor values
    wpmqtt->publishMessage(String(mqttTopic + "temp"), String(wpbme->getTemperature()));
    wpmqtt->publishMessage(String(mqttTopic + "hum"), String(wpbme->getHumidity()));
    wpmqtt->publishMessage(String(mqttTopic + "press"), String(wpbme->getPressure()));
    wpmqtt->publishMessage(String(mqttTopic + "rssi"), String(WiFi.RSSI()));
    wpmqtt->publishMessage(String(mqttTopic + "photo"), String(photoresistorVal));

    // ESP internal values
    wpmqtt->publishMessage(String(mqttTopic + "heap/free"), String(ESP.getFreeHeap()));
    wpmqtt->publishMessage(String(mqttTopic + "heap/total"), String(ESP.getHeapSize()));
    wpmqtt->publishMessage(String(mqttTopic + "heap/min-free"), String(ESP.getMinFreeHeap()));
    wpmqtt->publishMessage(String(mqttTopic + "heap/max-alloc"), String(ESP.getMaxAllocHeap()));
    wpmqtt->publishMessage(String(mqttTopic + "psram/free"), String(ESP.getFreePsram()));
    wpmqtt->publishMessage(String(mqttTopic + "psram/total"), String(ESP.getPsramSize()));
    wpmqtt->publishMessage(String(mqttTopic + "psram/min-free"), String(ESP.getMinFreePsram()));
    wpmqtt->publishMessage(String(mqttTopic + "psram/max-alloc"), String(ESP.getMaxAllocPsram()));
    wpmqtt->publishMessage(String(mqttTopic + "chip-revision"), String(ESP.getChipRevision()));
    wpmqtt->publishMessage(String(mqttTopic + "cpu-freq"), String(ESP.getCpuFreqMHz()));
    wpmqtt->publishMessage(String(mqttTopic + "flash/size"), String(ESP.getFlashChipSize()));
    wpmqtt->publishMessage(String(mqttTopic + "flash/speed"), String(ESP.getFlashChipSpeed()));
    wpmqtt->publishMessage(String(mqttTopic + "sketch/size"), String(ESP.getSketchSize()));
    wpmqtt->publishMessage(String(mqttTopic + "sketch/free"), String(ESP.getFreeSketchSpace()));
    wpmqtt->publishMessage(String(mqttTopic + "sketch/md5"), String(ESP.getSketchMD5()));
}
