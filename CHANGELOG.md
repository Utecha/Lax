# CHANGELOG

Each section is denoted by the version number it corresponds to. Below is a table of contents for quickly jumping between sections:

- [0.1.0](#0.1.0)
- [0.2.0](#0.2.0)
- [0.3.0](#0.3.0)
- [0.3.5](#0.3.5)
- [0.4.0](#0.4.0)
- [0.4.5](#0.4.5)
- [0.5.0](#0.5.0)
- [0.5.5](#0.5.5)

## 0.1.0
#### New Files
- clox.c -- main
- clox_common.h -- Commonly used macros/imports throughout project, most files will include this header
- clox_chunk.c && clox_chunk.h -- Bytecode chunk definition
- clox_debug.c && clox_debug.h -- Debug info generator
- clox_memory.c && clox_memory.h -- Main memory handling for the project. Home of the code that allows for the dynamic arrays.
- clox_value.c && clox_value.h -- Defines the 'Value' type for lox, as well as functions to handle value arrays and printing values.

## 0.2.0

#### New Files
- clox_vm.c && clox_vm.h -- The beating heart, the VM.

The VM is added here, but still at this point, the main function in 'clox.c' is still just hard-coded bytecode for testing. Operations that have been added are:
- OP_CONSTANT
- OP_ADD
- OP_SUBTRACT
- OP_MULTIPLY
- OP_DIVIDE
- OP_RETURN (acts as OP_PRINT and OP_EOF at the moment)

## 0.3.0

#### New Files
- clox_bcompiler.c && clox_bcompiler.h -- Named 'bcompiler' as this is the source that compiles the users code into bytecode.
- clox_scanner.c && clox_scanner.h -- Named scanner just to fit with the book, but it's the lexer.

No new ops added at this point. However, 'clox.c' has now been modified to be able to read from a source file, as well as run a basic REPL in the event that no source file was provided.
```console
$ Usage: clox <source>*
```

## 0.3.5
No new files in this one. Just a lot of updating to the previous ones. The bytecode compiler and VM are now up to a point of successful compilation and interpretation of that compiled bytecode. Simple instructions such as '5 + 5' will actually resolve as they should at this point.

## 0.4.0

Again, no new files. However, lots of changes were made. Value is no longer just a typedef for 'double', but is now its own type altogether, which tracks the actual type being passed in. Currently it is a union that supports boolean and number values. As a result, several macros were made in order to type check and type cast the values.

The new additions in this version are: '!', '!=', '==', '<', '<=', '>', '>=', booleans ('true' and 'false' as literals), and nil ('nil' as a literal).

## 0.4.5
#### New Files
- clox_object.c && clox_object.h -- Defines a new "Obj" type that represents large objects that need to be stored on the heap, such as strings.

Apparently in the scanner I forgot to actually account for any 't' keywords, and as a result, forgot to add in 'TOKEN_TRUE'. As a result, entering 'true' in the repl or reading it from a file would throw a Lox error. That has been fixed as of this version. Everything should work now. Plus, you now have strings! And they can be concatenated! (Escape sequences coming later).

## 0.5.0

#### New Files

- clox_table.c && clox_table.h -- Hash table implementations in C. Objects, especially strings, are now run through the hash table to keep track of key/value pairs. Speeds of comparing strings have improved as they are now compared by their hash rather than character by character directly.

At the moment, the hash table only supports strings. I may go back in later and modify it to support the keys for the other primitive types: numbers, booleans, and nil.

## 0.5.5

Variables are now supported. Of course, there are no blocks or anything like that yet, so up to this point, it's just global variables. They're also not handled in the most efficient way, and re-referencing a variable can actually take up space in the stack, causing the user to run out of instructions sooner.
