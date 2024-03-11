#include "common.h"
#include "read.h"

char *
readFile(const char *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        laxlog(ERROR, "Could not open file '%s'.", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char *buf = (char *)malloc(fileSize + 1);
    if (buf == NULL) {
        laxlog(ERROR, "Not enough memory to read '%s' && buy more RAM LOL!", path);
        exit(74);
    }

    size_t bytes = fread(buf, sizeof(char), fileSize, file);
    if (bytes < fileSize) {
        laxlog(ERROR, "Could not read file '%s'.", path);
    }

    buf[bytes] = '\0';
    fclose(file);
    return buf;
}
