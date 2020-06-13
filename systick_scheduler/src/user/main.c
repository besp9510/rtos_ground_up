// SysTick Round Robin Scheduler
#include "os_kernel.h"

// Scheduler quanta in milliseconds:
#define QUANTA 10

// Counters will keep track of thread switches:
uint32_t count_0;
uint32_t count_1;
uint32_t count_2;

// Example tasks that just count
void task_0(void) {
    while (1) {
        count_0++;
    };
}

void task_1(void) {
    while (1) {
        count_1++;
    };
}

void task_2(void) {
    while (1) {
        count_2++;
    };
}

// Simple example of scheduler with three threads
int main(void) {
    count_0 = 0;
    count_1 = 0;
    count_2 = 0;

    // Add threads to scheduler and start:
    os_kernel_init();
    os_kernel_add_thread(&task_0, &task_1, &task_2);
    os_kernel_launch(QUANTA);

    // Spin forever
    while (1) {};

    // Will never reach this:
    return 0;
}
