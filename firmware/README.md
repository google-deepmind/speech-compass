# Firmware

The firmware runs on the STM32 L5 microcontroller (ARM Cortex-M33). It reads raw audio
from the four microphones, runs the GCC-PHAT localization algorithm, and streams azimuth
angle estimates to the phone over USB.

The firmware source is provided as an STM32CubeIDE project. Loading it onto the MCU
requires an ST-LINK programmer.

## Compiling

1. Install [STM32Cube IDE](https://www.st.com/software/stm32cube-ide) and the ST-LINK
   toolchain.

2. Download the
   [zipped project](https://drive.google.com/file/d/1aSLFQMz3HJg2O-bxhoN2yHJ5k2ODyI81/view?usp=sharing&resourcekey=0-FB9BwKRDcssJl4RME0ycYQ)
   and unzip it.

3. Import into STM32Cube IDE: **File → Import → Existing Projects into Workspace** and
   select the project folder.

4. Build the project. The console should show no errors.

## Flashing

Flashing requires a programmer and a tag-connect cable:

- [ST-LINK V3 Mini programmer](https://www.mouser.com/ProductDetail/STMicroelectronics/STLINK-V3MINIE?qs=MyNHzdoqoQKcLQe5Jawcgw%3D%3D)
- [Tag-Connect TC2030-CTX-STDC14 cable](https://www.tag-connect.com/product/tc2030-ctx-stdc14-for-use-with-stm32-processors-with-stlink-v3) (compact footprint)

1. Connect a USB cable for board power (the programmer does not supply power).
2. Hold the tag-connect cable against the board's programming header.
3. In STM32Cube IDE, click the debug/flash button. On first use, configure the programmer
   if prompted.
4. To verify: open a serial terminal (e.g., Arduino IDE serial monitor) on the USB port.
   You should see angle values printing continuously. Baud rate does not matter.

> **Note:** Flashing can also be done without the IDE by using ST-LINK command-line tools
> to flash a pre-compiled `.hex` or `.bin` binary directly.
