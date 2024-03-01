# CHANGELOG

Each section is denoted by the version number it corresponds to. Below is a table of contents for quickly jumping between sections:

- [0.1.0](#0.1.0)
- [0.2.0](#0.2.0)

## 0.1.0
##### New Files
- clox.c -- main
- clox_common.h -- Commonly used macros/imports throughout project, most files will include this header
- clox_chunk.c && clox_chunk.h -- Bytecode chunk definition
- clox_debug.c && clox_debug.h -- Debug info generator
- clox_memory.c && clox_memory.h -- Main memory handling for the project. Home of the code that allows for the dynamic arrays.
- clox_value.c && clox_value.h -- Defines the 'Value' type for lox, as well as functions to handle value arrays and printing values.

## 0.2.0

##### New Files
- clox_vm.c && clox_vm.h -- The beating heart, the VM.

The VM is added here, but still at this point, the main function in 'clox.c' is still just hard-coded bytecode for testing. Operations that have been added are:
- OP_CONSTANT
- OP_ADD
- OP_SUBTRACT
- OP_MULTIPLY
- OP_DIVIDE
- OP_RETURN (acts as OP_PRINT and OP_EOF at the moment)
