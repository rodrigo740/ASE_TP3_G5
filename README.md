# ASE TP3 G5

### Application 1 I2C & PWM

This application reads the values of the temperature sensor using I2C, after each time a value read the mode of operation of the temperature sensor is switched to standby mode. A led is also used with pwm, the duty cycle of this led depends on value from the temperature sensor.

duty = temperature_value/125.0; 

duty = (pow(2, LEDC_DUTY_RES) - 1) * duty, LEDC_DUTY_RES = 13;

### Application 2 SPI & DAC

This application writes values from 0x00 to 0x79 in a eeprom memory using SPI. When the application is writing a value in memory it turns a led on and when its dumping the values to the terminal it turns another led on, using DAC for both.
The voltage of the first led varies with the position in which the program is writing, this means that the voltage will vary from 0 to 3.3v (voltage = ((mem_addr*2)*VDD)/ 255), VDD = 3.3v), the voltage of the second led will always be 2.59v (200*3.3/255 = 2.59v).


### Application 3 ADC & UART

Starts 4 FreeRTOS task that read the analog value provided to a GPIO using an ADC and writes that value in the TX of the UART and is received by the RX of the same UART, the last task is used to detect 7 different types of interrupt events.


### Power Management ADC & UART

Starts 4 FreeRTOS task that read the analog value provided to a GPIO using an ADC and writes that value in the TX of the UART and is received by the RX of the same UART if the difference of the average of the last 5 values and the current value is inferior to 10 the ESP32 is placed in light sleep mode, and is awaken after 2 seconds by a timer. The last task is used to detect 7 different types of interrupt events.


### Simple OTA example

This example is based on `esp_https_ota` component's APIs.

#### Configuration

This Project uses the default `python http server` and at first the example `hello world` application is used, but if an update is triggered the `App1 I2C PWM` is downloaded into the ESP32 board. The wifi settings must be changed in order to work in other networks.

## Repository contents

This repository has 5 subfolders, each one is a ESP IDF Project. Each Project also contains an extensive `README` file that explains the application and the GPIO used, please refer to them for more information about each project.

Below is the configuration of the projects (except the OTA project):

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
