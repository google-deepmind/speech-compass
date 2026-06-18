@echo off
echo [1] Script dir: %~dp0
echo [2] Working dir: %CD%
if "%JAVA_HOME%"=="" (
    if exist "C:\Program Files\Android\Android Studio\jbr" (
        set "JAVA_HOME=C:\Program Files\Android\Android Studio\jbr"
    ) else if exist "%LOCALAPPDATA%\Programs\Android\Android Studio\jbr" (
        set "JAVA_HOME=%LOCALAPPDATA%\Programs\Android\Android Studio\jbr"
    ) else (
        echo ERROR: JAVA_HOME not set and Android Studio JDK not found.
        exit /b 1
    )
)
echo [3] JAVA_HOME: %JAVA_HOME%
cd /d "%~dp0..\.."
echo [4] Changed to: %CD%
echo [5] Building debug APK...
call gradlew.bat assembleDebug --console=plain
