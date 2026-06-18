@echo off
setlocal enabledelayedexpansion
echo Checking for paired wireless devices...

adb mdns check > nul 2>&1
if errorlevel 1 goto :usb_fallback

set COUNT=0
for /f "skip=1 tokens=1,3" %%a in ('adb mdns services 2^>nul') do (
    set /a COUNT+=1
    set ADDR_!COUNT!=%%b
    echo   !COUNT!. %%b  ^(%%a^)
)

if !COUNT!==0 (
    echo No paired devices found via mDNS.
    echo Make sure Wireless Debugging is toggled ON on the device:
    echo   Settings ^> Developer options ^> Wireless debugging
    echo.
    goto :usb_fallback
)

if !COUNT!==1 (
    set MDNS_ADDR=!ADDR_1!
    goto :connect
)

echo.
set /p CHOICE="Select device [1-!COUNT!]: "
if "!CHOICE!"=="" exit /b 1
for %%x in (!CHOICE!) do set MDNS_ADDR=!ADDR_%%x!
if "!MDNS_ADDR!"=="" (
    echo Invalid selection.
    exit /b 1
)
goto :connect

:usb_fallback
echo Falling back to USB method ^(device must be connected via USB^)...
adb tcpip 5555
timeout /t 2 /nobreak > nul
for /f %%i in ('powershell -Command "((adb shell ip route) -match \"wlan\")[0].Split()[-1]"') do set DEVICE_IP=%%i
if "!DEVICE_IP!"=="" (
    echo Could not detect device IP. Make sure the device is connected via USB and on Wi-Fi.
    exit /b 1
)
set MDNS_ADDR=!DEVICE_IP!:5555

:connect
echo Connecting to !MDNS_ADDR!...
adb connect !MDNS_ADDR!
echo.
echo Done. You can now disconnect USB if connected.
