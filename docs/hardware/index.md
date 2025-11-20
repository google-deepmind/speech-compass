# SpeechCompass hardware design

The hardware is composed of two PCBs: the main board with the microcontroller
and flexible PCB connecting all the microphones together.

[TOC]

![Phone case](/third_party/deepmind/speech_compass/docs/images/electronics.jpg)

## Main PCB

The main PCB is a motherboard that has the STM32 microcontroller and I/O ports.
The board includes an audio codec that provides headphone output. There is a
Bluetooth module as well, but we are not using it. With Bluetooth and the
battery, the system does not need to be tethered to the phone.
[Schematic pdf](/third_party/deepmind/speech_compass/docs/hardware/main_board_schematic.pdf)

## Flex PCB

Flexible PCB is mainly a cable to connect the microphones to the main board. The
surface mount microphones were soldered to the flex PCB.
[Schematic pdf](/third_party/deepmind/speech_compass/docs/hardware/flex_pcb_schematic.pdf)

## Old version (LiveLocalizer)

Our initial version of the phone case had one rigid board for everything. (See
UIST demo [proceedings](https://dl.acm.org/doi/10.1145/3586182.3615789) for
details). It is more bulky but it is simpler to build and uses microphones on a
breakout boards. It can run the same firmware.

![Phone case](/third_party/deepmind/speech_compass/docs/images/livelocalizer.png)

## Firmware

The firmware runs on the microcontroller. Mainly it runs the localization
algorithm and sends the data to the phone
