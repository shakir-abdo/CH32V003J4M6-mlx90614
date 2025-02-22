#ifndef MLX90614_H
#define MLX90614_H

#include "ch32v003fun.h"
#include <stdint.h>
#include <stdio.h>


// MLX90614 I2C Address (default)
#define MLX90614_I2CADDR 0x5A

// RAM registers
#define MLX90614_RAWIR1 0x04
#define MLX90614_RAWIR2 0x05
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_TOBJ2 0x08

// EEPROM registers
#define MLX90614_TOMAX 0x20
#define MLX90614_TOMIN 0x21
#define MLX90614_PWMCTRL 0x22
#define MLX90614_TARANGE 0x23
#define MLX90614_EMISSIVITY 0x24
#define MLX90614_CONFIG 0x25
#define MLX90614_ADDR 0x2E
#define MLX90614_ID1 0x3C
#define MLX90614_ID2 0x3D
#define MLX90614_ID3 0x3E
#define MLX90614_ID4 0x3F

// Function declarations
void MLX90614_Init(void);
float MLX90614_ReadAmbientTemp(void);
float MLX90614_ReadObjectTemp(void);
uint8_t MLX90614_IsDeviceReady(void);

#endif
