*This project has been created as part of the 42 curriculum by zbidas.*

# ft_printf

## Description

`ft_printf` is a re-implementation of a subset of the C standard library function `printf`. The goal of the project is to learn how **variadic functions** work in C (the `<stdarg.h>` macros) and to build a small, dependency-free formatted output function on top of the `write` system call.

The result is a static library, `libftprintf.a`, exposing:

```
int ft_printf(const char *format, ...);
```

It returns the number of characters written, or `-1` if a `write` fails (or if the format string is `NULL` or ends with a lone `%`).

### Supported conversions

| Conversion | Prints |
|---|---|
| `%c` | a single character |
| `%s` | a string (`(null)` if the pointer is `NULL`) |
| `%p` | a pointer in hexadecimal, prefixed with `0x` (`(nil)` for `NULL`, as on Linux) |
| `%d` / `%i` | a signed decimal integer |
| `%u` | an unsigned decimal integer |
| `%x` / `%X` | an unsigned integer in lowercase / uppercase hexadecimal |
| `%%` | a literal `%` |

A `%` followed by an unsupported character is printed as ordinary text (the behaviour of the real `printf` is undefined in that case, and the project does not require handling it).

### Project files

| File | Role |
|---|---|
| `ft_printf.c` | the main loop over the format string, and the dispatcher on the conversion character |
| `ft_putchar.c`, `ft_putstr.c`, `ft_strlen.c` | character and string output |
| `ft_putnbr.c` | signed integers |
| `ft_putnbr_base.c` | unsigned integers in any base (used for `%u`, `%x`, `%X`, `%p`) |
| `ft_putadd.c` | pointers (`%p`) |
| `ft_printf.h` | prototypes |
| `Makefile` | builds `libftprintf.a` |

## Instructions

### Compilation

```
make        # builds libftprintf.a
make clean  # removes the object files
make fclean # removes the object files and libftprintf.a
make re     # fclean, then all
```

The library is compiled with `cc -Wall -Wextra -Werror`.

### Usage

Include the header and link the library:

```c
#include "ft_printf.h"

int main(void)
{
	int	n;

	n = ft_printf("Hi, I'm %s and this is my ft_printf project (%d%% done!)\n", "Zineb", 100);
	ft_printf("%d characters were printed\n", n);
	return (0);
}
```

```
cc main.c libftprintf.a -o demo
./demo
```

### Testing

The output and the return value can be compared with the real `printf`, by calling both with the same arguments in a small `main`, and checking that the text and the returned count are identical. Cases worth testing: `INT_MIN`, `INT_MAX`, `UINT_MAX`, `0`, `NULL` for `%s` and `%p`, real 64-bit pointers,
`%c` with `0`, an empty string, and several conversions in one call.

## Algorithm and data structures

### Overall algorithm

`ft_printf` makes a **single pass** over the format string, from left to right, in `O(n)` for a format of length `n` (plus the length of the printed values).
At each position there are only two cases:

1. The character is **not** `%`: it is written as it is.
2. The character is `%` followed by another character: the next character is looked at by a dispatcher (`check_specifier`), which reads the matching argument with `va_arg` and calls the right output function.

Every output function returns the **number of bytes it wrote, or `-1`** on error. The main loop adds each result to a running total and stops at the first `-1`, calling `va_end` exactly once before returning. Using the same return convention everywhere keeps the code short and makes error handling uniform.

The loop body is split into one helper that handles a single step and the loop itself, so that each function stays short and has a single responsibility.

### Variadic arguments

The arguments are read with `va_start`, `va_arg` and `va_end`. The `va_list` is passed to the dispatcher **by address**. A `va_list` is a bookmark on the current argument: if it were passed by value, the callee could advance a copy and the caller would read the same argument twice on some platforms, and the C
standard says the caller's value is indeterminate afterwards. Passing a pointer guarantees there is only one bookmark.

The type given to `va_arg` must match what the caller passed, taking the default argument promotions into account: `%c` is read as an `int` (a `char` is promoted), `%d`/`%i` as `int`, `%u`/`%x`/`%X` as `unsigned int` and `%p` as `void *`.

### Printing numbers

Numbers are printed with a **recursive** function that works in any base. To print `n` in base `b`, it first prints `n / b` (the more significant digits), then writes the single digit `n % b` taken from an alphabet string (`"0123456789"`, `"0123456789abcdef"`, `"0123456789ABCDEF"`).

Justification:

- Digits are produced from the least significant to the most significant by repeated division, but they must be printed in the opposite order. The recursion's call stack plays the role of the stack that reverses the order, so no buffer, no `malloc`, and no string reversal are needed.
- One function serves `%u`, `%x`, `%X` and `%p`: only the alphabet and the base change.
- Signed integers are converted to `unsigned int` after printing the minus sign, so `INT_MIN` is handled correctly (its absolute value does not fit in an `int`).
- The number function takes an `unsigned long`, because a pointer is 64 bits wide on the target platform. A 32-bit parameter would silently cut the upper half of an address.

### Data structures

No custom data structure is needed. The only state is the `va_list`, the pointer into the format string, and the running character count. The alphabets are constant strings, and the recursion stack is used implicitly, as described above.

### Trade-offs

Each character is sent with its own `write` call. This is simple, needs no allocation, and makes the return value and error handling trivial, but it is slower than building the output in a buffer and writing it at once. The project values correctness and clarity, so the simple approach was chosen.

## Resources

- `man 3 printf`: the reference behaviour that is being reproduced.
- `man 3 stdarg`: `va_start`, `va_arg`, `va_end` and `va_copy`.
- `man 2 write`: the system call used for all output.
- B. Kernighan and D. Ritchie, *The C Programming Language*, section 7.3 (variable-length argument lists).
- cppreference, "Variadic arguments": https://en.cppreference.com/w/c/variadic
- Eli Bendersky, "Stack frame layout on x86-64":
  https://eli.thegreenplace.net/2011/09/06/stack-frame-layout-on-x86-64 (how arguments are passed in registers and on the stack, which explains why `va_arg` needs the exact type).

### Use of AI

AI (Claude, by Anthropic) was used as a study and review tool, for the
following tasks:

- **Learning:** explaining how `<stdarg.h>` works, including what happens under the hood (registers, the stack, the `va_list` structure on x86-64), and suggesting books, videos and documentation.
