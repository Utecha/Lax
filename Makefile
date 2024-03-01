CFLAGS := -Wall -Wextra -Wno-unused-parameter

SOURCE_DIR = := src
HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(notdir $(SOURCES:.c=.o))

clox:
	gcc -o clox $(SOURCES) -lm

debug:
	@ mkdir -p build/debug
	gcc $(CFLAGS) -DDEBUG -ggdb -o build/debug/clox $(SOURCES) -lm
