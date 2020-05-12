// Device header
#include <stm32f4xx.h> // Register, bit, mask, data structure definitions

// Set GPIO LEDs bit positions:
#define RED_SET    (1 << 14)
#define GREEN_SET  (1 << 12)
#define BLUE_SET   (1 << 15)
#define ORANGE_SET (1 << 13)

// Output GPIO LEDs bit position:
#define RED_OUTPUT    (1 << 28)
#define GREEN_OUTPUT  (1 << 24)
#define ORANGE_OUTPUT (1 << 26)
#define BLUE_OUTPUT   (1 << 30)

// Enable GPIO port D clock output:
#define GPIOD_CLK_ENABLE (1 << 3)

// LED main function exception stack frames for manual interaction
// (Total of 40 * 32 bytes to accommodate floating point enabled)
uint32_t orange_main_stack[40];
uint32_t blue_main_stack[40];

// Stack pointers pointing to address of the top of
// the exception stack frames
uint32_t *sp_orange_main = &orange_main_stack[40];
uint32_t *sp_blue_main = &blue_main_stack[40];

// Number of elapsed seconds since start
volatile uint32_t seconds;

void systick_isr(void) {
    ++seconds;
}

// Get elapsed ticks
uint32_t get_tick(void) {
    // Disable interrupts while updating to ensure tick remains static while
    // its value is copied:
    __disable_irq();

    uint32_t temp_seconds = seconds;

    // Re-enable as critical section is now complete:
    __enable_irq();

    // Return with ticks:
    return temp_seconds;
}

// Initialize GPIO and SysTick
void gpio_init(void) {
    // Enable clock access for GPIO port D from AHB1 bus so they can be used:
    RCC->AHB1ENR |= GPIOD_CLK_ENABLE;

    // Set PD15-12 pins to output so they can set/clear (blink):
    // (MODER = GPIO port mode register)
    GPIOD->MODER |= (RED_OUTPUT | GREEN_OUTPUT | ORANGE_OUTPUT | BLUE_OUTPUT);

    // Update system clock frequency to ensure SystemCoreClock global variable
    // is correct (clock is in MHz):
    SystemCoreClockUpdate();

    // Configure system tick to clock frequency so interrupt is every 1 second:
    // (Delay [s] = Ticks / Clock Frequency [1/s])
    SysTick_Config(SystemCoreClock);

    // Enable interrupts so we can handle system tick interrupts (what's
    // keeping time for the delay function):
    __enable_irq();
}

void sleep(uint32_t delay) {
    // Get current "time" to know when delay was started:
    uint32_t start = get_tick();

    // Spin until delay has elapsed and exit 
    while ((get_tick() - start) < delay);
}

void blue_on(void) {
    GPIOD->ODR |= BLUE_SET;
}

void red_on(void) {
    GPIOD->ODR |= RED_SET;
}

void orange_on(void) {
    GPIOD->ODR |= ORANGE_SET;
}

void green_on(void) {
    GPIOD->ODR |= GREEN_SET;
}

void led_off(void) {
    // Clear entire port as no other pins are being used:
    GPIOD->ODR = 0x0;
}

// Orange LED blink test
int orange_main(void) {
    while (1) {
        orange_on();
        sleep(1);
        led_off();
        sleep(1);
    }
}

// Blue LED blink test
int blue_main(void) {
    while (1) {
        blue_on();
        sleep(1);
        led_off();
        sleep(1);
    }
}

int main() {
    // Initialize GPIO (clock + mode) for testing:
    gpio_init();

    // Initialize own stack for manual use:
    // *(--sp_color_main) means we will be updating the current memory address 
    // pointed to - 4 bytes (the size of unin32_t) operation after operation 
    // leaving the stack updated from the buttom up. For example:
    // color_main_stack[36] --> color_main_stack[32] -- > color_main_stack[28]
    *(--sp_orange_main) = (1U << 24);              // xPSR
    *(--sp_orange_main) = (uint32_t) &orange_main; // PC
    *(--sp_orange_main) = 0x1;                     // LR
    *(--sp_orange_main) = 0x1;                     // R12
    *(--sp_orange_main) = 0x1;                     // R3
    *(--sp_orange_main) = 0x1;                     // P2
    *(--sp_orange_main) = 0x1;                     // P1
    *(--sp_orange_main) = 0x1;                     // R0

    *(--sp_blue_main) = (1U << 24);            // xPSR
    *(--sp_blue_main) = (uint32_t) &blue_main; // PC
    *(--sp_blue_main) = 0x2;                   // LR
    *(--sp_blue_main) = 0x2;                   // R12
    *(--sp_blue_main) = 0x2;                   // R3
    *(--sp_blue_main) = 0x2;                   // P2
    *(--sp_blue_main) = 0x2;                   // P1
    *(--sp_blue_main) = 0x2;                   // R0

    // Do not return:
    while (1);
}