# clox++
clox++ is an extended implementation of the Lox language made by Robert Nystrom, following the book [Crafting Interpreters](#https://craftinginterpreters.com/).

This is not my original edition, though I may include a link to that as well if you want to see the differences. I'm still relatively new and uncomfortable at this point with working on such a project but I made it through the Java part fine despite not knowing any Java (I basically learned it a decent bit from the book). That being said, that implementation did have support for all bitwise operators (&, |, ^, <<, >>) as well as modulo (%) and power (**). In my process of learning in this section, I managed to sort of figure out a way to incorporate escape characters, but the only ones I really cared that much to add (considering it took me forever to figure out) were newlines and tabs. I also added the C-style increment and decrement operators (++, --) but my implementation was a little buggy to say the least. For both of those, I've learned better ways of implementing them.

That brings me to clox++. This edition is going to have a handful of extra features not included with the original language:

- Bitwise Operators
- Modulo & Power
- More built-in functions (printf being highest priority);
- Support for variadic arguments
- Support for multiple files featuring an 'include' keyword
- Structs - I'm debating on this one since at the end of the day, you can just define a class fields and no methods and you basically have a struct. In C++, you can quite literally use the 2 keywords interchangeably, though if I'm not mistaken, you can actually define methods on your 'struct' (class).
- Built-in array support. Thanks to a fellow learner who implemented Lox in itself, I learned a good bit about how they work because he quite literally implemented them in Lox, err.. in Lox. When I say them, I mean lists (basic arrays) and maps (dictionaries for you Python people). Ideally though, I would like to leverage C to add proper arrays, in the same sense as C or basically any other language. Rather than creating a list by instantiating a list class (as in the loxlox implementation), it will be built-in from the C bytecode compiler and work the same way as any other language -- access the index of the item using '[]'.
- Some degree of a standard library. I'd like to add some more basic but very necessary things like file i/o, some ways to print to stdout/stderr. More general interfacing with the OS (Linux only for now).
