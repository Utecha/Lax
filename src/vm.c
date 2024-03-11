#include <math.h>

#include "bcompiler.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

static void
resetStack(VM *vm)
{
    vm->stackTop = vm->stack;
}

void
initVM(VM *vm)
{
    resetStack(vm);
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
#define BINARY(op)                                                  \
    do {                                                            \
        double b = pop(vm);                                         \
        double a = pop(vm);                                         \
        push(vm, a op b);                                           \
    } while (false)
#define MOD()                                                       \
    do {                                                            \
        int b = round((int)pop(vm));                                \
        int a = round((int)pop(vm));                                \
        push(vm, a % b);                                            \
    } while (false)
#define POW()                                                       \
    do {                                                            \
        int b = round((int)pop(vm));                                \
        int a = round((int)pop(vm));                                \
        push(vm, pow(a, b));                                        \
    } while (false)
#define SHIFT(op)                                                   \
    do {                                                            \
        int b = round((int)pop(vm));                                \
        int a = round((int)pop(vm));                                \
        push(vm, a op b);                                           \
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
            case OP_ADD:        BINARY(+); break;
            case OP_SUBTRACT:   BINARY(-); break;
            case OP_MULTIPLY:   BINARY(*); break;
            case OP_DIVIDE:     BINARY(/); break;
            case OP_MODULO:     MOD(); break;
            case OP_POWER:      POW(); break;
            case OP_SHL:        SHIFT(<<); break;
            case OP_SHR:        SHIFT(>>); break;
            case OP_NEGATE:     push(vm, -(*(--vm->stackTop))); break;
            // case OP_NEGATE:     push(vm, pop(vm)); break;
            case OP_RETURN: {
                // printValue(pop(vm));
                printValue(*(--vm->stackTop));
                printf("\n");
                return INTERPRET_OK;
            } break;
            default: laxlog(ERROR, "Undefined OpCode: %s", instruction);
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY
#undef MOD
#undef POW
#undef SHIFT
}

InterpretResult
interpret(VM *vm, const char *src)
{
    compile(vm, src);
    return INTERPRET_OK;
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
