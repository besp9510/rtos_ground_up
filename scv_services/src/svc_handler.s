// SVC handler defined here and not in main because I am not
// using ARMCC but rather the ARM GCC compiler

// New instructions used here:
// 1. TST = Bitwise AND (set N and Z flags)
// 2. MRS = Special --> general register move

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.section .text, "x"

.type   SVC_Handler, %function          // Define symbol type for assembler so
.global SVC_Handler                     // it can be called

.global SVC_Handler_c                   // C handler defined in main.c

// ------------------------------------------------------------------------- //
// SVC handler
// - Responsible for delivering stack pointer to C handler; MSP or PSP
//   depending on context prior to SVC exception.
// Arguments
// - None
// Returns
// - R0: stack pointer (MSP or PSP)
// ------------------------------------------------------------------------- //
SVC_Handler:
            TST     LR, #0x4            // Test bit 3 (0x4 = 0100) to determine
                                        // MSP or PSP stack is being used:
                                        // MSP: x0xx & 0100 = 0000 => Z = 1
                                        // PSP: x1xx & 0100 = 0100 => Z = 0

            ITE     EQ                  // If MSP Else PSP (EQ: Z = 1 => MSP)
            MRSEQ   R0, MSP             // Copy correct stack pointer to pass
            MRSNE   R0, PSP             // the stack frame address to C handler

            LDR     R1, =SVC_Handler_c  // Return to C handler (instead of LR)

            BX      R1
