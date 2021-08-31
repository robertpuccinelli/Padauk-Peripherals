# Padauk Peripherals

## Purpose
A portable library for initializing and using  [Padauk microcontroller](http://www.padauk.com.tw/en/product/method.aspx?num=1) peripherals.

## Background

Padauk microcontrollers gained some recognition in the hobbiest community  around 2018 for their cheap cost and minimalistic features. There was a lot of discussion regarding their pros and cons at the time and I'll point you to [Jay Carlson's in-depth review](https://jaycarlson.net/2019/09/06/whats-up-with-these-3-cent-microcontrollers/) if you want to learn more. However, despite the enthusiasm, these microcontrollers really lacked interesting examples that either put the ICs to their limits or helped make the code readable and portable. In June 2021, the extent of publicly available code is either the equivalent of "blink" or literal copy-pasta of the IDE's code generator.

While the original goal of tihs repository was to improve the usability of Padauk ICs in my own projects, I hope that the code and examples listed here will be more informative than what's currently available.

## How to use

1. Load [system_settings.h](./system_settings.h) into your Padauk IDE project
2. Load the desired peripheral headers and sources into the project
3. Edit system_settings.h as described in the following steps
4. Change SYSTEM_CLOCK to match SYSCLK defined in your program
5. Change IC_TARGET to match the Padauk microcontroller being programmed
    * **NOTE:** If your IC is not listed, create an entry in the SUPPORTED MICROCONTROLLERS section. It is important to correctly define the IC features. Refer to the IC datasheet or Padauk's website for a complete list of peripherals.
6. Enable the desired pheripherals such as PERIPH_I2C_M
7. Change peripheral configuration in the PERIPHERAL SETTINGS section
    * **NOTE:** Different settings can have a large impact on RAM/ROM usage. As an example, the 11-bit PWM has a utility that can automatically find the best register settings for a desired PWM frequency. However, it uses ~20B of RAM and ~200B of ROM! Disable the setting if it isn't needed or if the IC is short on memory.

## Testing

In addition to providing peripheral header and source files, the [PeripheralTest](./PeripheralTest/) directory contains a Padauk IDE project that loads the peripherals and demonstrates how to use properly use them. This is the same project that I use to validate that the sources compile and I also use it to evaluate how much RAM and ROM is consumed by each peripheral. The resource usage of each peripheral is located in the description at the top of the header file. 
This library is currently not tested on a Padauk 5S-I-S02B in-circuit emulator, but it will be soon. Using the emulator will allow for behavior validation and permit compiler optimizations. 

## Example Code and Projects

### Peristaltic Pump
This project currently uses the **I2C Master** and the **11-bit PWM** peripherals. The I2C communicates with a 16x2 LCD display and a 128-bit EEPROM that stores settings and calibration data.
