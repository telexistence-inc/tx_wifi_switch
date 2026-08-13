@echo off
setlocal enabledelayedexpansion

echo ============================================
echo   ESP32 Flash + Configure Tool
echo ============================================

if "%~5"=="" (
    echo.
    echo Usage: flash.bat PORT SSID PASSWORD LOCAL_IP GATEWAY [SUBNET]
    echo Example: flash.bat COM5 MyWiFi MyPassword123 192.168.1.50 192.168.1.1 255.255.255.0
    echo ^(subnet is optional, defaults to 255.255.255.0^)
    echo.
    pause
    exit /b 1
)

set PORT=%~1
set SSID=%~2
set PASS=%~3
set IP=%~4
set GW=%~5
set SUBNET=%~6
if "%SUBNET%"=="" set SUBNET=255.255.255.0

REM ===== Step 1: Check Python =====
where python >nul 2>&1
if errorlevel 1 (
    echo.
    echo Python was not found on this computer.
    where winget >nul 2>&1
    if errorlevel 1 (
        echo Please install Python manually from https://www.python.org/downloads/
        echo IMPORTANT: check "Add python.exe to PATH" during setup.
        pause
        exit /b 1
    ) else (
        echo Installing Python via winget, this may take a minute...
        winget install -e --id Python.Python.3.12
        echo.
        echo Python was just installed. Please close this window, open a
        echo NEW Command Prompt, and run this script again so the updated
        echo PATH takes effect.
        pause
        exit /b 1
    )
)

REM ===== Step 2: Check / bootstrap pip =====
python -m pip --version >nul 2>&1
if errorlevel 1 (
    echo pip not found, bootstrapping it...
    python -m ensurepip --upgrade
)

REM ===== Step 3: Check / install esptool =====
python -m esptool version >nul 2>&1
if errorlevel 1 (
    echo esptool not found, installing...
    python -m pip install --upgrade esptool
)

REM ===== Step 4: Build the config block from the given parameters =====
echo.
echo Building configuration...
python make_config.py "%SSID%" "%PASS%" "%IP%" "%GW%" "%SUBNET%" config.bin
if errorlevel 1 (
    echo Failed to build configuration file. Check the values you entered.
    pause
    exit /b 1
)

REM ===== Step 5: Flash firmware + configuration in ONE pass =====
REM NOTE: we intentionally do NOT use the merged.bin here. merge-bin pads
REM the image out to the full 4MB flash size, which collides with the
REM usrcfg offset (0x3E0000) and would force a second bootloader entry.
REM Flashing the individual pieces at their real offsets instead means
REM everything fits in a single esptool call -> board only needs to be
REM put into bootloader mode once.
echo.
echo Flashing firmware and configuration to %PORT% ...
echo If nothing happens for a few seconds, hold BOOT, tap RESET, then
echo release BOOT on the board now.
echo.
python -m esptool --chip esp32 --port %PORT% --baud 921600 write_flash ^
    0x1000   WifiConnect_configurable.ino.bootloader.bin ^
    0x8000   WifiConnect_configurable.ino.partitions.bin ^
    0xe000   boot_app0.bin ^
    0x10000  WifiConnect_configurable.ino.bin ^
    0x3E0000 config.bin

if errorlevel 1 (
    echo.
    echo Flash failed. Check the USB connection, the COM port number,
    echo and that the board is in bootloader mode.
    pause
    exit /b 1
)

echo.
echo Done. Device flashed and configured:
echo   SSID:    %SSID%
echo   IP:      %IP%
echo   Gateway: %GW%
echo   Subnet:  %SUBNET%
pause