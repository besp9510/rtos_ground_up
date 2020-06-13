// SysTick Handler acting as a static round robin scheduler

// New instructions used
// CPSID = Disable interrupts and configurable fault handlers (set PRIMASK)
// PUSH  = Push registers onto a full-descending stack (uses the value in the
//         SP register minus four as the highest memory address). Alias to
//         STR Rd, [SP, #-4]!
// POP   = Pop registers off a full-descending stack (uses the value in the
//         SP register as the lowest memory address) then increment SP register
//         by 4. Alias to LDR Rd, [SP], #4

// Important things to know:
// LDR Rd,=label        : load address of label into register Rd
// LDR Rd,=const/#const : Load constant (number) into Rd
// LDR Rd,[Rn]          : Load contents of address pointed to by Rn into Rd
// STR Rd,[Rn]          : Store contents of Rd into address pointed to by Rn

// Addressing types of LDR and STR:
// [Rn, #offset]  : Offset addressing. Add/subtrack from address using Rn
//                  leaving Rn unaltered. Result is used as the address.
// [Rn, #offset]! : Pre-indexed addressing. Add/subtrack from address using Rn
//                  and write result back to Rn. Results it used as the addr.
// [Rn], #offset  : Post-indexed addressing. Use address from Rn and then
//                  add/subtrack and write result back to Rn.

.extern current_tcb                     // Defined in os_kernel.c

.type   SysTick_Handler, %function
.global SysTick_Handler

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.section .text, "x"

// Upon SysTick exception, the exception stack frame is push onto the stack.
// This frame consists of R0, R1, R2, R3, R12, LR, PC, and PSR.
//
// However, to get the full context prior to exception when a thread is
// being suspended, R4-R11 must be saved manually. Our SysTick handler will
// be responsible for doing this by pushing R4 to R11 on the stack of the
// current thread and popping from the thread's stack.
//
// Another responsibility will be to update the current's thread stack
// pointer and the pointer to the next TCB. Once the next TCB has been updated,
// the next (now current) TCB stack pointer can be loaded. Once loaded,
// R4 to R11 is manually popped from the stack leaving exception return to
// pop R0, R1, R2, R3, R12, LR, PC, and PSR.
SysTick_Handler:
            CPSID   I                   // Disable interrupts as this is a
                                        // critical code section

            PUSH    {R4-R11}            // Save R4-11 by pushing onto stack
                                        // as they are not in exception stack
                                        // frame

            LDR     R0, =current_tcb    // Load address of *current_tcb
                                        // (R0 = &current_tcb)

            LDR     R1, [R0]            // Load contents of address pointed to
                                        // by R0 into R1 which will be the
                                        // address of SP
                                        // (R1 = &current_tcb.stack_pointer)

            STR     SP, [R1]            // Store SP register in address pointed
                                        // to by R1. Address pointed to by R1
                                        // is the current_tcb.stack_pointer
                                        // (the SP has changed since
                                        // the thread started executing)
                                        // (current_tcb->stack_pointer = SP)

            LDR     R1, [R1, #4]        // Load contents of address pointed to
                                        // by R1 + 4 bytes into R1. This loads
                                        // current_tcb->next_tcb into memory
                                        // (int32_t = 4 bytes= .next_tcb size)
                                        // (R1 = current_tcb->next_tcb)

            STR     R1, [R0]            // Store contents of R1 into the
                                        // address pointed to by R0. R0
                                        // holds &current_tcb meaning
                                        // we are scheduling the next thread
                                        // to execute by updating *current_tcb.
                                        // (current_tcb = current_tcb->next_tcb)

            LDR     SP, [R1]            // Load contents of address pointed to
                                        // in R1 into SP. R1 currently points
                                        // to current_tcb.next_tcb which is
                                        // the address of the next TCB. The
                                        // first element is stack_pointer
                                        // (SP = current_tcb->next_tcb)
                                        // (SP = next_tcb.stack_pointer)

            POP     {R4-R11}            // Restore R4 - R11 registers manaully
                                        // from next task's stack as they
                                        // are not in the exception stack frame

            CPSIE   I                   // Enable interrupts as critical
                                        // code is done and SysTick is needed

            BX      LR                   // Branch to next task

                                         // At this point, exception stack
                                         // frame is popped from the stack