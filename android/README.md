# Android Application

The app runs on the phone. It uses the phone's built-in microphone for speech recognition
(ASR) and receives azimuth angle data from the SpeechCompass phone case over USB.
Visualizations are built with the [Processing for Android](https://android.processing.org/)
framework.

![App screenshot](https://github.com/google-deepmind/speech_compass/blob/main/docs/images/app.jpg)

## Building from source

1. Install the latest [Android Studio](https://developer.android.com/studio).

2. Clone this repository and open the `android/` folder in Android Studio (**File → Open**).

3. Build the project (**Build → Make Project**).

4. Connect the phone over USB and click **Run** to install and launch.

## Installing a pre-built APK

Pre-built APKs are attached to [GitHub Releases](../../releases). Download the latest
`speechcompass.apk`, connect the phone to your PC, then sideload via ADB:

```
adb install speechcompass.apk
```

> The app targets Android 14 (API 34). It may stop working on newer Android versions
> due to API changes.
