@echo off
echo Flashing ESP32...
python -m esptool --chip esp32 --port %1 --baud 921600 write_flash 0x0 Wificonnect.ino.merged.bin
pause