# Android Application

The app runs on the phone. It uses the phone's built-in microphone for speech recognition
(ASR) and receives azimuth angle data from the SpeechCompass phone case over USB.
Visualizations are built with the [Processing for Android](https://android.processing.org/)
framework.

![App screenshot](https://github.com/google-deepmind/speech_compass/blob/main/docs/images/app.jpg)

## Quickest way: install the pre-built APK

If you don't need to modify the app, sideload the pre-built APK via ADB:

```
adb install path/to/speechcompass.apk
```

The APK is available on
[Google Drive](https://drive.google.com/file/d/15mf4d6tlzD6GbkNFa18XGd1UUCz8RhcP/view?usp=drive_link&resourcekey=0-Whdp8aFD-M6qDvHfQQJZww).
Connect the phone to your PC before running the command.

> The app may stop working on newer Android versions due to API changes.

## Building from source

1. Install the latest [Android Studio](https://developer.android.com/studio).

2. Download the [zipped Android Studio project](TODO) and unzip it.

3. Open Android Studio and import the project (**File → Open**).

4. Build the project (**Build → Make Project**).

5. Connect the phone over USB and click **Run** to install and launch.
