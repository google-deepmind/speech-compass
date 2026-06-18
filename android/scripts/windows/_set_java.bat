@echo off
if "%JAVA_HOME%"=="" (
    if exist "C:\Program Files\Android\Android Studio\jbr" (
        set "JAVA_HOME=C:\Program Files\Android\Android Studio\jbr"
    ) else if exist "%LOCALAPPDATA%\Programs\Android\Android Studio\jbr" (
        set "JAVA_HOME=%LOCALAPPDATA%\Programs\Android\Android Studio\jbr"
    ) else (
        echo ERROR: JAVA_HOME is not set and Android Studio was not found.
        echo Please set JAVA_HOME to a JDK 17+ installation, or install Android Studio.
        exit /b 1
    )
    echo Using JDK: %JAVA_HOME%
)
