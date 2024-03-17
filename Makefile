CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wno-unused-parameter -lm
DBG_FLAGS := -DDEBUG -ggdb -O0
REL_FLAGS := -O3

SRCDIR = src
BUILDDIR = build

OBJDIR := $(SRCDIR)/obj
DBGDIR := $(BUILDDIR)/debug
RELDIR := $(BUILDDIR)/release

SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))

CLOX_SRCDIR = lox

CLOX_OBJDIR := $(CLOX_SRCDIR)/obj
CLOX_DBGDIR := $(BUILDDIR)/clox/debug
CLOX_RELDIR := $(BUILDDIR)/clox/release

CLOX_SRC := $(wildcard $(CLOX_SRCDIR)/*.c)
CLOX_OBJ := $(patsubst $(CLOX_SRCDIR)/%.c, $(CLOX_OBJDIR)/%.o, $(CLOX_SRC))

INSTALLDIR := /usr/local/bin

TARGET = lax
DBG_TARGET := $(DBGDIR)/$(TARGET)db
REL_TARGET := $(RELDIR)/$(TARGET)

CLOX_TARG = clox
CLOX_DBG_TARG := $(CLOX_DBGDIR)/$(CLOX_TARG)db
CLOX_REL_TARG := $(CLOX_RELDIR)/$(CLOX_TARG)

all: release clox_rel

release: $(REL_TARGET) | $(RELDIR)
	@ cp $(REL_TARGET) ./

clox_rel: $(CLOX_REL_TARG) | $(CLOX_RELDIR)
	@ cp $(CLOX_REL_TARG) ./

debug: $(DBG_TARGET) | $(DBGDIR)

clox_dbg: $(CLOX_DBG_TARG) | $(CLOX_DBGDIR)

install: release
	@ printf "Copying lax to [%s]\n" $(INSTALLDIR); \
	sudo cp $(REL_TARGET) $(INSTALLDIR) && \
	printf "\033[1;32mInstall Successful -- %s -> [%s/%s]\033[0m\n\n" $(REL_TARGET) $(INSTALLDIR) $(TARGET)

uninstall: clean
	@ printf "\033[1;33mRemoving\033[0m lax from [%s]\n" $(INSTALLDIR); \
	sudo rm $(INSTALLDIR)/$(TARGET) && \
	printf "\033[1;32mUNINSTALL SUCCESS\033[0m\n\n"

clean:
	@ echo "Cleaning lax..."; \
	rm -rf $(OBJDIR) $(BUILDDIR) $(CLOX_OBJDIR); \
	if [ -e $(TARGET) ]; then \
		rm $(TARGET); \
		rm $(CLOX_TARG); \
	fi; \
	echo "Cleaned lax successfully!"

$(DBG_TARGET): $(OBJ) | $(DBGDIR)
	$(CC) $(DBG_FLAGS) $(CFLAGS) $^ -o $@

$(REL_TARGET): $(OBJ) | $(RELDIR)
	$(CC) $(REL_FLAGS) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@ printf "%-8s: %-16s --> %s\n" "compiling" $< $@; \
	$(CC) $(CFLAGS) -c $< -o $@

$(CLOX_DBG_TARG): $(CLOX_OBJ) | $(CLOX_DBGDIR)
	$(CC) $(DBG_FLAGS) $(CFLAGS) $^ -o $@

$(CLOX_REL_TARG): $(CLOX_OBJ) | $(CLOX_RELDIR)
	$(CC) $(REL_FLAGS) $(CFLAGS) $^ -o $@

$(CLOX_OBJDIR)/%.o: $(CLOX_SRCDIR)/%.c | $(CLOX_OBJDIR)
	@ printf "%-8s: %-16s --> %s\n" "compiling" $< $@; \
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@ mkdir -p $(OBJDIR)

$(DBGDIR):
	@ mkdir -p $(DBGDIR)

$(RELDIR):
	@ mkdir -p $(RELDIR)

$(CLOX_OBJDIR):
	@ mkdir -p $(CLOX_OBJDIR)

$(CLOX_DBGDIR):
	@ mkdir -p $(CLOX_DBGDIR)

$(CLOX_RELDIR):
	@ mkdir -p $(CLOX_RELDIR)

$(BUILDDIR):
	@ mkdir -p $(BUILDDIR)

.PHONY: all clean release install uninstall clox_rel
.DEFAULT: all
