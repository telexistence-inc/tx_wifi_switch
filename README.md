# Wifi_Switch

A simple ESP32-based Wi-Fi switch that exposes a small web server to turn a GPIO pin on or off over the network. This switch is placed in a way to control the power going to EOAT.

## What it does

This sketch runs on an ESP32 and:
- connects to "txLogiMesh"(SSID/PW is hard-coded) 
- assigns a fixed local IP address(192.168.50.139 is hard-coded)
- starts a simple web server on port 80
- controls GPIO 16 through the following endpoints:
  - `/on` → turns the output pin HIGH
  - `/off` → turns the output pin LOW
  - `/` → returns a simple status message

## Hardware

- ESP32 development board based on the board linked here: https://ja.aliexpress.com/i/1005005597999376.html?gatewayAdapt=glo2jpn
- This board does not include a USB port, so it is flashed using the onboard pin headers.
- The onboard relay is used to drive a 51V relay.
- One output load connected to GPIO 16 (defined as `IO_Pin`)
- 24V is supplied to the board, which is used to drive the coils.


## How to Control

After the Board is powered up with 24V, it automatically connects to Wi-Fi.

Then use a browser or any HTTP client to navigate to:
- `http://192.168.50.139/` for the status page
- `http://192.168.50.139/on` to turn the output ON -> **Which turns off the power to EOAT.**
- `http://192.168.50.139/off` to turn the output OFF -> **Which turns on the power to EOAT.**


## Flashing Process

1. Open the sketch in the Arduino IDE or PlatformIO.
2. Make sure the ESP32 board package is installed.
3. Update the Wi-Fi credentials in the sketch(If required):
   - `ssid`
   - `password`
4. Optionally change the static IP configuration (if needed) :
   - `local_IP`
   - `gateway`
   - `subnet`
5. Upload the sketch to the ESP32.
