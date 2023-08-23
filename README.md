# Padauk Peripherals

## Purpose
A portable, human-readable library for initializing and using [Padauk microcontroller](http://www.padauk.com.tw/en/product/method.aspx?num=1) peripherals. The original goal of this repository was to improve the usability of Padauk ICs in my own projects, but I hope that the code and examples listed here will extend the accessibility of these cheap micros for others as well.

## Background

Padauk microcontrollers gained some recognition in the hobbiest community around 2018 for their cheap cost and minimalistic features. There was a lot of discussion regarding their pros and cons at the time and I'll point you to [Jay Carlson's in-depth review](https://jaycarlson.net/2019/09/06/whats-up-with-these-3-cent-microcontrollers/) if you want to learn more. However, despite the enthusiasm, these microcontrollers really lacked interesting examples that either put the ICs to their limits or helped make the code human-readable and portable. In June 2021, the extent of publicly available code was virtually the equivalent of "blink", [7-segment display drivers](https://cpldcpu.wordpress.com/2020/04/05/addressable-7-segment-display/), or literal copy-pasta of the IDE's code generator.

While this library does provide an extensive set of peripheral utilities that have been designed to minimize ROM and RAM usage when possible, a major downside is that nested function calls may consume more RAM than expected. As an example, if I call a function in the main loop of the program, the stack depth is 2 and 4 bytes of RAM are used to maintain this stack. If the function I called calls another function before returning, then the stack depth is 3 and 6 bytes of RAM are used to maintain the stack. Function calls/stack depth can be limiting when pushing the limits of resource constrained micros like the PMS150C.

To help with developing resource constrained programs, each utility lists the ROM and RAM used when all functions are called in the file description. In some cases, advance features can be activated by enabling that option in the system_settings.h configuration file. An example would be [automated parameter solving for the 11b PWM](./pdk_pwm_11b.c). WITHOUT the automated frequency solver, the utility uses up to 281B of ROM and 18B of RAM. WITH the solver, maximum resource usage jumps to 648B of ROM and 39B of RAM!

One quirk of mini C used by the IDE is that arguments cannot be passed to functions, so variables are often set before a function call. It might help to review examples in [PeripheralTest.C](./PeripheralTest/PeripheralTest.C) or the of some of my public projects found at the bottom of this README.

## How to use

1. Load [**system_settings.h**](./system_settings.h) into your Padauk IDE project along with the desired peripheral headers and sources.
2. Edit **system_settings.h** as described in the following steps:
3. Change `SYSTEM_CLOCK` to match `SYSCLK` defined in your main program.
4. Change `IC_TARGET` to match the Padauk microcontroller being programmed.
    * **NOTE:** If your IC is not listed, create an entry in the *`SUPPORTED MICROCONTROLLERS`* section. It is important to correctly define the IC features. Refer to the IC datasheet or Padauk's website for a complete list of peripherals.
5. Enable the desired peripherals.
    * **NOTE:** If you have a peripheral like a 7-segment display that uses I2C, enable both `PERIPH_7SEGMENT` and `PERIPH_I2C`.
6. To help keep track of what pins and peripherals are used in your project, consider planning your project in the *`RESOURCE UTILIZATION`* section.
7. Modify peripheral configurations in the *`PERIPHERAL SETTINGS`* section. This section is used for pin assignments and for storing definitions such as register names and parameters if they don't already exist.
    * **NOTE:** Different settings can have a large impact on RAM/ROM usage. As an example, this is where you would enable/disable the 11b PWM solver mentioned in the background above. Disable extra features if they aren't needed or if the IC is short on memory.
8. `#include system_settings.h` and other relevant peripheral headers in your main program. Examples for how to use the peripherals can be found in [PeripheralTest.C](./PeripheralTest/PeripheralTest.C).

## Testing

In addition to providing peripheral header and source files, the [PeripheralTest](./PeripheralTest/) directory contains a Padauk IDE project that loads the peripherals and generally demonstrates how variables are set and functions are called. This is the same project that I use to debug the source code and evaluate the maximum RAM and ROM consumed by each peripheral. The resource usage of each peripheral is located in the description at the top of the header file. 

This library is partially tested on a Padauk 5S-I-S02B in-circuit emulator (ICE) to validate timing and behavior. You are welcome to reach out if you're interested in having your code validated on the ICE or programming your micros.

## Example Projects

### [Basic Optical Density Meter](https://gitlab.com/puccilabs/PLOD1)
This is a commonly used tool in research labs - it helps scientists estimate concentrations by measuring the relative light absorption of a sample. It is often used to monitor the growth of bacterial cultures.

Microcontroller: 
* PMS150C, SOT23
* 2KB ROM
* 64B RAM
* 4 IO

Peripherals:
* I2C, 7-segment display
* I2C, APDS light sensor
* 8b PWM, LED control
* Digital Input / Wake-up, microswitch
