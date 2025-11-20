# Android application

This application runs on the phone, and receives the data from the SpeechCompass
phone case over the USB. We used the
[Processing](https://android.processing.org/) framework for visualizations.

## Simplest way to run the app

If no debugging or development is need, loading the app
[APK](https://drive.google.com/file/d/15mf4d6tlzD6GbkNFa18XGd1UUCz8RhcP/view?usp=drive_link&resourcekey=0-Whdp8aFD-M6qDvHfQQJZww)
over [Android Debug Bridge](https://developer.android.com/tools/adb) (adb) is
the easiest way. To do so connect the phone to the PC, open the terminal and
load the app with the command line:

```adb install path_to_app```

The app might stop running on the newer version of Android.

## Building the Android application

Building the app is more involved, especially for first time users.

1) Download and install latest
[Android studio](https://developer.android.com/studio).

2) Download the
[zipped](https://drive.google.com/file/d/1jFQJekD14Jx-xwWLs2wC-8Qwh1GPZKnF/view?resourcekey=0-wajrcNOpDrbjtKxqiirDPg)
Android Studio project for SpeechCompass.

3) Import the project to Android Studio.

4) Build the project.

5) Connect the phone over USB and load the application by clicking the Run
button in Android Studio.
