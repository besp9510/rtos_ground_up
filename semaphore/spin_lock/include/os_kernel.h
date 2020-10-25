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

void os_thread_yield(void);

void os_semaphore_init(int32_t *semaphore, int32_t value);

void os_signal_set(int32_t *semaphore);

void os_signal_wait(int32_t *semaphore);

#endif // !_OS_KERNEL_H