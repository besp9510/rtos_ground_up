// SCV service to perform an operation on two input numbers and return the
// result to a calling "application task" by levarging a handler in asm and C

static int svc_service_add(int x, int y) {
    // Tell compiler to keep result stored in a processor register instead of
    // memory. Specifcally, use R0 which is one of the two resulting passing
    // registers.
    register int result __asm__ ("r0");

    // According to ARM Architecture Procedure Core Standard (AAPCS), R0, R1,
    // R2, and R3 are designated the parameter passing registers. R0 and
    // R1 are return registers.
    __asm__ (
             "MOV %%R0, %1\n"
             "MOV %%R1, %2\n"
             "SVC 0x00\n"
             : "=r" (result)
             : "r" (x), "r" (y)
             :
    );

    return result;
}

static int svc_service_sub(int x, int y) {
    register int result __asm__ ("r0");

    __asm__ (
             "MOV %%R0, %1\n"
             "MOV %%R1, %2\n"
             "SVC 0x01\n"
             : "=r" (result)
             : "r" (x), "r" (y)
             :
    );

    return result;
}

static int svc_service_mul(int x, int y) {
    register int result __asm__ ("r0");

    __asm__ (
             "MOV %%R0, %1\n"
             "MOV %%R1, %2\n"
             "SVC 0x02\n"
             : "=r" (result)
             : "r" (x), "r" (y)
             :
    );

    return result;
}

static int svc_service_div(int x, int y) {
    register int result __asm__ ("r0");

    __asm__ (
             "MOV %%R0, %1\n"
             "MOV %%R1, %2\n"
             "SVC 0x03\n"
             : "=r" (result)
             : "r" (x), "r" (y)
             :
    );

    return result;
}

// Test various SVC services
int main() {
    // Definitions and initializations:
    int x;
    int y;
    int z;

    x = 12;
    y = 6;

    // Test each SVC service to perform operations:
    z = svc_service_add(x, y);
    z = svc_service_sub(x, y);
    z = svc_service_mul(x, y);
    z = svc_service_div(x, y);

    return 0;
}

// C handler responsible for handling SCV exception and executing associated
// service for a given SCV number. Assembly SCV handler returns here with
// stack pointer.
void SVC_Handler_c(unsigned int *svc_args) {
    // The exception stack frame is pushed onto the stack and contains:
    //  SP (svc_args) --> [0] R0
    //                    [1] R1
    //                    [2] R2
    //                    [3] R3
    //                    [4] R12
    //                    [5] R14 (LR)
    //                    [6] R15 (PC)
    //                    [7] xPSR
    // SVC args is the stack pointer (MSP or PSP) and points to the top of 
    // the stack frame (stack is in descending order)

    // To find the SVC number, we need to find the instruction and access the
    // byte where the SCV number if contained. We do this by getting the
    // program counter address and transversing two bytes back. The PC
    // points to the next instruction after the SCV instruction that caused
    // the exception. 2 bytes from the PC as the SCV instruction is constructed
    // in Thumb state as:
    // SCV           0xN
    // [0 --> 2]     [2 -- > 4]
    // (instruction) (operand)
    unsigned int svc_number = ((char*)svc_args[6])[-2];

    int x = svc_args[0];
    int y = svc_args[1];

    // According to AAPCS, if a function returns a value, then that return will
    // be stored in R0 and R1. Since we are only returning one value, place
    // result in R0.
    switch (svc_number) {
        // SVC 0x0 = Addition
        case 0x0:
            svc_args[0] = x + y;
            break;
        // SVC 0x1 = Subtraction
        case 0x1:
            svc_args[0] = x - y;
            break;
        // SVC 0x2 = Multiplication
        case 0x2:
            svc_args[0] = x * y;
            break;
        // SVC 0x3 = Division
        case 0x3:
            svc_args[0] = x / y;
            break;
    }
}
