# Application 2 SPI & DAC

Starts a FreeRTOS task to print Read and Write values to an EEPROM memory using SPI, while turning on 2 LEDs using DAC.


# Description

This application writes values from 0x00 to 0x79 in a eeprom memory using SPI. When the application is writing a value in memory it turns a led on and when its dumping the values to the terminal it turns another led on, using DAC for both.
The voltage of the first led varies with the position in which the program is writing, this means that the voltage will vary from 0 to 3.3v (voltage = ((mem_addr*2)*VDD)/ 255), VDD = 3.3v), the voltage of the second led will always be 2.59v (200*3.3/255 = 2.59v).


## GPIO Configuration

Pins configuration:

GPIO_CS		13
GPIO_MISO	18
GPIO_MOSI	23
GPIO_SCLK	19

LED_writing = DAC_CHANNEL_1 -> GPIO 25
LED_duping = DAC_CHANNEL_2 -> GPIO 26

## Folder contents

The project **hello_world** contains one source file in C language [hello_world_main.c](main/hello_world_main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt` files that provide set of directives and instructions describing the project's source files and targets (executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── example_test.py            Python script used for automated example testing
├── main
│   ├── CMakeLists.txt
│   ├── component.mk           Component make file
│   └── hello_world_main.c
├── Makefile                   Makefile used by legacy GNU Make
└── README.md                  This is the file you are currently reading
```
