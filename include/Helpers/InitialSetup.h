#include "config.h"
#include <WiFi.h>
#include <MQTT.h>
#include <debug.h>
#include <WPGFX/WPGFX.h>
#include <ArduinoNvs.h>
#include <WebServer.h>
#include <Esp.h>

#ifndef INTIAL_SETUP_H
#define INTIAL_SETUP_H

class InitialSetup
{
    WPGFX *wpgfx;
    String getHtmlString(String content); 
    String getRandomString(int length);

public:
    InitialSetup(WPGFX *gfx);
    void runInitialSetup();
};

#endif
