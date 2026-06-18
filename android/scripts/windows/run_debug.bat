@echo off
cd /d "%~dp0..\.."
call "%~dp0_select_device.bat"
if errorlevel 1 exit /b 1
echo Installing debug APK on %SELECTED_DEVICE%...
adb -s %SELECTED_DEVICE% install -r app\build\outputs\apk\debug\app-debug.apk
