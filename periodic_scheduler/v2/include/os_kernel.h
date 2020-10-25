// Include guard to prevent double double declarations
#ifndef _OS_KERNEL_H
#define _OS_KERNEL_H

// Include C standard libraries:
#include <stdint.h> // C Standard integer types

// Device header
#include <stm32f4xx.h> // Register, bit, mask, data structure definitions

void os_kernel_launch(uint32_t quanta);

void os_kernel_init(void);

uint8_t os_kernel_add_thread(void(*task_0) (void), void(*task_1) (void),
                             void(*task_2) (void));

#endif // !_OS_KERNEL_H