# CHANGELOG

Each section is denoted by the version number it corresponds to. Below is a table of contents for quickly jumping between sections:

- [CLOX](#clox)
- [LAX](#lax)

## CLOX

- [clox 0.1.0](#clox-010) | The Beginning
- [clox 0.2.0](#clox-020) | Introducing the VM and basic instructions
- [clox 0.3.0](#clox-030) | Lexer (Scanner) and Bytecode Compiler added
- [clox 0.3.5](#clox-035) | bCompiler && VM hooked up and interpreting simple expressions
- [clox 0.4.0](#clox-040) | Initial introduction of dynamic typing
- [clox 0.4.5](#clox-045) | Introduced new generic "Obj" type for heap-allocated code
- [clox 0.5.0](#clox-050) | Implemented Hash Table for better managing values such as strings
- [clox 0.5.5](#clox-055) | Implemented global variables and the essence of global scoping
- [clox 0.5.9](#clox-059) | Nearly finished implementing local scopes (read the section)
- [clox 0.6.0](#clox-060) | Finished implementing local scopes (read the section)
- [clox 0.6.5](#clox-065) | Added the control-flow constructs: if-else statements, while, and for loops
- [clox 0.7.0](#clox-070) | Begin function implementation. File is itself now an implicit 'main' function.
- [clox 0.7.4](#clox-074) | Functions fully implemented (this version exists because I thought they were broken, see 0.7.5)
- [clox 0.7.5](#clox-075) | Confirmed functions are implemented properly
- [clox 0.8.0](#clox-080) | Implemented closures for functions
- [clox 0.9.0](#clox-090) | Implemented the Garbage Collector
- [clox 0.9.25](#clox-0925) | Implemented the base for classes and instances of classes
- [clox 0.9.75](#clox-0975) | Classes and methods fully implemented, minus inheritance.
- [clox 0.10.0](#clox-0100) | Inheritance added. Language is now complete (up to the original Lox spec)
- [clox 0.10.1](#clox-0101) | Added Modulo, Power, Increment/Decrement operators. Begin split into Lax.

### clox 0.1.0
#### New Files
- clox.c -- main
- clox_common.h -- Commonly used macros/imports throughout project, most files will include this header
- clox_chunk.c && clox_chunk.h -- Bytecode chunk definition
- clox_debug.c && clox_debug.h -- Debug info generator
- clox_memory.c && clox_memory.h -- Main memory handling for the project. Home of the code that allows for the dynamic arrays.
- clox_value.c && clox_value.h -- Defines the 'Value' type for lox, as well as functions to handle value arrays and printing values.

### clox 0.2.0

#### New Files
- clox_vm.c && clox_vm.h -- The beating heart, the VM.

The VM is added here, but still at this point, the main function in 'clox.c' is still just hard-coded bytecode for testing. Operations that have been added are:
- OP_CONSTANT
- OP_ADD
- OP_SUBTRACT
- OP_MULTIPLY
- OP_DIVIDE
- OP_RETURN (acts as OP_PRINT and OP_EOF at the moment)

### clox 0.3.0

#### New Files
- clox_bcompiler.c && clox_bcompiler.h -- Named 'bcompiler' as this is the source that compiles the users code into bytecode.
- clox_scanner.c && clox_scanner.h -- Named scanner just to fit with the book, but it's the lexer.

No new ops added at this point. However, 'clox.c' has now been modified to be able to read from a source file, as well as run a basic REPL in the event that no source file was provided.
```console
$ Usage: clox <source>*
```

### clox 0.3.5
No new files in this one. Just a lot of updating to the previous ones. The bytecode compiler and VM are now up to a point of successful compilation and interpretation of that compiled bytecode. Simple instructions such as '5 + 5' will actually resolve as they should at this point.

### clox 0.4.0

Again, no new files. However, lots of changes were made. Value is no longer just a typedef for 'double', but is now its own type altogether, which tracks the actual type being passed in. Currently it is a union that supports boolean and number values. As a result, several macros were made in order to type check and type cast the values.

The new additions in this version are: '!', '!=', '==', '<', '<=', '>', '>=', booleans ('true' and 'false' as literals), and nil ('nil' as a literal).

### clox 0.4.5
#### New Files
- clox_object.c && clox_object.h -- Defines a new "Obj" type that represents large objects that need to be stored on the heap, such as strings.

Apparently in the scanner I forgot to actually account for any 't' keywords, and as a result, forgot to add in 'TOKEN_TRUE'. As a result, entering 'true' in the repl or reading it from a file would throw a Lox error. That has been fixed as of this version. Everything should work now. Plus, you now have strings! And they can be concatenated! (Escape sequences coming later).

### clox 0.5.0

#### New Files

- clox_table.c && clox_table.h -- Hash table implementations in C. Objects, especially strings, are now run through the hash table to keep track of key/value pairs. Speeds of comparing strings have improved as they are now compared by their hash rather than character by character directly.

At the moment, the hash table only supports strings. I may go back in later and modify it to support the keys for the other primitive types: numbers, booleans, and nil.

### clox 0.5.5

Variables are now supported. Of course, there are no blocks or anything like that yet, so up to this point, it's just global variables. They're also not handled in the most efficient way, and re-referencing a variable can actually take up space in the stack, causing the user to run out of instructions sooner.

### clox 0.5.9

The reason this section exists is because the last time I tried implementing the local variables, it completely broke global variables. At this point, everything seems to be working as it should be. However, I fear that may not be the case when I implement the last bit of the chapter that fixes a specific edge case with scoping. I have suspicions it was in that section that things broke. It's a minor change overall (in terms of amount of code), but it does fundamentally change how the VM reacts to scoping. Hence, that is why it is '.1' shy of '0.6.0'.

### clox 0.6.0

It turns out I was incorrect. It was an error I made somewhere else in the code that was causing the issues my first time attempting the implementation. I'll have to spend the time to investigate what that may have been. Local and global variables are now a thing and do resolve correctly, even when they share a common identifier ('a' in the global scope and 'a' in the local scope are 2 different variables.)

### clox 0.6.5

- Added the main 3 control-flow structures: if-else statements, while loops, and for loops.
- Several simple tests for the VM have been added. Each one specifically covers the operations added in each version. The while loop test, specifically, is the early implementation of the fibonacci sequence (also the fastest implementation of it!)

### clox 0.7.0
- Added the support backbone for functions. It has replaced or required an update to quite a bit of the code.

One major change is that the size of the VM stack has been increased 64x, as it now needs to hold functions. Functions run on call frames which are like snapshots of the functions.

Another thing you may notice (if you have debug on), is that there is a value ```<Script>``` that is firmly placed at index 0 on the VM's stack. That is a reference to the top-level "main" function of the code. The entire script is now considered to be a function in itself, and therefore the first slot in the stack is reserved for that top-level function.

I'm going ahead and calling this '0.7.0' and committing at this point as a potential backup point. There was a lot that changed and there is still a lot more to add to finish implementing functions as a whole.

### clox 0.7.4
Functions are fully implemented, complete with return statements.. However, there definitely is a bug somewhere. I attempted to test functions using a recursive fibonacci function and it went into an endless loop. Hence, that is why it is at '0.7.4' and not '0.7.5'. That, and the fact that I haven't added in the native functions yet either. I'd rather go back and figure out the bugs in user function declarations first. Alas, I am tired and must take a break at this point. '0.7.5' will be after the bug fix. I plan on taking liberty sooner rather than later when it comes to implementing native functions to the language. The main one that I care about will be a little tough, as it'll involve implementing variadic arguments into Lox somehow or another. Maybe not, though. I might be able to piggy-back off of C's printf.

### clox 0.7.5

Good news, everyone! It turns out I'm just a dummy dumb dumb and functions weren't actually broken *at all*. They work just fine, including that recursive fibonacci test, as you'll see I've re-updated it to that, also featuring the new native clock function! (Not really new to Lox but... new in this version of the project, up to this point.)

Speaking of that recursive fibonacci test, the times I got were far more drastic for than that of the author, likely due to different hardware. For me, jlox took roughly 104 seconds to complete 40 cycles. clox++ completed the same exact script in about 12 seconds (rounding up, it was actually about 11.6)! That's a 12x boost in speed for such recursion! Keep in mind though, the more you add, it will still get EXPONENTIALLY slower. I found that the threshold began around 37 when it started slowing down by about 1.7x each time.

### clox 0.8.0
Closures have now been implemented. I added a few test programs to test out the functionality. In order, the results of each should be 'Outer', 'Outer', and 'Updated'.

This is the final point in the book before the addition of the Garbage Collector. The language up to this point is still pretty open in terms of memory allocation and deallocation. Not everything is handled completely. That will change with the next update.

### clox 0.9.0
You might ask, "Gee, that's a big version jump considering there is no new files or anything." What about new features? Not really much, necessarily. At least not available to the end user. The reason for such a big jump in version is because this point marks the addition of the Garbage Collector. That is a huge step for clox, as now even the seemingly limited memory for creating programs has now been effectively increased ad infinitum. Further debug output has been implemented for checking in on what the GC is actually doing. This includes a "DEBUG_STRESS_GC" directive that will cause the GC to run constantly. That exists mainly to help with debugging the GC itself. When it runs constantly before and after each instruction, it is almost certain to either mark something incorrectly or to free an object that it isn't supposed to. This will point out any flaws in the language implementation that prevent the GC from accessing and managing certain chunks of memory.

One thing that is of interesting note is the fact that the GC stress tester doesn't seem to slow things down much, at least not on simple programs. Even the recursive fibonacci function stress test doesn't skip a beat as a result of constant collection.

The reason that is of note is due to the GC implementation. It is a Mark-Sweep style GC, and fully pauses on the users program whenever the GC itself runs. Reasonable defaults have been set for handling the throughput of the program and latency generated by the GC, which is why it still manages such speed.

Keep in mind, however, that I have not -- by this point -- done extensive testing on it with much more complicated programs. I plan on waiting until the end of the book (just before optimizations but after the addition of classes and methods) to do such hardcore stress testing.

I will also note now that I plan on adding at least some, though likely the majority of the extra features I would like in the language BEFORE the 'Optimization' chapter.

### clox 0.9.25

Classes and instances of classes are now implemented as of this version. You can now define very simple classes (sort of loose C struct-like objects at the moment). When you create an instance, you can define fields for it through dot notation. Up to this point, this is the only way to implement fields on classes as initializers have not been implemented yet (those will come with the implementation of methods).

A new simple test case (classes.lox) has been added to showcase how it works at this point in the implementation.

### clox 0.9.75

Classes and methods (including the constructor/initializer method) have been fully implemented with the exception of inheritance. '0.10.0' will mark that addition. Outside of inheritance and built in support for abstract classes/methods, you essentially have a large extent of OOP available to you. I plan on personally implementing structs as a separate construct later on, but for now, classes can essentially act as structs, or as a traditional class with methods called on them. I may or may not make an attempt to implement abstract objects.

Fields can even be initialized with functions or classes (e.g. ```self.name = name();```). That is taken full advantage of with the new test case added for testing classes: 'listmap.lox'. As the name suggests, it implements a simple form of Lists and Maps (regular, not hash).

The test was yoinked from [loxlox](#https://github.com/benhoyt/loxlox) which is a self-hosted implementation of Lox done by user 'benhoyt' on github. He had to implement those himself for Lox since they are not implemented in the language directly, and they're a great example of how to take advantage of the lox language as a whole, really. It of course makes use of classes, as well as initializing fields with functions/classes, and even nests a function inside of a method. And, since it is a test case, the classes (well, just the list one for now), are actually instantiated and invoked at the end to test their functionality so you get a basic taste of how it works.

One final note: Not only due to the base language used to implement this (C instead of Java), but due to a small optimization, invoking methods are significantly faster than jlox.

### clox 0.10.0

It is now complete! Inheritance has been implemented. Faithful to the original, clox is complete. I have not yet gone on to the Chapter on Optimization though. I plan to do so after I've added a few new things to the language.

### clox 0.10.1

Added 4 new operator types:
- MODULO (%)
- POWER (**)
- INCREMENT (++)
- DECREMENT (--)

The increment and decrement work ALMOST the same as C. They do, in the sense that, for example:

Pre-Increment:

```javascript
var i = 0;
var x = ++i;
print i; // Expect 1
print x; // Expect 1
```

Post-Increment:

```javascript
var i = 0;
var x = i++;
print i; // Expect 1
print x; // Expect 0
```

However, the way in which I managed to implement that introduces a few bugs in their use. When using a post-crement as the iterator value for a for loop, you will get stuck in an infinite loop, unless you prepend the variable name and '='. For example:

```javascript
for (var i = 0; i < 10; i++) {} // Causes infinite loop since i is reassigned with its original value rather than the incremented value
```

A simple fix (that should be unnecessary but idfk) is this:

```javascript
for (var i = 0; i < 10; i = i++) {} // This works, for some odd reason.
```

Instead, when using a loop, it's recommended to always iterate with a pre-crement, even for while loops.

There is also a weird issue when using it with a classes fields. In this case, it is the pre-crement that is broken. For example:

```javascript
class Enum {
    init() {
        self.iotac = 0;
    }

    iota() {
        var result = self.iotac;
        ++self.iotac; // Results in an error: "Operand must be a number."
        return result;
    }
}
```

```javascript
class Enum {
    init() {
        self.iotac = 0;
    }

    iota() {
        var result = self.iotac;
        self.iotac++; // Functionally a pre-increment, for some reason.
        return result;
    }
}
```

Now, I could easily "fix" this by making both operations work as a pre-increment. However, for now, I think it's best to just keep that in mind when attempting to use them, as they are more accurate and useful as they are now. Just a little confusing right now...

I also have now moved the entire 'src' directories contents into 'clox'.

'src' is going to be my remake, which will involve significant refactoring. I will be trying to split things up a little bit more (Separate the Tokens from the Lexer, Split the Parser and Compiler into separate modules, etc.) as well. The main reason for this (mainly the splitting of the parser and compiler), is to make it easier to interface with the parser as it's own entity. I plan on developing a legitimate compiler for this language (thinking about using the qbe backend), so having that separation will make it possible to implement without including the bytecode compiler into the native compiler. In general, the goal is to shorten the files and break the code up into separate usable pieces.

For now, I will slowly be updating this version bit by bit with small new features as I rewrite and refactor. That way, there is something that is still up to date and working.

## LAX

- [lax 0.0.5](#lax-005)
- [lax 0.0.75](#lax-0075)
- [lax 0.1.0](#lax-010)

### lax 0.0.5

Well, plans have definitely changed. I've caught back up to the equivalent of Chapter 17. It's basically just a glorified calculator, but the important thing is it is working. It was seg-faulting on me and it took me forever to figure out why but I did and everything seems to work now.

I will be marking this point as version '0.0.5'. I will likely not call version '0.1.0' until I've added control flow, since that is the point where the language will actually be relatively usable.

One thing that is different compared to clox is that I have added all of the arithmetic and bitwise operators now rather than later, so they can be played with at this point in the language.

However, plans in regards to how I am going to split up modules has definitely changed.

For one, I have decided not to separate the tokens from the lexer. There wasn't much of a need since they're just in the header and not the source file anyways.

When in comes to the parser, it is very deeply intertwined with the bytecode compiler. It would be quite difficult to separate the two and make the parser more generalized, plus it would likely mean the parser would become much more bloated to handle various use cases.

Due to that, I will likely do either one of two things: make a separate standalone parser specifically for native compilation, or - what I might actually do - is use the bytecode compiler as the first step, and then natively compile from the bytecode. That honestly sounds like the better path.

### lax 0.0.75

Gonna put a minor update in here since variables, both global and local have been added. That also means global vs local scoping has been implemented. Hash tables were introduced to store variable definitions as well as strings. Strings are denoted only with double quotes (""). Concatenation is supported, though only between two or more strings. There is no support for escape sequences or string interpolation currently. That last one I'm not 100% sure of how to go about implementing, the former though I just haven't added support yet. Should be supported by version '0.1.0'.

### lax 0.1.0

Hooray! We're back to a reasonable point. As in...

- Added logical 'and' and 'or' (as literals and using '&&' or '||')
- Added if statements, while, and for loops
- Plus everything from before

At this point, the language is about as complete as a BASIC-like language, though the syntax is more C-like. One could actually argue it is more like JS since it is dynamically typed, though 'var' in this language is not as dangerous as apparently is in JS.

The next update will add functions to the mix. It will also probably take a little longer to work on as I plan on doing a MAJOR overhaul and refactor to the codebase. Some things will be split up and others will be combined. Some implementations may also possibly change though I may hold off on that for now. Regardless, it should be much easier to go through and much more suited to my later goals of adding a native compiler for the language.
