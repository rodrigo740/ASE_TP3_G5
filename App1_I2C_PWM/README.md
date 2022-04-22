# Application 1 I2C & PWM

Starts a FreeRTOS task to read the temperature value of a temperature sensor using I2C to communicate and turning on a LED using PWM with duty cycle depending on the temperature value.

# Description

This application reads the values of the temperature sensor using I2C, after each time a value read the mode of operation of the temperature sensor is switched to standby mode. A led is also used with pwm, the duty cycle of this led depends on value from the temperature sensor.

duty = temperature_value/125.0; 
duty = (pow(2, LEDC_DUTY_RES) - 1) * duty, LEDC_DUTY_RES = 13;


## Example Folder contents

The project **App1_I2C_PWM** contains one source file in C language [hello_world_main.c](main/hello_world_main.c). The file is located in folder [main](main).

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
