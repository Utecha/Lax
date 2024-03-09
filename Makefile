CFLAGS := -Wall -Wextra -Wno-unused-parameter

OG_SOURCE_DIR := lox
SOURCE_DIR := src
OG_HEADERS := $(wildcard $(OG_SOURCE_DIR)/*.h)
OG_SOURCES := $(wildcard $(OG_SOURCE_DIR)/*.c)
HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
SOURCESBAK := $(wildcard $(SOURCE_BAK_DIR)/*.c)
OBJECTS := $(notdir $(SOURCES:.c=.o))

all: lax clox
dump: lax_dump clox_dump
dbg: lax_dbg clox_dbg

lax:
	@ mkdir -p build/release
	gcc $(CFLAGS) -O3 -o build/release/lax $(SOURCES)
	@ cp build/release/lax .

lax_dbg:
	@ mkdir -p build/release
	gcc $(CFLAGS) -DDEBUG -ggdb -O0 -o build/debug/laxdb $(SOURCES)
	@ cp build/release/lax .

lax_dump:
	@ mkdir -b build/objdump/lax

og: clox clox_dump

clox:
	@ mkdir -p build/release
	gcc $(CFLAGS) -O3 -o build/release/clox $(OG_SOURCES) -lm
	@ cp build/release/clox .

clox_dbg:
	@ mkdir -p build/debug
	gcc $(CFLAGS) -DDEBUG -ggdb -O0 -o build/debug/cloxdb $(OG_SOURCES) -lm
	@ cp build/debug/cloxdb .

clox_dump:
	@ mkdir -p build/objdump/clox
	gcc -o build/objdump/clox/clox.o -c src/clox.c
	gcc -o build/objdump/clox/clox_bcompiler.o -c src/clox_bcompiler.c
	gcc -o build/objdump/clox/clox_chunk.o -c src/clox_chunk.c
	gcc -o build/objdump/clox/clox_debug.o -c src/clox_debug.c
	gcc -o build/objdump/clox/clox_memory.o -c src/clox_memory.c
	gcc -o build/objdump/clox/clox_object.o -c src/clox_object.c
	gcc -o build/objdump/clox/clox_scanner.o -c src/clox_scanner.c
	gcc -o build/objdump/clox/clox_table.o -c src/clox_table.c
	gcc -o build/objdump/clox/clox_value.o -c src/clox_value.c
	gcc -o build/objdump/clox/clox_vm.o -c src/clox_vm.c
