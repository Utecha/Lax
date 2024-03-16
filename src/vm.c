#include <math.h>
#include <stdarg.h>
#include <string.h>

#include "bcompiler.h"
#include "debug.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

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

static Value
peek(VM *vm, int distance)
{
    return vm->stackTop[-1 - distance];
}

static bool
isFalsey(Value value)
{
    return IS_NULL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void
concatenate(VM *vm)
{
    ObjString *b = AS_STRING(pop(vm));
    ObjString *a = AS_STRING(pop(vm));

    int length = a->length + b->length;
    char *chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString *result = takeString(vm, chars, length);
    push(vm, OBJ_VAL(result));
}

static void
resetStack(VM *vm)
{
    vm->stackTop = vm->stack;
}

static void
runtimeError(VM *vm, const char *fmt, ...)
{
    size_t instruction = vm->ip - vm->chunk->code - 1;
    int line = vm->chunk->lines[instruction];
    laxlog(ERROR, "on [Ln %d] in 'Script'", line);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputs("\n", stderr);

    resetStack(vm);
}

VM *
initVM()
{
    VM *vm = (VM *)malloc(sizeof(VM));
    resetStack(vm);
    vm->objects = NULL;
    initTable(&vm->globals);
    initTable(&vm->strings);

    return vm;
}

void
freeVM(VM *vm)
{
    freeTable(&vm->strings);
    freeObjects(vm);
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
#define READ_STRING()   AS_STRING(READ_CONSTANT())
#define READ_SHORT()                                                \
    (vm->ip += 2, (uint16_t)((vm->ip[-2] << 8) | vm->ip[-1]))
#define BINARY_DBL(valueType, op)                                   \
    do {                                                            \
        if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {   \
            runtimeError(vm, "Operands must be numbers.");          \
            return INTERPRET_RUNTIME_ERROR;                         \
        }                                                           \
        double b = AS_NUMBER(pop(vm));                              \
        double a = AS_NUMBER(pop(vm));                              \
        push(vm, valueType(a op b));                                \
    } while (false)
#define BINARY_INT(valueType, op)                                   \
    do {                                                            \
        if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {   \
            runtimeError(vm, "Operands must be numbers.");          \
            return INTERPRET_RUNTIME_ERROR;                         \
        }                                                           \
        int b = round((int)AS_NUMBER(pop(vm)));                     \
        int a = round((int)AS_NUMBER(pop(vm)));                     \
        push(vm, valueType(a op b));                                \
    } while (false)
#define POW(valueType)                                              \
    do {                                                            \
        if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {   \
            runtimeError(vm, "Operands must be numbers.");          \
            return INTERPRET_RUNTIME_ERROR;                         \
        }                                                           \
        int b = round((int)AS_NUMBER(pop(vm)));                     \
        int a = round((int)AS_NUMBER(pop(vm)));                     \
        push(vm, valueType(pow(a, b)));                             \
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
            case OP_NULL:       push(vm, NULL_VAL);         break;
            case OP_TRUE:       push(vm, BOOL_VAL(true));   break;
            case OP_FALSE:      push(vm, BOOL_VAL(false));  break;
            case OP_POP:        pop(vm);                    break;
            case OP_GET_LOCAL: {
                uint8_t slot = READ_BYTE();
                push(vm, vm->stack[slot]);
            } break;
            case OP_SET_LOCAL: {
                uint8_t slot = READ_BYTE();
                vm->stack[slot] = peek(vm, 0);
            } break;
            case OP_GET_GLOBAL: {
                ObjString *name = READ_STRING();
                Value value;
                if (!tableGet(&vm->globals, name, &value)) {
                    runtimeError(vm, "Undefined variable '%s'.", name->chars);
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(vm, value);
            } break;
            case OP_SET_GLOBAL: {
                ObjString *name = READ_STRING();
                if (tableSet(&vm->globals, name, peek(vm, 0))) {
                    tableDelete(&vm->globals, name);
                    runtimeError(vm, "Undefined variable '%s'.", name->chars);
                    return INTERPRET_RUNTIME_ERROR;
                }
            } break;
            case OP_DEFINE_GLOBAL: {
                ObjString *name = READ_STRING();
                tableSet(&vm->globals, name, peek(vm, 0));
                pop(vm);
            } break;
            case OP_EQUAL: {
                Value b = pop(vm);
                Value a = pop(vm);
                push(vm, BOOL_VAL(valuesEqual(a, b)));
            } break;
            case OP_GREATER:    BINARY_DBL(BOOL_VAL, >);    break;
            case OP_LESS:       BINARY_DBL(BOOL_VAL, <);    break;
            case OP_ADD: {
                if (IS_STRING(peek(vm, 0)) && IS_STRING(peek(vm, 1))) {
                    concatenate(vm);
                } else if (IS_NUMBER(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
                    double b = AS_NUMBER(pop(vm));
                    double a = AS_NUMBER(pop(vm));
                    push(vm, NUMBER_VAL(a + b));
                } else {
                    runtimeError(vm, "Operands must be two numbers or two strings.");
                    return INTERPRET_RUNTIME_ERROR;
                }
            } break;
            case OP_SUBTRACT:   BINARY_DBL(NUMBER_VAL, -);  break;
            case OP_MULTIPLY:   BINARY_DBL(NUMBER_VAL, *);  break;
            case OP_DIVIDE:     BINARY_DBL(NUMBER_VAL, /);  break;
            case OP_MODULUS:    BINARY_INT(NUMBER_VAL, %);  break;
            case OP_POWER:      POW(NUMBER_VAL);            break;
            case OP_NOT: {
                push(vm, BOOL_VAL(isFalsey(*(--vm->stackTop))));
            } break;
            case OP_BAND:       BINARY_INT(NUMBER_VAL, &);  break;
            case OP_BOR:        BINARY_INT(NUMBER_VAL, |);  break;
            case OP_BXOR:       BINARY_INT(NUMBER_VAL, ^);  break;
            case OP_SHL:        BINARY_INT(NUMBER_VAL, <<); break;
            case OP_SHR:        BINARY_INT(NUMBER_VAL, >>); break;
            case OP_NEGATE: {
                if (!IS_NUMBER(peek(vm, 0))) {
                    runtimeError(vm, "Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(vm, NUMBER_VAL(-AS_NUMBER(*(--vm->stackTop))));
            } break;
            case OP_ECHO: {
                printValue(*(--vm->stackTop));
                printf("\n");
            } break;
            case OP_JUMP: {
                uint16_t offset = READ_SHORT();
                vm->ip += offset;
            } break;
            case OP_JUMP_FALSE: {
                uint16_t offset = READ_SHORT();
                if (isFalsey(peek(vm, 0))) vm->ip += offset;
            } break;
            case OP_LOOP: {
                uint16_t offset = READ_SHORT();
                vm->ip -= offset;
            } break;
            case OP_RETURN: {
                return INTERPRET_OK;
            } break;
            default: laxlog(ERROR, "Undefined Opcode: %s", instruction);
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_STRING
#undef READ_SHORT
#undef BINARY_DBL
#undef BINARY_INT
#undef POW
}

InterpretResult
interpret(VM *vm, const char *src)
{
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(vm, src, &chunk)) {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm->chunk = &chunk;
    vm->ip = vm->chunk->code;

    InterpretResult result = run(vm);

    freeChunk(&chunk);
    return result;
}
