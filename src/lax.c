#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "read.h"
#include "vm.h"

// Very simple REPL for interpreting code.
// Not super useful for testing functions or classes in its current state.
static void
repl(VM *vm)
{
    char line[1024];
    for (;;) {
        printf(">>> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        if (!strncmp(line, "exit", 4)) {
            break;
        }

        interpret(vm, line);
    }
}

static void
runFile(VM *vm, const char *path)
{
    char *src = readFile(path);
    InterpretResult result = interpret(vm, src);
    free(src);

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

/* Start her up! */
int
main(int argc, char **argv)
{
    VM vm;
    initVM(&vm);

    if (argc == 1) {
        repl(&vm);
    } else if (argc == 2) {
        runFile(&vm, argv[1]);
    } else {
        laxlog(INFO, "Usage: %s <source>", argv[0]);
        laxlog(ERROR, "Lax currently can only run 1 source file.");
        exit(64);
    }

    freeVM(&vm);
    return 0;
}
