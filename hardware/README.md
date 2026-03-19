# Hardware

The SpeechCompass phone case consists of two PCBs.

![Electronics](https://github.com/google-deepmind/speech_compass/blob/main/docs/images/electronics.jpg)

## Main PCB

The main board hosts the STM32 L5 microcontroller, an audio codec (headphone output),
and a Bluetooth module (currently unused). With a battery added, the system can operate
untethered from the phone.

[Schematic (PDF)](main_board_schematic.pdf)

## Flex PCB

The flexible PCB routes the four surface-mount microphones back to the main board.

[Schematic (PDF)](flex_pcb_schematic.pdf)

## Earlier version: LiveLocalizer

The original prototype used a single rigid PCB — bulkier but simpler to build, with
microphones on breakout boards. It runs the same firmware. See the
[UIST 2023 demo paper](https://dl.acm.org/doi/10.1145/3586182.3615789) for details.

![LiveLocalizer](https://github.com/google-deepmind/speech_compass/blob/main/docs/images/livelocalizer.png)
