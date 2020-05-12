// LED blink program except now in assembly

.equ RCC_AHB1ENR, 0x40023830            // GPIO clock access
.equ GPIOD_MODER, 0x40020C00            // GPIO port D mode
.equ GPIOD_ODR, 0x40020C14              // GPIO port D output

.equ DELAY, 0x186004                    // Delay (1 second)

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.section .text, "x"                     // Assemble new code "text" section
                                        // as executable

.global main                            // Make callable from startup.s
                                        // (aka symbol external linkage)

main:
            BL      gpiod_init          // Initilize GPIOs (clock + pins)
loop:       BL      light_on            // Turn LEDs on
            BL      delay               // Delay 1 second
            BL      light_off           // Turn LEDS off
            BL      delay               // Delay 1 second
            B       loop

gpiod_init:
            LDR     R1, =RCC_AHB1ENR    // Load AHB1 enable bus address for clk
            LDR     R0, [R1]            // Save content to manipulate
            ORR     R0, R0, #0x08       // Create word to enable GPIO D clk
            STR     R0, [R1]            // Enable clock access

            LDR     R1, =GPIOD_MODER    // Load GPIO D mode bus address
            LDR     R0, [R1]            // Save content to manipulate
            LDR     R0, =#0x55000000    // Create word to enable PD15-12
            STR     R0, [R1]            // Set pins to output now

light_on:
            LDR     R1, =GPIOD_ODR      // Load GPIO D output bus address
            LDR     R0, [R1]            // Save content to manipulate
            ORR     R0, R0, #0xF000     // Create word to turn on PD15-12
            STR     R0, [R1]            // Set pins now

            BX      LR

light_off:
            LDR     R1, =GPIOD_ODR      // Load GPIO D output bus address
            LDR     R0, [R1]            // Save that register's content
            LDR     R0, =#0x0           // Word to enable PD15-12
            STR     R0, [R1]            // Clear pins now

            BX      LR

delay:
            LDR     R2, =DELAY          // Load delay duration to subtract
subtract:   SUBS    R2, R2, #0x1        // Subtract but set cond flag (Z)
            IT      EQ                  // Subtract to zero will set Z=1 (EQ)
            BXEQ    LR                  // Delay as elapsed
            B       subtract            // Keep subtracting to get full delay