// Device header
#include <stm32f4xx.h> // Register, bit, mask, data structure definitions

// Set GPIO LEDs:
#define RED_SET    (1 << 14)
#define GREEN_SET  (1 << 12)
#define BLUE_SET   (1 << 15)
#define ORANGE_SET (1 << 13)

// Output GPIO LEDs:
#define RED_OUTPUT    (1 << 28)
#define GREEN_OUTPUT  (1 << 24)
#define ORANGE_OUTPUT (1 << 26)
#define BLUE_OUTPUT   (1 << 30)

// Enable GPIO port D clock output:
#define GPIOD_CLK_ENABLE (1 << 3)

// Turn on LEDs:
int main() {
    // Enable clock access for GPIO port D from AHB1 bus:
    RCC->AHB1ENR |= GPIOD_CLK_ENABLE;

    // Set each pin to output:
    // (MODER = GPIO port mode register)
    GPIOD->MODER |= (RED_OUTPUT | GREEN_OUTPUT | ORANGE_OUTPUT | BLUE_OUTPUT);

    // Infinite loop:
    while (1) {
        // Set pin high:
        // (ODR = GPIO port output data register)
        GPIOD->ODR |= (RED_SET | BLUE_SET | ORANGE_SET | GREEN_SET);
    };
}