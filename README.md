miniforth readme
----------------

miniforth is a small forth-like language designed to work as a shell
for embedded systems and other awkward places.

To build the library archive for linking:

    make

To build a stand-alone executable for a specific platform/api:

    make STUBS=<platform>

See `stubs/` for the list of already made stubs, and feel free to submit a pull
request if you add support for some other platform.

## Features:

- No dynamic memory allocation, all memory is passed to the interpreter
  at initialization
- Portable, no architecture-specific things used
- Doesn't rely on the (possibly non-existent) C library
- Easily extendable with stub and archive interface

## Caveats:

- Not very fast, small or featureful as far as forth-likes go
- No dynamic memory allocation makes it unsuitable for a lot of
  "real world" usage
- Not ansi compliant, and doesn't aim to be
