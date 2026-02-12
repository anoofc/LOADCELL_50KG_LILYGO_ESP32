
# LOADCELL_50KG_LILYGO_ESP32

A LilyGO ESP32 project for a 50kg load cell.

## main.cpp

This is the main application file for the load cell sensor project. It handles:

- **Initialization**: Sets up the ESP32 and load cell sensor interface
- **Sensor Reading**: Acquires weight measurements from the 50kg load cell
- **Data Processing**: Calibrates and filters sensor data
- **Output**: Displays or transmits readings

## Features

- ESP32 microcontroller
- 50kg load cell sensor
- Real-time weight measurement
- Data calibration support

## Requirements

- PlatformIO
- ESP32 board support
- Load cell library dependencies

## Build & Upload

```bash
platformio run --target upload
```
