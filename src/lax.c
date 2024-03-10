#include "common.h"

static void
repl()
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
    }
}

int
main(int argc, char **argv)
{
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        // runFile(argv[1]);
    } else {
        laxlog(NONE, "Usage: lax <source>\n");
        exit(64);
    }

    return 0;
}
