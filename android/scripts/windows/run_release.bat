@echo off
cd /d "%~dp0..\.."
call "%~dp0_select_device.bat"
if errorlevel 1 exit /b 1
if exist app\build\outputs\apk\release\app-release.apk (
    echo Installing release APK on %SELECTED_DEVICE%...
    adb -s %SELECTED_DEVICE% install -r app\build\outputs\apk\release\app-release.apk
) else if exist app\build\outputs\apk\release\app-release-unsigned.apk (
    echo NOTE: APK is unsigned. Install will fail unless a signing config is added to build.gradle.
    adb -s %SELECTED_DEVICE% install -r app\build\outputs\apk\release\app-release-unsigned.apk
) else (
    echo No release APK found. Run compile_release.bat first.
    exit /b 1
)
