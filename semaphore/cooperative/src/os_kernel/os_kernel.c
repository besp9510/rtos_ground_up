// SysTick Round Robin Scheduler
#include "os_kernel.h"

// A fixed amount of threads are supported with static sized array
#define TOTAL_THREAD 3
#define STACK_SIZE   100

// Default bus clock in Hz
#define BUS_CLOCK 16e6

// Interrupt control and state register
// - 0xE000ED04 address of register
// - (volatile uint32_t *) cast as unsigned 32-bit pointer to address
// - (*((volatile uint32_t *)0xE000ED04)) dereference pointer to write a value
//   to the register
#define INTCTL (*((volatile uint32_t *)0xE000ED04))

// Thread control block with bare minimum information:
struct tcb_def {
    int32_t *stack_pointer;
    struct tcb_def *next_tcb;
};

// Array of all TCBs; one for each thread:
struct tcb_def tcbs[TOTAL_THREAD];
struct tcb_def *current_tcb;

// Stack array where all threads will work out of:
int32_t thread_stack[TOTAL_THREAD][STACK_SIZE];

extern void os_scheduler_launch(void);

// Initialize a thread's stack by
// 1. Set stack pointer to allow for initial contex switch
// 2. Set value used for xPSR
void os_kernel_stack_init(int thread_number) {
    // The initial stack pointer will point to the address 16*4 = 64 bytes from
    // the top. This memory region skipped over is where R0-11, SP, R12, LR, PC
    // , and xPSR are stored for the first iteration. 100 - 16 = 84 index will
    // be pointed to inside the TCB for thread's private stack.
    //
    // Memory view:
    //              [84] TCB stack pointer
    //              [..] R0-11 and SP
    //              [97] LR (Empty at OS launch; populated on first switch)
    //              [98] PC (next instruction in thread)
    //              [99] xPSR
    tcbs[thread_number].stack_pointer = \
        &thread_stack[thread_number][STACK_SIZE - 16];

    // The top of stack (largest address) value is where the xPSR is stored.
    // 0x01000000 = 00000001 00000000 00000000 00000000 which means the T-bit
    // of the Execution PSR does not cause a hard fault. This is a safe value
    // that we will use to identify visually where the stack lies in memory.
    thread_stack[thread_number][STACK_SIZE - 1] = 0x01000000;
}

// Create TCBs for all three threads and initialize their stack.
// Note: void(*task_0) (void) means declare task_0 as pointer to function
// (void) returning void. This is how pointers to functions are used as input
// arguments
uint8_t os_kernel_add_thread(void(*task_0) (void), void(*task_1) (void),
                             void(*task_2) (void)) {
    // This is a critical code section that cannot be interrupted. Interrupts
    // are then disabled while the TCB linked list is created.
    __disable_irq();

    // Circular linked list is required; this is done by linking
    // TCB 0 --> 1, TCB 1 --> 2, TCB 2 --> 0
    tcbs[0].next_tcb = &tcbs[1];
    tcbs[1].next_tcb = &tcbs[2];
    tcbs[2].next_tcb = &tcbs[0];

    // Initialize the stack for a thread and copy the function pointer to
    // address that will be copied to the PC. This is so that the thread
    // will begin executing when it is first scheduled to execute.
    os_kernel_stack_init(0);
    thread_stack[0][STACK_SIZE - 2] = (int32_t)(task_0); // Type cast from void
                                                         // to numerical addr.

    os_kernel_stack_init(1);
    thread_stack[1][STACK_SIZE - 2] = (int32_t)(task_1);

    os_kernel_stack_init(2);
    thread_stack[2][STACK_SIZE - 2] = (int32_t)(task_2);

    // Initially points to the first thread:
    current_tcb = &tcbs[0];

    // Critical section is now done so interrupts can be enabled again:
    __enable_irq();

    return 1;
}

void os_kernel_init(void) {
    __disable_irq();
}

// set up systick
void os_kernel_launch(uint32_t quanta) {
    // Disable SysTick for the time being while its setup:
    SysTick->CTRL = 0x0;

    // Clear the current counter and count flag to zero. This causes the
    // counter to reload on the next timer clock but does not trigger an
    // interrupt. The value reloaded will be the value stored in LOAD.
    SysTick->VAL = 0x0;

    // Set SysTick (IRQn = 15) to lowest priority (7). This makes it so SysTick
    // only suspends foreground threads and not other interrupts.
    NVIC_SetPriority(15, 7);

    // Set reload counter to quanta [ms] so that when SysTick is enabled, it
    // will begin it's count down to zero at the quanta. Reload value is
    // calculated the taking the desired number of processor clock cycles - 1
    SysTick->LOAD = ((quanta / 1e3) * BUS_CLOCK - 1) ; // [s] * [1/s] = cycles

    // Systick is now configured and ready to be started. Set specific bits:
    // Bit 0: 1 = Indicate processor clock, 0 = Indicate external clock
    // Bit 1: 1 = Enable SysTick enable request (zero = exception request),
    //        0 = Disable Systick enable request
    // Bit 2: 1 = Counter enabled, 0 = Counter disabled
    SysTick->CTRL = (0x1) | (0x2) | (0x4); // 001 | 010 | 100 = 0000 0111

    // This will start the scheduler:
    os_scheduler_launch();
}

// Thread yield to scheduler
void os_thread_yield(void) {
    // Reset SysTick counter before triggering an interrupt to ensure quanta
    // is respected for next thread:
    SysTick->VAL = 0;

    // Trigger SysTick interrupt and allow thread to yield processor to the
    // scheduler:
    // Bit 26: 1 = changes SysTick exception state to pending
    SCB->ICSR = 0x4000000;
}

// Initialize a semaphore and provide it a number of tokens:
void os_semaphore_init(int32_t *semaphore, int32_t value) {
    *semaphore = value;
}

// Set a semaphore
void os_signal_set(int32_t *semaphore) {
    // Atomic operation required for semaphore set:
    __disable_irq();

    // This sets the semaphore:
    *semaphore += 1;

    __enable_irq();
}

// Wait for a semaphore
void os_signal_wait(int32_t *semaphore) {
    // Atomic operation required for semaphore read:
    __disable_irq();

    // Spin-lock while waiting for semaphore:
    while (*semaphore <= 0) {
        // Allow SysTick interrupts while waiting:
        __enable_irq();

        // "Cooperate" (stop spinning and give up processor to scheduler):
        // - SysTick ISR disables and enables interrupts on it's own
        os_thread_yield();

        __disable_irq();
    }

    // This releases the semaphore:
    *semaphore -= 1;

    __enable_irq();
}