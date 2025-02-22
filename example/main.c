#include "ch32v003fun.h"
#include "mlx90614.h"

// Utility function to print float values
void print_float(float value) {
    int32_t integer_part = (int32_t)value;
    int32_t decimal_part = (int32_t)((value - integer_part) * 100);
    if (decimal_part < 0) decimal_part = -decimal_part;
    printf("%ld.%02ld", integer_part, decimal_part);
}
int main() {
    SystemInit();

    // Setup UART for debugging
    uint32_t baud = 115200;
    uint32_t uartBRR = (FUNCONF_SYSTEM_CORE_CLOCK + baud / 2) / baud;
    SetupUART(uartBRR);

    printf("\r\nMLX90614 Temperature Sensor Test\r\n");

    MLX90614_Init();

    if (MLX90614_IsDeviceReady()) {
        printf("MLX90614 initialized successfully\r\n");

        while (1) {
            float ambient = MLX90614_ReadAmbientTemp();
            float object = MLX90614_ReadObjectTemp();

            printf("Ambient: ");
            print_float(ambient);
            printf("°C, Object: ");
            print_float(object);
            printf("°C\r\n");

            Delay_Ms(1000);
        }
    } else {
        printf("MLX90614 not found! Check connections\r\n");
        while (1);
    }
}
