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

// Number of elapsed seconds:
volatile uint32_t seconds;

// Create interrupt service routine to count elapsed ticks
// (further interrupts are disabled automatically)
void systick_isr(void) {
    // Count elapsed tick:
    ++seconds;
}

// Get elapsed ticks
uint32_t get_tick(void) {
    // Disable interrupts while updating:
    __disable_irq();

    // Get elapsed ticks:
    // (requires systick ISR to be disabled; ensures tick remains static
    // while its value is copied)
    uint32_t temp = seconds;

    // Re-enable:
    __enable_irq();

    // Return with ticks:
    return temp;
}

// Initialize GPIO and SysTick
void gpio_init(void) {
    // Enable clock access for GPIO port D from AHB1 bus:
    RCC->AHB1ENR |= GPIOD_CLK_ENABLE;

    // Set each pin to output:
    // (MODER = GPIO port mode register)
    GPIOD->MODER |= (RED_OUTPUT | GREEN_OUTPUT | ORANGE_OUTPUT | BLUE_OUTPUT);

    // Update system clock frequency:
    // (clock is in MHz)
    SystemCoreClockUpdate();

    // Configure system tick to interrupt every 1 second:
    // (Delay [s] = Ticks / Clock Frequency [1/s])
    SysTick_Config(SystemCoreClock);

    // Enable interrupts so we can handle system tick interrupts:
    __enable_irq();
}

// Sleep
void sleep(uint32_t delay) {
    // Get current elapsed ticks:
    uint32_t temp = get_tick();

    // Spin until delay has been achieved:
    // (Find difference between start and now ticks)
    while ((get_tick() - temp) < delay);
}

// LED Blink Test
int main() {
    // Initialize GPIO:
    gpio_init();

    // Infinite loop:
    while (1) {
        // Set and clear pins:
        // (ODR = GPIO port output data register)
        GPIOD->ODR ^= (RED_SET | BLUE_SET | ORANGE_SET | GREEN_SET);

        // Sleep:
        sleep(1);
    };
}