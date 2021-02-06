#include "WPGFX/WPGFX.h"
#include "WPGFX/ScreenUtils.h"
#include "ArduinoNvs.h"

WPGFX::WPGFX(WPBME280 *bme)
{
    bme280 = bme;
    touch = new XPT2046_Touchscreen(TOUCH_CS, TOUCH_IRQ);
    tft = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
    mainPage = new MainPage(tft, touch);
}

void WPGFX::begin()
{
    pinMode(TFT_LED, OUTPUT);
    // turn on display lighting
    digitalWrite(TFT_LED, LOW);
    // Start driver
    tft->begin();
    touch->begin();
#ifdef _debug
    // Resolution of the display
    Serial.print("[TFT] X=");
    Serial.print(tft->width());
    Serial.print(" Y=");
    Serial.println(tft->height());
#endif
    tsx = 0;
    tsy = 0;
    tsxraw = 0;
    tsyraw = 0;
    tsdown = false;
    rotation = 1;
}

void WPGFX::showFatalError(String error, bool reboot)
{
    uint16_t W, h;
    // Select the colors and font orientation
    tft->setRotation(1);
    tft->fillScreen(ILI9341_BLACK);
    tft->setTextColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
    tft->setFont(&FreeMono9pt7b);
    W = tft->width();
    h = tft->height();

    tft->setCursor(W / 2 - 64, h / 2 + 9);
    tft->println("-         -");
    tft->setCursor(W / 2 - 64, h / 2 + 18);
    tft->println(" \\_(x.x)_/ ");

    tft->setFont(&gidugu10pt7b);
    tft->setCursor(W / 2 - 56, h / 2 + 40);
    tft->println(error);
    tft->setFont(&FreeMono9pt7b);

    if (reboot)
    {
        delay(3000);
        ESP.restart();
    }
}

// current position and contact state
// from the touchscreen
void WPGFX::handleGraphics()
{
#ifdef _debug
    if (touch->touched())
    {
        TS_Point p = ScreenUtils().getTouchPosition(tft, touch);
        Serial.print("[Touch] Drawing circle at ");
        Serial.print(p.x);
        Serial.print(",");
        Serial.println(p.y);
        tft->drawCircle(p.x, p.y, 8, ILI9341_GREEN);
    }
#endif
    redrawIfNecessary();
}

void WPGFX::redrawIfNecessary()
{
    bool timeChanged = lastShownTime != getCurrentTime();
    bool signalChanged = lastSignalString != getSignalStrength();
    bool touchStateChanged = touch->touched() != tsdown;
    bool nvsShouldRedraw = NVS.getInt("shouldRedraw") == 1;

    if (touchStateChanged)
    {
        tsdown = touch->touched();
    }

    if (nvsShouldRedraw)
    {
        bool result = NVS.setInt("shouldRedraw", 0);
#ifdef _debug
        if (!result)
        {
            Serial.println("[NVS] write failed! k=shouldRedraw, v=0");
        }
#endif
    }

    if (timeChanged || signalChanged || touchStateChanged || nvsShouldRedraw)
    {
        // redraw the screen
        draw_screen(rotation);

#ifdef _debug
        Serial.print("[GFX] Redraw cause(s): ");
        if (timeChanged)
        {
            Serial.print("time ");
        }
        if (signalChanged)
        {
            Serial.print("signal ");
        }
        if (touchStateChanged)
        {
            Serial.print("touch ");
        }
        if (nvsShouldRedraw)
        {
            Serial.print("nvs ");
        }
        Serial.println();
#endif
    }
}

// Display the main screen
void WPGFX::draw_screen(uint8_t red)
{
    tft->setRotation(red);
    tft->fillScreen(ILI9341_BLACK);
    tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft->setFont(&FreeSans9pt7b);

    drawStatusBar();

    mainPage->drawMainPage();
}

// Display the boot screen
void WPGFX::drawBootScreen()
{
    uint16_t W, h;
    // Select the colors and font orientation
    tft->setRotation(1);
    tft->fillScreen(ILI9341_BLACK);
    tft->setTextColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
    tft->setFont(&FreeMono9pt7b);
    W = tft->width();
    h = tft->height();

    tft->setCursor(W / 2 - 64, h / 2 + 9);
    tft->println("-         -");
    tft->setCursor(W / 2 - 64, h / 2 + 18);
    tft->println(" \\_(*_*)_/ ");

    tft->setFont(&gidugu10pt7b);
    tft->setCursor(W / 2 - (bootStatus.length() * 3), h / 2 + 40);
    tft->println(bootStatus);
    tft->setFont(&FreeMono9pt7b);
}

void WPGFX::setBootStatus(String status)
{
    bootStatus = status;
    drawBootScreen();
}

// Display the attention screen
void WPGFX::drawAttentionScreen()
{
    uint16_t W, h;
    // Select the colors and font orientation
    tft->setRotation(1);
    tft->fillScreen(ILI9341_BLACK);
    tft->setTextColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
    tft->setFont(&FreeMono9pt7b);
    W = tft->width();
    h = tft->height();

    tft->setCursor(W / 2 - 64, h / 2 + 9);
    tft->println("-         -");
    tft->setCursor(W / 2 - 64, h / 2 + 18);
    tft->println(" \\_(*.*)_/ ");

    tft->setCursor(W / 2 - (bootStatus.length() * 5), h / 2 + 40);
    tft->println(bootStatus);
    tft->setFont(&FreeMono9pt7b);
}

void WPGFX::setAttentionStatus(String status)
{
    bootStatus = status;
    drawAttentionScreen();
}