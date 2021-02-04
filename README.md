# ESP32 Wall Panel Operating System (wallOS)

This is a firmware for the [ESP32 wall panel by AZ-Delivery](https://az-delivery.de/products/az-touch-wandgehauseset-mit-2-8-zoll-touchscreen-fur-esp8266-und-esp32) (ESP32 only, although the board can also use an 8266).

## Features

- Boots
- Connects to WiFi
- Can (must have) an BME280 on the board (not included by default)
- Can connect to MQTT
- Pushes the BME280 telemetry to MQTT with a configurable interval
- Has the ability to trigger and show the state of MQTT switches (these are currently hardcoded 🙈)
- ... as of now all the rest is TODO

## Usage

1. Install PlatformIO
2. Clone this repo
3. Rename `src/config.h.example` to `src/config.h`
4. Edit `src/config.h` to fit your needs
5. Compile and flash the code
