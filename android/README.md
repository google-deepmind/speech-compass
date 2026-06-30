# Android Application

The app runs on the phone. It uses the phone's built-in microphone for speech recognition
(ASR) and receives azimuth angle data from the SpeechCompass phone case over USB.
Visualizations are built with the [Processing for Android](https://android.processing.org/)
framework.

![App screenshot](https://github.com/google-deepmind/speech_compass/blob/main/docs/images/app.jpg)

## Demo

Chapters from the [project demo video](../README.md#demo-video):

**Transcripts diarized by direction**

https://github.com/user-attachments/assets/6112df95-f870-410e-9ffa-ed4d674b14fa

**Visualization customization**

https://github.com/user-attachments/assets/a756192b-5d0b-4ae6-b0c1-60f5a0f14a97

**Live demo and group conversation**

https://github.com/user-attachments/assets/6d96438e-9292-4822-893e-d382fb6d08f2

## Building from source

### Android Studio

1. Install the latest [Android Studio](https://developer.android.com/studio).

2. Clone this repository and open the `android/` folder in Android Studio (**File → Open**).

3. Build the project (**Build → Make Project**).

4. Connect the phone over USB and click **Run** to install and launch.

### Windows command line

The `scripts/windows/` folder provides batch scripts for building and installing
without opening Android Studio. Requires Android Studio (for the bundled JDK)
and `adb` on the PATH.

**Build:**
```
scripts\windows\compile_debug.bat
scripts\windows\compile_release.bat
```

**Install on device:**
```
scripts\windows\run_debug.bat
scripts\windows\run_release.bat
```

If multiple devices are connected, you will be prompted to select one by name.

**Wireless debugging** — run once with the device connected via USB:
```
scripts\windows\connect_wifi.bat
```

If the device was already paired via Android Studio wireless debugging,
the USB cable is not needed.

## Installing a pre-built APK

Pre-built APKs are attached to [GitHub Releases](https://github.com/google-deepmind/speech-compass/releases).
Download the latest `speechcompass.apk`, connect the phone to your PC, then sideload via ADB:

```
adb install speechcompass.apk
```

> The app targets Android 14 (API 34). It may stop working on newer Android versions
> due to API changes.
