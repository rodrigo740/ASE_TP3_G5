# Application 3 ADC UART

Starts 4 FreeRTOS task that read the analog value provided to a GPIO using an ADC and writes that value in the TX of the UART and is received by the RX of the same UART, the last task is used to detect 7 different types of interrupt events.


## GPIO Configuration

Pins configuration:

TXD_PIN GPIO 17

RXD_PIN GPIO 16

ADC GPIO 15


## Example Folder contents

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
