#include "WPGFX.h"

WPGFX::WPGFX()
{
    touch = new XPT2046_Touchscreen(TOUCH_CS, TOUCH_IRQ);
    tft = new Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
}

void WPGFX::begin()
{
    pinMode(TFT_LED, OUTPUT);
    digitalWrite(TFT_LED, LOW);
    // turn on display lighting
    // Start driver
    tft->begin();
    touch->begin();
#ifdef _debug
    // Resolution of the display
    Serial.print("tftx =");
    Serial.print(tft->width());
    Serial.print("tfty =");
    Serial.println(tft->height());
#endif
    tsx = 0;
    tsy = 0;
    tsxraw = 0;
    tsyraw = 0;
    tsdown = false;
    rotation = 0;
}

void WPGFX::printError(String error)
{
    //TODO: implement
}

// current position and contact state
// from the touchscreen
void WPGFX::handleGraphics()
{
    TS_Point p;
    p = touch->getPoint();
    // read current data
    tsxraw = p.x;

    //x and y as raw values
    tsyraw = p.y;
    delay(1);

    // determine the orientation screen
    uint8_t red = tft->getRotation();
    // relative screen positions depending on the orientation
    // determine
    switch (red)
    {
    case 0:
        tsx = map(tsyraw, TS_MINY, TS_MAXY, 240, 0);
        tsy = map(tsxraw, TS_MINX, TS_MAXX, 0, 320);
        break;
    case 1:
        tsx = map(tsxraw, TS_MINX, TS_MAXX, 0, 320);
        tsy = map(tsyraw, TS_MINY, TS_MAXX, 0, 240);
        break;
    case 2:
        tsx = map(tsyraw, TS_MINY, TS_MAXY, 0, 240);
        tsy = map(tsxraw, TS_MINX, TS_MAXX, 320, 0);
        break;
    case 3:
        tsx = map(tsxraw, TS_MINX, TS_MAXX, 320, 0);
        tsy = map(tsyraw, TS_MINY, TS_MAXY, 240, 0);
        break;
    }

    // determine the contact state
    if (touch->touched() != tsdown)
    {
        tsdown = touch->touched();
        // Check if the green rectangle has been touched in the middle
        if (tsdown && (tsx > (tft->width() / 2 - 20)) && (tsx < (tft->width() / 2 + 20)) && (tsy > (tft->height() / 2 - 20)) && (tsy < (tft->height() / 2 + 20)))
        {
            // if yes, change screen orientation
            rotation++;
            if (rotation > 3)
                rotation = 0;
        }
        // redraw the screen
        draw_screen(rotation);
    }
}

// Display the main screen
void WPGFX::draw_screen(uint8_t red)
{
    uint16_t W, h;
    // Select the colors and font orientation
    tft->setRotation(red);
    tft->fillScreen(ILI9341_BLACK);
    tft->setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft->setFont(&FreeSans9pt7b);
    W = tft->width();
    h = tft->height();
    // depending on the orientation measured values and
    // draw the green rectangle in the middle
    if ((red == 1) || (red == 3))
    {
        drawposition1(tsxraw, tsyraw, tsx, tsy, tsdown);
        tft->fillRect(W / 2 - 10, h / 2 - 20, 20, 40, ILI9341_GREEN);
    }
    else
    {
        drawposition2(tsxraw, tsyraw, tsx, tsy, tsdown);
        tft->fillRect(W / 2 - 20, h / 2 - 10, 40, 20, ILI9341_GREEN);
    }
    // Display the rotation index in the green rectangle
    tft->setCursor(W / 2 - 5, h / 2 + 6);
    tft->setTextColor(ILI9341_BLACK, ILI9341_GREEN);
    tft->print(red);
    // Show Arrows to the Corner Points
    tft->drawLine(0, 0, 20, 0, ILI9341_WHITE);
    tft->drawLine(0, 0, 0, 20, ILI9341_WHITE);
    tft->drawLine(0, 0, 40, 40, ILI9341_WHITE);
    tft->drawLine(W - 1, 0, W - 20, 0, ILI9341_WHITE);
    tft->drawLine(W - 1, 0, W - 1, 20, ILI9341_WHITE);
    tft->drawLine(W - 1, 0, W - 40, 40, ILI9341_WHITE);
    tft->drawLine(W - 1, h - 1, W - 40, h, ILI9341_WHITE);
    tft->drawLine(W - 1, h - 1, W, h - 40, ILI9341_WHITE);
    tft->drawLine(W - 1, h - 1, W - 40, h - 40, ILI9341_WHITE);
    tft->drawLine(0, h - 1, 20, h - 1, ILI9341_WHITE);
    tft->drawLine(0, h - 1, 0, h - 20, ILI9341_WHITE);
    tft->drawLine(0, h - 1, 40, h - 40, ILI9341_WHITE);
}

// Display landscape metrics
void WPGFX::drawposition1(uint16_t xraw, uint16_t yraw, uint16_t X, uint16_t Y, bool Down)
{
    tft->setCursor(20, 60);
    tft->print("X =");
    display_right(110, 60, String(X));
    tft->setCursor(180, 60);
    tft->print("Y =");
    display_right(270, 60, String(Y));
    tft->setCursor(20, 180);
    tft->print("Xraw =");
    display_right(120, 180, String(xraw));
    tft->setCursor(180, 180);
    tft->print("Yraw =");
    display_right(280, 180, String(yraw));
    if (Down)
        tft->fillCircle(160, 160, 10, ILI9341_RED);
    else
        tft->fillCircle(160, 160, 10, ILI9341_YELLOW);
}

// Display metrics for high format
void WPGFX::drawposition2(uint16_t xraw, uint16_t yraw, uint16_t X, uint16_t Y, bool Down)
{
    tft->setCursor(20, 60);
    tft->print("X =");
    display_right(110, 60, String(X));
    tft->setCursor(20, 100);
    tft->print("Y =");
    display_right(110, 100, String(Y));
    tft->setCursor(20, 240);
    tft->print("Xraw =");
    display_right(120, 240, String(xraw));
    tft->setCursor(20, 280);
    tft->print("Yraw =");
    display_right(120, 280, String(yraw));
    if (Down)
        tft->fillCircle(120, 200, 10, ILI9341_RED);
    else
        tft->fillCircle(120, 200, 10, ILI9341_YELLOW);
}

// Issue a number right
void WPGFX::display_right(int X, int Y, String Val)
{
    int16_t x1, y1;
    uint16_t W, h;
    int str_len = Val.length() + 1;
    char char_array[str_len];
    Val.toCharArray(char_array, str_len);
    tft->getTextBounds(char_array, X, Y, &x1, &y1, &W, &h);
    tft->setCursor(X - W, Y);
    tft->print(char_array);
}

// Display the boot screen
void WPGFX::drawBootScreen()
{
    uint16_t W, h;
    // Select the colors and font orientation
    tft->setRotation(0);
    tft->fillScreen(ILI9341_BLACK);
    tft->setTextColor(ILI9341_LIGHTGREY, ILI9341_BLACK);
    tft->setFont(&FreeMono9pt7b);
    W = tft->width();
    h = tft->height();

    tft->setCursor(W / 2 - 64, h / 2 + 9);
    tft->println("-         -");
    tft->setCursor(W / 2 - 64, h / 2 + 18);
    tft->println(" \\_(*_*)_/ ");

    tft->setCursor(W / 2 - 56, h / 2 + 36);
    tft->println(bootStatus);
}

void WPGFX::setBootStatus(String status)
{
    bootStatus = status;
    drawBootScreen();
}