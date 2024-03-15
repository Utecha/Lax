CFLAGS := -Wall -Wextra -Wno-unused-parameter -std=c99
DBGFLAGS := -DDEBUG -ggdb

CLOX_DIR := lox
CLOX := clox
CLOXDB := cloxdb
LAX_DIR := src
LAX := lax
LAXDB := laxdb
CLOX_HEADERS := $(wildcard $(CLOX_DIR)/*.h)
CLOX_SOURCES := $(wildcard $(CLOX_DIR)/*.c)
HEADERS := $(wildcard $(LAX_DIR)/*.h)
SOURCES := $(wildcard $(LAX_DIR)/*.c)
RELEASE := build/release
DEBUG := build/debug
CLOX_OBJDUMP := build/objdump/clox
LAX_OBJDUMP := build/objdump/lax

default: lax laxdbg

both: clox lax

bothdbg: cloxdbg laxdbg

lax: laxpch
	@ mkdir -p $(RELEASE)
	gcc $(CFLAGS) -O3 -o $(RELEASE)/$(LAX) $(SOURCES) -lm
	@ cp $(RELEASE)/$(LAX) .

laxdbg: laxpch
	@ mkdir -p $(DEBUG)
	gcc $(CFLAGS) -DDEBUG -ggdb -O0 -o $(DEBUG)/$(LAXDB) $(SOURCES) -lm

laxdump:
	@ mkdir -p $(LAX_OBJDUMP)
	gcc -o $(LAX_OBJDUMP)/$(LAX).o -c $(LAX_DIR)/$(LAX).c
	gcc -o $(LAX_OBJDUMP)/bcompiler.o -c $(LAX_DIR)/bcompiler.c
	gcc -o $(LAX_OBJDUMP)/chunk.o -c $(LAX_DIR)/chunk.c
	gcc -o $(LAX_OBJDUMP)/debug.o -c $(LAX_DIR)/debug.c
	gcc -o $(LAX_OBJDUMP)/lexer.o -c $(LAX_DIR)/lexer.c
	gcc -o $(LAX_OBJDUMP)/log.o -c $(LAX_DIR)/log.c
	gcc -o $(LAX_OBJDUMP)/memory.o -c $(LAX_DIR)/memory.c
	gcc -o $(LAX_OBJDUMP)/object.o -c $(LAX_DIR)/object.c
	gcc -o $(LAX_OBJDUMP)/parser.o -c $(LAX_DIR)/parser.c
	gcc -o $(LAX_OBJDUMP)/read.o -c $(LAX_DIR)/read.c
	gcc -o $(LAX_OBJDUMP)/table.o -c $(LAX_DIR)/table.c
	gcc -o $(LAX_OBJDUMP)/value.o -c $(LAX_DIR)/value.c
	gcc -o $(LAX_OBJDUMP)/vm.o -c $(LAX_DIR)/vm.c

laxpch:
	@ gcc $(SOURCES) $(HEADERS) -lm
	@ rm a.out

clox: cloxpch
	@ mkdir -p $(RELEASE)
	gcc $(CFLAGS) -O3 -o $(RELEASE)/$(CLOX) $(CLOX_SOURCES) -lm
	@ cp $(RELEASE)/$(CLOX) .

cloxdbg: cloxpch
	@ mkdir -p $(DEBUG)
	gcc $(CFLAGS) $(DBGFLAGS) -O0 -o $(DEBUG)/$(CLOXDB) $(CLOX_SOURCES) -lm

cloxpch:
	@ gcc $(CLOX_SOURCES) $(CLOX_HEADERS) -lm
	@ rm a.out

cloxdump:
	@ mkdir -p $(CLOX_OBJDUMP)
	gcc -o $(CLOX_OBJDUMP)/$(CLOX).o -c $(CLOX_DIR)/$(CLOX).c
	gcc -o $(CLOX_OBJDUMP)/$(CLOX)_bcompiler.o -c $(CLOX_DIR)/$(CLOX)_bcompiler.c
	gcc -o $(CLOX_OBJDUMP)/$(CLOX)_chunk.o -c $(CLOX_DIR)/$(CLOX)_chunk.c
	gcc -o $(CLOX_OBJDUMP)/$(CLOX)_debug.o -c $(CLOX_DIR)/$(CLOX)_debug.c
	gcc -o $(CLOX_OBJDUMP)/$(CLOX)_memory.o -c $(CLOX_DIR)/$(CLOX)_memory.c
	gcc -o $(CLOX_OBJDUMP)/$(CLOX)_object.o -c $(CLOX_DIR)/$(CLOX)_object.c
	gcc -o $(CLOX_OBJDUMP)/$(CLOX)_scanner.o -c $(CLOX_DIR)/$(CLOX)_scanner.c
	gcc -o $(CLOX_OBJDUMP)/$(CLOX)_table.o -c $(CLOX_DIR)/$(CLOX)_table.c
	gcc -o $(CLOX_OBJDUMP)/$(CLOX)_value.o -c $(CLOX_DIR)/$(CLOX)_value.c
	gcc -o $(CLOX_OBJDUMP)/$(CLOX)_vm.o -c $(CLOX_DIR)/$(CLOX)_vm.c

clean:
	@ rm -f $(RELEASE)/*
	@ rm -f $(DEBUG)/*
	@ rm -f $(CLOX_OBJDUMP)/*
	# @ rm -f $(LAX_OBJDUMP)/*
	@ rm $(CLOX)
	# @ rm $(LAX)
