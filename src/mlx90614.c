#include "mlx90614.h"

// Private function declarations
static uint16_t MLX90614_ReadReg(uint8_t reg);
static void MLX90614_CheckBus(void);

void MLX90614_Init(void) {
    // Enable GPIOC peripheral clock
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;

    // Configure PC1 (SCL) and PC2 (SDA) as open-drain outputs
    GPIOC->CFGLR &= ~(0xFF << (4 * 1));  // Clear PC1, PC2 config
    GPIOC->CFGLR |=
        ((GPIO_CNF_OUT_OD_AF << 4) | (GPIO_Speed_10MHz << 4)) |  // PC1
        ((GPIO_CNF_OUT_OD_AF << 8) | (GPIO_Speed_10MHz << 8));   // PC2

    // Set pins high initially
    GPIOC->BSHR = (1 << 1) | (1 << 2);
    Delay_Ms(10);

    // Enable and configure I2C
    RCC->APB1PCENR |= RCC_APB1Periph_I2C1;

    // Reset I2C
    I2C1->CTLR1 |= I2C_CTLR1_SWRST;
    Delay_Ms(10);
    I2C1->CTLR1 &= ~I2C_CTLR1_SWRST;

    // Configure I2C
    I2C1->CTLR2 = 24;    // Input clock 24 MHz
    I2C1->CKCFGR = 240;  // ~50kHz I2C clock
    I2C1->CTLR1 |= I2C_CTLR1_PE;

    Delay_Ms(100);  // Wait for I2C to stabilize

    MLX90614_CheckBus();  // Check and recover bus if needed
}

float MLX90614_ReadAmbientTemp(void) {
    uint16_t raw = MLX90614_ReadReg(MLX90614_TA);
    if (raw == 0) return -999.0f;
    return (float)raw * 0.02f - 273.15f;
}

float MLX90614_ReadObjectTemp(void) {
    uint16_t raw = MLX90614_ReadReg(MLX90614_TOBJ1);
    if (raw == 0) return -999.0f;
    return (float)raw * 0.02f - 273.15f;
}

uint8_t MLX90614_IsDeviceReady(void) {
    uint32_t timeout = 1000;

    I2C1->CTLR1 |= I2C_CTLR1_START;
    while (!(I2C1->STAR1 & I2C_STAR1_SB) && --timeout);
    if (!timeout) return 0;

    I2C1->DATAR = MLX90614_I2CADDR << 1;
    timeout = 1000;

    while (!(I2C1->STAR1 & I2C_STAR1_ADDR) && --timeout) {
        if (I2C1->STAR1 & I2C_STAR1_AF) {
            I2C1->STAR1 &= ~I2C_STAR1_AF;
            I2C1->CTLR1 |= I2C_CTLR1_STOP;
            return 0;
        }
    }

    if (!timeout) return 0;

    (void)I2C1->STAR2;
    I2C1->CTLR1 |= I2C_CTLR1_STOP;

    return 1;
}

static void MLX90614_CheckBus(void) {
    // Read current pin states
    uint32_t scl_state = (GPIOC->INDR & (1 << 1)) != 0;
    uint32_t sda_state = (GPIOC->INDR & (1 << 2)) != 0;

    if (!scl_state || !sda_state) {
        // Disable I2C temporarily
        I2C1->CTLR1 &= ~I2C_CTLR1_PE;

        // Configure pins as GPIO output open-drain
        GPIOC->CFGLR &= ~(0xFF << (4 * 1));
        GPIOC->CFGLR |= (GPIO_CNF_OUT_OD << 4) | (GPIO_Speed_10MHz << 4) |
                        (GPIO_CNF_OUT_OD << 8) | (GPIO_Speed_10MHz << 8);

        // Toggle SCL up to 9 times
        for (int i = 0; i < 9; i++) {
            GPIOC->BSHR = (1 << (1 + 16));  // SCL LOW
            Delay_Ms(1);
            GPIOC->BSHR = (1 << 1);  // SCL HIGH
            Delay_Ms(1);
        }

        // Generate STOP condition
        GPIOC->BSHR = (1 << (2 + 16));  // SDA LOW
        Delay_Ms(1);
        GPIOC->BSHR = (1 << 1);  // SCL HIGH
        Delay_Ms(1);
        GPIOC->BSHR = (1 << 2);  // SDA HIGH
        Delay_Ms(1);

        // Reconfigure pins for I2C
        GPIOC->CFGLR &= ~(0xFF << (4 * 1));
        GPIOC->CFGLR |= ((GPIO_CNF_OUT_OD_AF << 4) | (GPIO_Speed_10MHz << 4)) |
                        ((GPIO_CNF_OUT_OD_AF << 8) | (GPIO_Speed_10MHz << 8));

        // Re-enable I2C
        I2C1->CTLR1 |= I2C_CTLR1_PE;
    }
}

static uint16_t MLX90614_ReadReg(uint8_t reg) {
    uint8_t data[3];
    uint32_t timeout;

    // Start condition
    I2C1->CTLR1 |= I2C_CTLR1_START;
    timeout = 1000;
    while (!(I2C1->STAR1 & I2C_STAR1_SB) && --timeout);
    if (!timeout) return 0;

    // Send device address (write)
    I2C1->DATAR = MLX90614_I2CADDR << 1;
    timeout = 1000;
    while (!(I2C1->STAR1 & I2C_STAR1_ADDR) && --timeout);
    if (!timeout) return 0;
    (void)I2C1->STAR2;

    // Send register address
    I2C1->DATAR = reg;
    timeout = 1000;
    while (!(I2C1->STAR1 & I2C_STAR1_BTF) && --timeout);
    if (!timeout) return 0;

    // Repeated START
    I2C1->CTLR1 |= I2C_CTLR1_START;
    timeout = 1000;
    while (!(I2C1->STAR1 & I2C_STAR1_SB) && --timeout);
    if (!timeout) return 0;

    // Send device address (read)
    I2C1->DATAR = (MLX90614_I2CADDR << 1) | 1;
    timeout = 1000;
    while (!(I2C1->STAR1 & I2C_STAR1_ADDR) && --timeout);
    if (!timeout) return 0;

    // Enable ACK
    I2C1->CTLR1 |= I2C_CTLR1_ACK;
    (void)I2C1->STAR2;

    // Read data bytes
    for (int i = 0; i < 3; i++) {
        if (i == 2) {
            I2C1->CTLR1 &= ~I2C_CTLR1_ACK;  // NACK for PEC byte
            I2C1->CTLR1 |= I2C_CTLR1_STOP;  // Generate STOP
        }

        timeout = 1000;
        while (!(I2C1->STAR1 & I2C_STAR1_RXNE) && --timeout);
        if (!timeout) return 0;
        data[i] = I2C1->DATAR;
    }

    return (data[1] << 8) | data[0];  // Return temperature value
}
