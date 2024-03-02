CFLAGS := -Wall -Wextra -Wno-unused-parameter

SOURCE_DIR = := src
HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(notdir $(SOURCES:.c=.o))

default: debug

clox:
	@ mkdir -p build/release
	gcc $(CFLAGS) -o build/release/clox $(SOURCES) -lm
	@ cp build/release/clox .

debug:
	@ mkdir -p build/debug
	gcc $(CFLAGS) -DDEBUG -ggdb -o build/debug/cloxdb $(SOURCES) -lm
	@ cp build/debug/cloxdb .

obj_dump:
	@ mkdir -p build/objdump
	gcc -o build/objdump/clox.o -c src/clox.c
	gcc -o build/objdump/clox_bcompiler.o -c src/clox_bcompiler.c
	gcc -o build/objdump/clox_chunk.o -c src/clox_chunk.c
	gcc -o build/objdump/clox_debug.o -c src/clox_debug.c
	gcc -o build/objdump/clox_memory.o -c src/clox_memory.c
	gcc -o build/objdump/clox_scanner.o -c src/clox_scanner.c
	gcc -o build/objdump/clox_value.o -c src/clox_value.c
	gcc -o build/objdump/clox_vm.o -c src/clox_vm.c
