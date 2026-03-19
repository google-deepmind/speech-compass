# SpeechCompass: Enhancing Mobile Captioning with Diarization and Directional Guidance via Multi-Microphone Localization

[![CHI 2025 Best Paper](https://img.shields.io/badge/CHI%202025-Best%20Paper%20Award-gold)](https://dl.acm.org/doi/10.1145/3706598.3713631)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![arXiv](https://img.shields.io/badge/arXiv-2502.08848-b31b1b.svg)](https://arxiv.org/abs/2502.08848)

[Paper (PDF)](https://arxiv.org/pdf/2502.08848) | [ACM Digital Library](https://dl.acm.org/doi/10.1145/3706598.3713631) | [Project Page](https://www.olwal.com/speechcompass) | [Google Research Blog](https://research.google/blog/making-group-conversations-more-accessible-with-sound-localization/)

Artem Dementyev*, Dimitri Kanevsky, Samuel J. Yang, Mathieu Parvaix, Chiong Lai, Alex Olwal*

Official code release for **SpeechCompass: Enhancing Mobile Captioning with Diarization and
Directional Guidance via Multi-Microphone Localization**, published at CHI 2025.

![SpeechCompass teaser](docs/images/speech_compass_teaser.jpg)

<small>*First and last author contributed equally to this work</small>

## Overview

Mobile speech-to-text apps have a fundamental limitation in group conversations: they
transcribe everything into a single undifferentiated stream, making it hard to follow who
said what. SpeechCompass addresses this by adding a spatial dimension — using multiple
microphones to localize speakers in real time and overlay directional guidance on live
captions.

The system is designed with accessibility in mind, particularly for people who are hard of
hearing. Rather than relying on machine learning approaches that require video, speaker
embeddings, or high compute, SpeechCompass uses classical DSP (GCC-PHAT + kernel density
estimation) that runs on a low-power embedded microcontroller with low latency and no voice
data retention.

![App](docs/images/app.jpg)

### Visualizations

The Android app offers multiple ways to display speaker direction alongside captions:

- **Colored text** — each speaker gets a distinct color
- **Directional arrows and glyphs** — indicate where speech is coming from
- **Radar minimap** — a persistent spatial overview of active speakers
- **Edge indicators** — subtle screen-edge cues for peripheral awareness
- **Speech suppression** — filter out speech from a specific direction

### Performance

- **Localization accuracy:** 11°–22° average error at normal conversational volume (60–65 dB),
  comparable to human localization ability
- **Diarization:** 4-microphone configuration achieves 23–35% relative improvement in
  Diarization Error Rate (DER) over a 3-microphone setup across varying SNR conditions

### User Research

A survey of 263 frequent captioning users identified speaker distinction as the most
significant unmet need. In a follow-up prototype study with 8 frequent users, colored text
and directional arrows were the preferred visualizations, and all participants agreed that
directional guidance was valuable for group conversations.

## System

![System diagram](docs/images/system_diagram.png)

SpeechCompass combines a custom hardware phone case with lightweight on-device processing:

- A **4-microphone phone case** sends audio to an STM32 L5 microcontroller, which runs
  GCC-PHAT localization and streams azimuth angles to the phone over USB
- The **Android app** uses the phone's built-in microphone for speech recognition (ASR)
  and receives speaker direction from the case — keeping voice data local and processing
  costs low
- The **DSP algorithms** are written in portable C11 and can also run on phones with
  2+ built-in microphones, providing 180° localization without additional hardware

## Repository Structure

| Component | Description |
|-----------|-------------|
| [`hardware/README.md`](hardware/README.md) | PCB schematics for the custom 4-microphone phone case |
| [`firmware/README.md`](firmware/README.md) | STM32 L5 firmware (GCC-PHAT localization → USB output) |
| [`dsp/README.md`](dsp/README.md) | Platform-agnostic C localization and beamforming algorithms, with Bazel unit tests |
| [`android/README.md`](android/README.md) | Android Studio app (ASR + directional visualization) |

Each component can be used independently — in particular, the DSP algorithms can be built
and tested with Bazel without any hardware.

## Citing this work

```bibtex
@inproceedings{dementyev2025speechcompass,
  title={SpeechCompass: Enhancing Mobile Captioning with Diarization and Directional Guidance via Multi-Microphone Localization},
  author={Dementyev, Artem and Kanevsky, Dimitri and Yang, Samuel and Parvaix, Mathieu and Lai, Chiong and Olwal, Alex},
  booktitle={Proceedings of the 2025 CHI Conference on Human Factors in Computing Systems},
  year={2025}
}
```

## Related Work

SpeechCompass builds on **LiveLocalizer** (UIST 2023), which first demonstrated
microphone-array localization augmenting mobile speech-to-text. The same hardware can run
the SpeechCompass firmware.

> Dementyev, A., Kanevsky, D., Yang, S., Parvaix, M., Lai, C., and Olwal, A.
> "LiveLocalizer: Augmenting Mobile Speech-to-Text with Microphone Arrays, Optimized
> Localization and Beamforming." *UIST 2023 Adjunct*, San Francisco, CA.
> [ACM DL](https://dl.acm.org/doi/10.1145/3586182.3615789)

## Acknowledgments

We thank Sagar Savla, Dmitrii Votintcev, Pascal Getreuer, Richard Lyon, Alex Huang, Shao-Fu Shih,
Chet Gnegy, Shaun Kane, James Landay, Malcolm Slaney, Meredith Morris, Carson Lau,
Ngan Nguyen, Mei Lu, Don Barnett, Ryan Geraghty, and Sanjay Batra for their contributions
and support.

## License and disclaimer

Copyright 2025 Google LLC

All software is licensed under the Apache License, Version 2.0 (Apache 2.0);
you may not use this file except in compliance with the Apache 2.0 license.
You may obtain a copy of the Apache 2.0 license at:
https://www.apache.org/licenses/LICENSE-2.0

All other materials are licensed under the Creative Commons Attribution 4.0
International License (CC-BY). You may obtain a copy of the CC-BY license at:
https://creativecommons.org/licenses/by/4.0/legalcode

Unless required by applicable law or agreed to in writing, all software and
materials distributed here under the Apache 2.0 or CC-BY licenses are
distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied. See the licenses for the specific language governing
permissions and limitations under those licenses.

This is not an official Google product.
