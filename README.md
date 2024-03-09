# Lax

Lax is (currently) a dynamically-typed language based on Lox (the clox bytecode interpreted edition). Later, when I have finished implementing Lax, it will be its own separate language based on clox, with the several additions including a native compiler. The reason I said 'currently' earlier is for that reason: when I implement the native compiler, I plan on implementing static typing for that compilation mode.

You will have the option of using the dynamically-typed version of Lax (clox extended), or you can use a superset (true Lax) that is statically-typed and compiles to native assembly instructions. I'm more than likely going to be using the [qbe](#https://c9x.me/compile/) backend so it should be pretty well cross-platform.

For now, you're stuck with my implementation of clox with some extra features added (also 'this' keyword was changed to 'self' cause reasons).

## Table of Contents

- [CHANGELOG](#changelog)
- [Upcoming](#upcoming)
- [Features](#features)
- [Usage](#usage)
- [MANUAL](#manual)
- [LICENSE](#license)

## CHANGELOG
You can view the changelog [here](#https://github.com/Utecha/Lax/blob/main/CHANGELOG.md).

## Upcoming

- Dynamic Arrays
- Maps
- More built-in functions such as: printf, system, exit
- Bitwise operators
- Ternary operators
- String interpolation
- Support for escape sequences in strings
- Native compilation
- Static typing for the native compiler
- Structs as a separate object -- Will ultimately be functionally the same as C++
- Built-in support for enums. Likely C/C++ style enums.
- Module support (currently Lax/clox programs only support a single source file)
- Macros
- Properly implemented pre and post increment/decrement operators (current implementation mostly works but is a bit buggy)
- Built-in support for variadic arguments to functions/methods

## Features

- Arithmetic Operators ( + || - || * || / || % (Remainder) || ** (Power))
- Comparison Operators ( > || >= || < || <= )
- Equality Operators ( == || != )
- Unary Operators ( ! || - || ++ || -- ) **see next**
- Increment Operators
    - ( ++ || -- )
    - Usable in both pre and post contexts. Weird bugs currently, see [MANUAL](#manual) for more info on their usage.
- Variables Declaration & Definition (like C -- Declared variables left undefined are initialized to 'nil');
- 'nil', 'true', and 'false' as functional keywords. Nil is a rough equivalent to NULL from C and other languages.
- Functions
- Classes, Methods, and Inheritance
- Very fast -- Bytecode compiled with a VM to interpret, and a built in garbage collector.

## Usage

To install, first clone the repo:

```console
git clone https://github.com/Utecha/lax.git
cd lax
```

Then, run ```make``` to build and generate the executables.

For now, only clox is usable, so to run it, enter:

```console
./clox
```

Running it without any arguments puts you into the REPL, much like you would with Python (though this REPL is not quite as good, admittedly).

You can also give it some arguments, such as a file:

```console
./clox source_file.lox
```

That runs code from an entire file (as you'd expect), which is much nicer than working 1 line at a time! (Especially if you're trying to squeeze something like a class on 1 line!)

When a usable version of Lax releases, I will be implementing support for multiple files.

Later on, there will also be usable commands to go along with it, though that likely won't be a reality until I have begun work on the native compiler.

## MANUAL

The manual is surprisingly extensive for such as language. This will be implemented soon!

If you have experience with C, Python, or JavaScript (especially that last one), it's similar to those, but way less (or more, in certain ways) featureful lol.

## LICENSE
Lax is governed under the same license as namesake Lox, the [MIT License](#https://github.com/Utecha/Lax/blob/main/LICENSE)
