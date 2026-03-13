# SpeechCompass

(This is not an officially supported Google product.)

SpeechCompass is a real-time, multi-microphone speech localization,
visualization, and diarization platform. We believe that adding a spatial
dimension to sound understanding can greatly improve the usability of audio
interfaces. For more details see our publication in
[CHI'25](https://arxiv.org/pdf/2502.08848)


![Phone case](images/speech_compass_teaser.jpg)

## Multi microphone phone case design

To allow experimentation, we designed a custom hardware phone case with embedded
four microphones. The localization data is sent from the phone case to the phone
over USB.

![Phone case](images/phone_case.jpg)

## Lightweight localization and beamforming

We implement localization and beamforming algorithms capable of running in
real-time on low-power microcontroller.

![Phone case](images/system_diagram.png)

## Android visualization application

The ASR and visualizations runs as an app on the phone. It actually uses phone
microphone for the ASR and receives the sound direction from the phone case over
USB. ![Phone case](images/app.jpg)


## Documentation

*   [Hardware](https://github.com/google-deepmind/speech_compass/blob/main/docs/hardware/index.md) 
*   [Firmware](https://github.com/google-deepmind/speech_compass/blob/main/docs/firmware/index.md)
*   [Android application](https://github.com/google-deepmind/speech_compass/blob/main/docs/android/index.md)
*   [DSP algorithms](https://github.com/google-deepmind/speech_compass/blob/main/docs/algorithms/index.md)
