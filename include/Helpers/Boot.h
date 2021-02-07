#include "config.h"
#include <debug.h>
#include <ArduinoNvs.h>
#include <WiFi.h>
#include <Helpers/Storage.h>
#include <WPGFX/WPGFX.h>
#include <WPMQTT/WPMQTT.h>
#include <WPBME280/WPBME280.h>

#ifndef BOOT_H
#define BOOT_H

class Boot
{
    WPGFX *wpgfx;
    Storage *storage;
    WPBME280 *wpbme;
    WPMQTT *wpmqtt;

    bool bootSuccessful();

public:
    Boot(WPGFX *gfx, Storage *s, WPBME280 *bme, WPMQTT *mqtt);
    void boot();
};

#endif
