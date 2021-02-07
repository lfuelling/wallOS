#include "Helpers/Storage.h"

void Storage::initializeStorage()
{
#ifdef _debug
    Serial.println("[NVS] Initializing storage...");
#endif
    bool r1 = NVS.setString("lastCommand", "none");
#ifdef _debug
    if (!r1)
    {
        Serial.println("[NVS] write failed! k=lastCommand, v=none");
    }
#endif

    for (int i = 0; i < (sizeof(switches) / sizeof(switches[0])); i++)
    {
        MqttSwitch s = switches[i];
        bool rs1 = NVS.setString(String("s/" + s.switchName), "unknown");
        bool rs2 = NVS.setString(s.switchName, "unknown");
#ifdef _debug
        if (!rs1)
        {
            Serial.print("[NVS] write failed! k=");
            Serial.print("s/" + s.switchName);
            Serial.println(", v=unknown");
        }
        if (!rs2)
        {
            Serial.print("[NVS] write failed! k=");
            Serial.print(s.switchName);
            Serial.println(", v=unknown");
        }
#endif
    }

    bool r4 = NVS.setString("c/delock/POWER", "none");
#ifdef _debug
    if (!r4)
    {
        Serial.println("[NVS] write failed! k=c/delock/POWER, v=none");
    }
#endif

    bool r5 = NVS.setString("c/delock2/POWER", "none");
#ifdef _debug
    if (!r5)
    {
        Serial.println("[NVS] write failed! k=c/delock2/POWER, v=none");
    }
#endif

    bool r6 = NVS.setInt("shouldRedraw", 0);
#ifdef _debug
    if (!r6)
    {
        Serial.println("[NVS] write failed! k=shouldRedraw, v=0");
    }
#endif
}
