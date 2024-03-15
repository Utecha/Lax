#include <math.h>
#include <string.h>

#include "bcompiler.h"
#include "chunk.h"
#include "common.h"
#include "debug.h"

static void
resetStack(VM *vm)
{
    vm->stackTop = vm->stack;
}

VM *
initVM()
{
    VM *vm = (VM *)malloc(sizeof(VM));
    resetStack(vm);

    return vm;
}

void
freeVM(VM *vm)
{

}

/*
 * This is where the magic happens. This function holds
 * the logic for interpreting all of the Bytecode instructions.
*/
static InterpretResult
run(VM *vm)
{
#define READ_BYTE()     (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
#define BINARY_DBL(op)                                              \
    do {                                                            \
        double b = pop(vm);                                         \
        double a = pop(vm);                                         \
        push(vm, a op b);                                           \
    } while (false)
#define BINARY_INT(op)                                              \
    do {                                                            \
        int b = round((int)pop(vm));                                \
        int a = round((int)pop(vm));                                \
        push(vm, a op b);                                           \
    } while (false)
#define POW()                                                       \
    do {                                                            \
        int b = round((int)pop(vm));                                \
        int a = round((int)pop(vm));                                \
        push(vm, pow(a, b));                                        \
    } while (false)

    for (;;) {

        if (sizeof(vm) > STACK_MAX) {
            laxlog(ERROR, "Stack Overflow!");
            return INTERPRET_RUNTIME_ERROR;
        }

#ifdef DEBUG_TRACE_EXECUTION
        printf("        ");

        for (Value *slot = vm->stack; slot < vm->stackTop; slot++) {
            printf("[");
            printValue(*slot);
            printf("]");
        }
        printf("\n");

        disassembleInstruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
#endif // DEBUG_TRACE_EXECUTION

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(vm, constant);
            } break;
            case OP_ADD:        BINARY_DBL(+);                  break;
            case OP_SUBTRACT:   BINARY_DBL(-);                  break;
            case OP_MULTIPLY:   BINARY_DBL(*);                  break;
            case OP_DIVIDE:     BINARY_DBL(/);                  break;
            case OP_MODULUS:    BINARY_INT(%);                  break;
            case OP_POWER:      POW();                          break;
            case OP_BAND:       BINARY_INT(&);                  break;
            case OP_BOR:        BINARY_INT(|);                  break;
            case OP_BXOR:       BINARY_INT(^);                  break;
            case OP_SHL:        BINARY_INT(<<);                 break;
            case OP_SHR:        BINARY_INT(>>);                 break;
            case OP_NEGATE:     push(vm, -(*(--vm->stackTop))); break;
            // case OP_NEGATE:     push(vm, pop(vm)); break;
            case OP_RETURN: {
                // printValue(pop(vm));
                printValue(*(--vm->stackTop));
                printf("\n");
                return INTERPRET_OK;
            } break;
            default: laxlog(ERROR, "Undefined Opcode: %s", instruction);
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_DBL
#undef BINARY_INT
#undef POW
}

InterpretResult
interpret(VM *vm, const char *src)
{
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(src, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm->chunk = &chunk;
    vm->ip = vm->chunk->code;

    InterpretResult result = run(vm);

    freeChunk(&chunk);
    return result;
}

void
push(VM *vm, Value value)
{
    *vm->stackTop = value;
    vm->stackTop++;
}

Value
pop(VM *vm)
{
    vm->stackTop--;
    return *vm->stackTop;
}
