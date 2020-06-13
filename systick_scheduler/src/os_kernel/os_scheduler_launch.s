// Launch static round robin scheduler

.type   os_scheduler_launch, %function  // Define symbol type for assembler
.global os_scheduler_launch             // so it can be called

.extern current_tcb                     // Defined in os_kernel.c

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.section .text, "x"

// Scheduler launch is responsible for
// 1. Restore context of task 0
//    - Pop R0 - R12
//    - Set SP botton of task 1 stack
//    - Point LR to task 0
// 3. Enable interrupts to allow SysTick exceptions
// 4. Branch to task 0
os_scheduler_launch:
            LDR     R0, =current_tcb     // Load address of current_tcb into R0
                                         // (R0 = &current_tcb)

            LDR     R2, [R0]             // Load contents of memory address
                                         // pointed to by R0 into R2.
                                         // (R2 = &current_tcb.stack_pointer)

            LDR     SP, [R2]             // Load contents of memory address
                                         // pointed to by R2 into the SP.
                                         // (SP = current_tcb->stack_pointer)

            POP     {R4-R11}             // Restore context from thread's in
            POP     {R0-R3}              // order they are saved: stack frame
            POP     {R12}                // then R4 - R11.

            ADD     SP, SP, #4           // Skip over SP's spot in the stack

            POP     {LR}                 // Load LR with pointer to thread

            ADD     SP, SP, #4           // Stack pointer + 4 so it's ready
                                         // for context switch to push
                                         // exception stack frame

            CPSIE   I                    // Enable interrupts to allow Systick

            BX      LR                   // Branch to task 0