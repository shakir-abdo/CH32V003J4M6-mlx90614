# MLX90614 Temperature Sensor Library for CH32V003j4m6

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: CH32V003](https://img.shields.io/badge/Platform-CH32V003-blue.svg)](http://www.wch-ic.com/products/CH32V003.html)

A library for interfacing the MLX90614 infrared temperature sensor with CH32V003j4m6 microcontroller.

## Features
- Read ambient temperature
- Read object temperature
- I2C bus monitoring and recovery
- Configurable I2C speed

## Hardware Connections
```
MLX90614 → CH32V003
VDD     → 3.3V
GND     → GND
SCL     → PC1 (+ 4.7kΩ pull-up to 3.3V)
SDA     → PC2 (+ 4.7kΩ pull-up to 3.3V)
```

## Usage Example
```c
#include "MLX90614.h"

int main() {
    SystemInit();
    SetupUART(UART_BAUD_115200);

    MLX90614_Init();

    while(1) {
        float ambient = MLX90614_ReadAmbientTemp();
        float object = MLX90614_ReadObjectTemp();
        // Use the temperatures...
        Delay_Ms(1000);
    }
}
```

## License
MIT License

