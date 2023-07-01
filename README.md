# Farmer Pickles - Irrigation System

Farmer Pickles is an Arduino sketch for an irrigation system that can be run on either an ESP32 or an ESP8266 microcontroller. This sketch allows you to configure the irrigation settings and control the irrigation pump through a graphical user interface (GUI).

## Features

- Set the irrigation hour, minute, and duration.
- Enable or disable irrigation.
- Manually trigger a test irrigation.
- Reset the system state.

## Hardware Requirements

- ESP32 or ESP8266 microcontroller.
- Wi-Fi module compatible with the selected microcontroller.
- Irrigation pump connected to a GPIO pin (D5 in the provided code).
- Optional LED connected to a GPIO pin (D4 in the provided code) for indicating the system state.

## Libraries Used

- [ESPmDNS](https://github.com/espressif/arduino-esp32/tree/master/libraries/ESPmDNS) (ESP32 only)
- [WiFi](https://www.arduino.cc/en/Reference/WiFi) (ESP32) / [ESP8266WiFi](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html) (ESP8266)
- [ESPUI](https://github.com/s00500/ESPUI) (GUI library)
- [ESP_EEPROM](https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM)
- [NTPClient](https://github.com/arduino-libraries/NTPClient)
- [TimeLib](https://github.com/PaulStoffregen/Time)
- [WiFiUdp](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/src)

## Installation

1. Install the necessary libraries listed above.
2. Download the `FarmerPickles.ino` file.
3. Open the `FarmerPickles.ino` file in the Arduino IDE.
4. Connect your ESP32 or ESP8266 board to your computer.
5. Select the appropriate board and port in the Arduino IDE.
6. Upload the sketch to your microcontroller.

## Configuration

1. Open the `FarmerPickles.ino` file in the Arduino IDE.
2. Modify the `ssid` and `password` variables in the `setup()` function to match your Wi-Fi network credentials.
3. Adjust any other configuration parameters in the `Settings` struct according to your requirements.

## Usage

1. Power on your ESP32 or ESP8266 board.
2. The board will connect to the specified Wi-Fi network and attempt to synchronize the time with an NTP server.
3. Once the board is connected and the time is synchronized, you can access the GUI by entering the IP address of the board in a web browser.
4. In the GUI, you can adjust the irrigation settings, enable/disable irrigation, perform a test irrigation, and reset the system state.
5. The board will continuously check the current time and perform irrigation if the conditions are met based on the settings.
6. 1 = 30 Minutes in settings. 

## Notes

- Make sure you have the necessary hardware connected and configured correctly before running the sketch.
- The sketch uses EEPROM memory to store the configuration settings, so the settings will persist even after power cycling the board.
- The GUI provided by the ESPUI library allows for easy configuration and control of the irrigation system.
- The code includes conditional compilation based on the selected microcontroller (ESP32 or ESP8266), so make sure to select the appropriate board in the Arduino IDE before uploading the sketch.

## License

This code is released under the [MIT License](LICENSE).
---

Please note that this generated README provides a basic overview of the code and its usage. You may need to provide additional information or customize it further based on your specific requirements and preferences.
