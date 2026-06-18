@echo off
setlocal enabledelayedexpansion
set _COUNT=0
for /f "skip=1 tokens=1,2" %%a in ('adb devices') do (
    if "%%b"=="device" (
        set /a _COUNT+=1
        set _SERIAL_!_COUNT!=%%a
        set _TMP_MODEL=
        for /f "delims=" %%m in ('adb -s %%a shell getprop ro.product.model 2^>nul') do set _TMP_MODEL=%%m
        echo   !_COUNT!. !_TMP_MODEL!	%%a
    )
)
if !_COUNT!==0 (
    echo No devices connected. Connect a device or run connect_wifi.bat first.
    endlocal
    exit /b 1
)
if !_COUNT!==1 (
    set _RESULT=!_SERIAL_1!
    goto :done
)
echo.
set /p _CHOICE="Select device [1-!_COUNT!]: "
if "!_CHOICE!"=="" ( endlocal & exit /b 1 )
for %%x in (!_CHOICE!) do set _RESULT=!_SERIAL_%%x!
if "!_RESULT!"=="" (
    echo Invalid selection.
    endlocal
    exit /b 1
)
:done
endlocal & set SELECTED_DEVICE=%_RESULT%
