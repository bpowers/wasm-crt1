Emscripten WebAssembly crt1 support
===================================

C and C++ programs can require doing a lot of work in-between calling
`execve()` on a binary and when that program's `main()` method is
invoked.  This could involve running functions marked with gcc's
`__attribute__((constructor))` syntax, as well as functions to perform
dynamic initialization of C++ global objects.

`main.c` in this repo is an example of a simple C file that has 4
functions:

- `fib()` that calculates the n'th Fibonacci number
- `main()` that calls fib and prints a message
- `init()` that prints a message before main
- `fini()` that prints a message after main

When this file is compiled to an executable and run, the `_start`
symbol is called (not `main()` directly).  `_start` jumps to
`_libc_start_main`, which takes care of calling initializers (like our
`init()`), invoking `main()`, and finally calling destructors.

Unfortunately, WebAssembly binaries compiled with Emscripten do not
have `_start` or `_libc_start_main` functions -- the customized
JavaScript wrapper file emitted next to each WebAssembly binary is
responsible for that.  For example, after running:

`$ make`

In this repo, `main.js` contains:

```JavaScript
/* global initializers */  __ATINIT__.push({ func: function() { _init() } });

// ...

function ensureInitRuntime() {
  if (runtimeInitialized) return;
  runtimeInitialized = true;
  callRuntimeCallbacks(__ATINIT__);
}
```

We would like to push this functionality into the WebAssembly binary
so that we can have a single JavaScript wrapper that is able to run
_any_ WebAssembly binary in Browsix.

The idea is to generate a simple C file with a single function,
`_browsix_start()`, that takes care of the above.  For this example,
we would expect it to look like the following:

```C
// from <stdlib.h>
void exit(int);

// from our program
void init(void);
void fini(void);
int main(int, char**);

void
_browsix_start(int argc, char *argv[])
{
	init();
	int result = main(argc, argv);
	fini();

	exit(result);
}
```

This simple C file would be written to a temporary file, compiled with
Emscripten into an LLVM bitcode file, and that would be included with
the rest of the program's bitcode object files when linking the final
WebAssembly binary, giving us a simple, single entry point we could
invoke.