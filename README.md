# ESP32 Wall Panel Operating System (wallOS)

This is a firmware for the [ESP32 wall panel by AZ-Delivery](https://az-delivery.de/products/az-touch-wandgehauseset-mit-2-8-zoll-touchscreen-fur-esp8266-und-esp32) (ESP32 only, although the board can also use an 8266).

## Features

- Boots
- Connects to WiFi
- Can (must have) an BME280 on the board (not included by default)
- Can connect to MQTT
- Pushes the BME280 telemetry to MQTT with a configurable interval
- Has the ability to trigger and show the state of MQTT switches
- Can read a photoresistor on the board (not included by default)
- ... as of now all the rest is TODO

## Usage

What you will need:
- ESP32 DevKit C
- Wall panel (see link above)
- BMP280 breakout board (i2c version) 
- A photoresistor (I just had one lying around)
- A 10kΩ resistor (for the photoresistor)
- Wires (I used those generic breadboard wires with the terrible coating)

### Hardware

1. Assemble the board according to the [official documentation](https://az-delivery.de/en/products/az-touch-wandgehauseset-mit-2-8-zoll-touchscreen-fur-esp8266-und-esp32-1?_pos=3&_sid=ac340f8d4&_ss=r)
2. Remove the display by unscrewing from the bottom of the board (so the spacers are still fixed to the display)
3. Remove the ESP32 (if mounted)
4. Solder the BMP280 to one of the edges of the board (because it will have better airflow there)
    - Connect `VIN` to `3V3` 
    - Connect `GND` to `GND`
    - Connect `SCL` to `IO25`
    - Connect `SDA` to `IO26`
5. Solder the photoresistor to the opposing corner of the board (so light can come through the slits)
    - Connect one pin of the photoresistor to `3V3` 
    - Connect the other pin to `IO34`
6. Solder the 10kΩ resistor to the board (somewhere near the photoresistor)
    - Connect one pin to `GND`
    - Connect the other pin to the pin of the photoresistor that is connected to `IO34`
7. Put the display back in
8. Put the ESP32 back in

### Firmware

1. Install PlatformIO
2. Clone this repo
3. Rename `src/config.h.example` to `src/config.h`
4. Edit `src/config.h` to fit your needs
5. Compile and flash the code
