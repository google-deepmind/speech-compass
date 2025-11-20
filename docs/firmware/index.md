# Firmware

The firmware runs on a low-power microcontroller (STM32 L5). It gets the raw
microphone data, runs lightweight localization and signal processing algorithms
and outputs results to the USB. Loading firmware on the MCU will need a cable
and an ST-LINK programmer. The steps assume some previous experience with STM32.

## Compiling the firmware

We used [STM32Cube IDE](https://www.st.com/software/stm32cube-ide) for firmware
development. It provides all the convenient tools for embedded ARM development.
We used the STM32CubeMX to create the project template and import the necessary
drivers. The most convenient way to access the code is to compile the code using
STM32Cube IDE as follows:

1) Install the STM32 CUBE IDE and ST-LINK toolchain.

2) Download the
[zipped project](https://drive.google.com/file/d/1aSLFQMz3HJg2O-bxhoN2yHJ5k2ODyI81/view?usp=sharing&resourcekey=0-FB9BwKRDcssJl4RME0ycYQ)
and unzip it.

3) Import the project into STM32Cube IDE. Click on File -> Import -> Existing
Projects into Workspace, and select the project folder.

4) Build the project. The console should show no errors.

## Loading the firmware

Loading and debugging the firmware on the microcontroller is more involved as it
requires a programmer and a specific connector.

1) Get an
[ST-LINK programmer](https://www.mouser.com/ProductDetail/STMicroelectronics/STLINK-V3MINIE?qs=MyNHzdoqoQKcLQe5Jawcgw%3D%3D)
and a special
[connector/cable](https://www.tag-connect.com/product/tc2030-ctx-stdc14-for-use-with-stm32-processors-with-stlink-v3).
We used such a connector to reduce physical footprint.

2) Plug in a USB cable for board power. The programmer doesn't provide power for
the board.

3) Open the STM32Cube project and compile. Alternatively, this can be done
without STM32Cube IDE by flashing the compiled binary file with the code. This
can be done over a terminal, but still needs ST-LINK drivers installed.

4) Connect and hold the connector to the board and upload by clicking the debug
button. If doing this the first time, the programmer might need to be
configured.

5) Open a serial terminal (e.g, Arduino IDE) on a PC connected to the board over
USB. Make sure the correct port is selected. The baud rate doesn't matter. You
should see angles coming in and printing.
