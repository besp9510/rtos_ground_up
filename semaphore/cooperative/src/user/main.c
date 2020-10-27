// SysTick Round Robin Scheduler
#include "os_kernel.h"

// Scheduler quanta in milliseconds:
#define QUANTA 10

// Counters will keep track of thread switches:
uint32_t count;
uint32_t count_1;

// Semaphores for tasks to use:
uint32_t semaphore_1;
uint32_t semaphore_2;

// Example tasks that just count
void task_0(void) {
    while (1) {
        os_signal_wait(&semaphore_1);
        --count;
        os_signal_set(&semaphore_2);
    };
}

void task_1(void) {
    while (1) {
        os_signal_wait(&semaphore_2);
        ++count;
        os_signal_set(&semaphore_1);
    };
}

void task_2(void) {
    while (1) {
        ++count_1;
    };
}

// Simple example of scheduler with three threads
int main(void) {
    count = 0;
    count_1 = 0;

    // Set up semaphores for tasks to use:
    os_semaphore_init(&semaphore_1, 1);
    os_semaphore_init(&semaphore_2, 0);

    // Add threads to scheduler and start:
    os_kernel_init();
    os_kernel_add_thread(&task_0, &task_1, &task_2);
    os_kernel_launch(QUANTA);

    // Spin forever
    while (1) {};

    // Will never reach this:
    return 0;
}
