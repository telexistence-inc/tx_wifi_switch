# ESP32 Flash + Configure Package

This lets an end user flash the switch firmware AND set their own
SSID, password, and static IP — using only `flash.bat` and a COM port,
no Arduino IDE or compiler required on their machine.

## How it works

The firmware no longer has SSID/password/IP hardcoded. Instead, at
boot it reads them from a small dedicated area of flash (the `usrcfg`
partition). `flash.bat` writes that area separately from the main
firmware image, using values you pass on the command line. If that
area has never been written, the firmware falls back to the defaults
built into the .ino.

## When Firmware needs a change
Arduino IDE (or arduino-cli) with the ESP32 core installed 
is require, to re-generate the base firmware binaries:

1. Open `WifiConnect_configurable.ino` in Arduino IDE.
2. Tools -> Partition Scheme -> **Custom** (this makes it use the
   `partitions.csv` in this folder, which reserves space for the
   config block at offset 0x3E0000).
3. Sketch -> Export Compiled Binary.
4. From the sketch's build output folder, copy these FOUR files into
   this folder, next to `flash.bat` (do NOT use the `.merged.bin):
   - `WifiConnect_configurable.ino.bootloader.bin`
   - `WifiConnect_configurable.ino.partitions.bin`
   - `boot_app0.bin`
   - `WifiConnect_configurable.ino.bin`

Zip up this whole folder. It should contain:
- `flash.bat`
- `make_config.py`
- `WifiConnect_configurable.ino.bootloader.bin`
- `WifiConnect_configurable.ino.partitions.bin`
- `boot_app0.bin`
- `WifiConnect_configurable.ino.bin`


## Flashing Process

1. Unzip the "FW_Flash" folder anywhere.
2. Plug the ESP32 in via USB.
3. Open Command Prompt, `cd` into the unzipped folder.
4. Find the COM port: Device Manager -> Ports (COM & LPT).
5. Run:
   ```
   flash.bat COM5 MyWiFiName MyWiFiPassword 192.168.1.50 192.168.1.1
   ```
   (subnet is optional, defaults to 255.255.255.0 — add it as a 6th
   argument if needed)
6. Press and hold the Black Button on-board while plugging in the Board.
7. Please hold the button until "Connecting....." text disappears.
8. After successful flashing of Firmware, the SSID,IP of is displayed.

The script will install Python/pip/esptool automatically if they
aren't already present, then flash the firmware and configuration
together in single step.
