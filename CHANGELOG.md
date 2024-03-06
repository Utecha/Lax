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
- [0.5.9](#0.5.9)
- [0.6.0](#0.6.0)
- [0.6.5](#0.6.5)
- [0.7.0](#0.7.0)
- [0.7.4](#0.7.4)
- [0.7.5](#0.7.5)
- [0.8.0](#0.8.0)

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

## 0.5.9

The reason this section exists is because the last time I tried implementing the local variables, it completely broke global variables. At this point, everything seems to be working as it should be. However, I fear that may not be the case when I implement the last bit of the chapter that fixes a specific edge case with scoping. I have suspicions it was in that section that things broke. It's a minor change overall (in terms of amount of code), but it does fundamentally change how the VM reacts to scoping. Hence, that is why it is '.1' shy of '0.6.0'.

## 0.6.0

It turns out I was incorrect. It was an error I made somewhere else in the code that was causing the issues my first time attempting the implementation. I'll have to spend the time to investigate what that may have been. Local and global variables are now a thing and do resolve correctly, even when they share a common identifier ('a' in the global scope and 'a' in the local scope are 2 different variables.)

## 0.6.5

- Added the main 3 control-flow structures: if-else statements, while loops, and for loops.
- Several simple tests for the VM have been added. Each one specifically covers the operations added in each version. The while loop test, specifically, is the early implementation of the fibonacci sequence (also the fastest implementation of it!)

## 0.7.0
- Added the support backbone for functions. It has replaced or required an update to quite a bit of the code.

One major change is that the size of the VM stack has been increased 64x, as it now needs to hold functions. Functions run on call frames which are like snapshots of the functions.

Another thing you may notice (if you have debug on), is that there is a value ```<Script>``` that is firmly placed at index 0 on the VM's stack. That is a reference to the top-level "main" function of the code. The entire script is now considered to be a function in itself, and therefore the first slot in the stack is reserved for that top-level function.

I'm going ahead and calling this '0.7.0' and committing at this point as a potential backup point. There was a lot that changed and there is still a lot more to add to finish implementing functions as a whole.

## 0.7.4
Functions are fully implemented, complete with return statements.. However, there definitely is a bug somewhere. I attempted to test functions using a recursive fibonacci function and it went into an endless loop. Hence, that is why it is at '0.7.4' and not '0.7.5'. That, and the fact that I haven't added in the native functions yet either. I'd rather go back and figure out the bugs in user function declarations first. Alas, I am tired and must take a break at this point. '0.7.5' will be after the bug fix. I plan on taking liberty sooner rather than later when it comes to implementing native functions to the language. The main one that I care about will be a little tough, as it'll involve implementing variadic arguments into Lox somehow or another. Maybe not, though. I might be able to piggy-back off of C's printf.

## 0.7.5

Good news, everyone! It turns out I'm just a dummy dumb dumb and functions weren't actually broken *at all*. They work just fine, including that recursive fibonacci test, as you'll see I've re-updated it to that, also featuring the new native clock function! (Not really new to Lox but... new for me in this version of the project up to this point.)

Speaking of that recursive fibonacci test, the times I got were far more drastic for than that of the author, likely due to different hardware. For me, jlox took roughly 104 seconds to complete 40 cycles. clox++ completed the same exact script in about 12 seconds (rounding up, it was actually about 11.6)! That's a 12x boost in speed for such recursion! Keep in mind though, the more you add, it will still get EXPONENTIALLY slower. I found that the threshold began around 37 when it started slowing down by about 1.7x each time.

## 0.8.0
Closures have now been implemented. I added a few test programs to test out the functionality. In order, the results of each should be 'Outer', 'Outer', and 'Updated'.

This is the final point in the book before the addition of the Garbage Collector. The language up to this point is still pretty open in terms of memory allocation and deallocation. Not everything is handled completely. That will change with the next update.
