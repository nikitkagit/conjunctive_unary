# SetEquation

A small C++17 solver for systems of equations over sets of natural numbers. Each
equation assigns to a named variable a set expression built from fixed sets,
unions, intersections and pairwise sums of other variables. The solver iterates
the whole system a configurable number of times and prints the final values.

## Features

- Set expression language with:
  - fixed sets: `{1, 2, 3}`
  - variable references: `A`, `B`, `C`, … (identifiers)
  - union: `|`
  - intersection: `&`
  - pairwise sum: `+`
  - parentheses for grouping
- Operator precedence (highest to lowest): `&`, `+`, `|`
- Recursive / mutually recursive equations (evaluated iteratively)
- Command-line interface with configurable input/output files and iteration count
- Comprehensive test suite (expressions, parser, calculator, application)

## Requirements

- CMake ≥ 3.16
- A C++17 compiler (MinGW-w64 g++, GCC, Clang or MSVC)
- CTest (bundled with CMake) for running the tests

## Building

```bash
cmake -S . -B build
cmake --build build
```

## Running

```bash
./build/conjuctive_unary_example [options]
```

Options:

| Option | Default            | Description                          |
|--------|--------------------|--------------------------------------|
| `-f`   | `gcu_input.txt`    | Path to the input file               |
| `-o`   | `gcu_output.txt`   | Path to the output file              |
| `-n`   | `30`               | Number of iterations                 |
| `-h`, `--help` | —          | Print usage and exit                 |

The result is written to the output file first and then to standard output.

## Input format

The input file contains one equation per line. Blank lines are ignored.
Each equation has the form:

```text
VAR_NAME = expression
```

where `expression` is built from:

- fixed sets: `{1, 2, 3}`, `{}` (empty set)
- variable names, which are created on first use
- the operators `&`, `+`, `|`
- parentheses `(` `)`

Operators associate left-to-right and follow the precedence `&` > `+` > `|`.
Variables referenced in an expression may be defined later in the same file;
the system is brought to a fixed point through iteration.

Example (`gcu_input.txt`):

```text
A = {1, 2, 3} | (B & C)
B = (A + {1}) & {2, 3, 4, 5}
C = ({1, 2} + {3}) | (A & B) | {10}
```

## Variable names

Variable names follow standard C identifier rules:

- the first character must be a letter (`A–Z`, `a–z`) or underscore `_`;
- subsequent characters may be letters, digits (`0–9`) or underscore `_`;
- names are case-sensitive (`A` and `a` are different variables);
- no length limit;
- Cyrillic letters, spaces, hyphens and other symbols are not allowed;
- reserved words are not checked, so names like `if` or `_` are valid.

Valid examples: `A`, `foo_bar`, `_tmp`, `x1`, `Counter2024`.

## Output format

The output lists every variable defined in the system, one per line,
sorted alphabetically by name:

```text
A = { 1 2 3 4 5 }
B = { 2 3 4 5 }
C = { 2 3 4 5 10 }
```

## Quick example

Create `gcu_input.txt`:

```text
A = {1, 2}
B = A | {3}
C = B & {2, 3, 4}
```

Run:

```bash
./build/gcu -f gcu_input.txt -o result.txt -n 10
```

Console and `result.txt` both contain:

```text
A = { 1 2 }
B = { 1 2 3 }
C = { 2 3 }
```

## Behavior on invalid input

Parsing is strict and fails on the first malformed line. Specifically:

- **Blank and whitespace-only lines are skipped** — they do not count as equations.
- **A syntax error aborts the whole run.** The application prints a message of the form  
  `Parse error on line N: <reason>` to `stderr` and exits with a non-zero status.
  Neither the input file nor the output file is processed further, and the
  output file is **not** written (any pre-existing file with that name is left
  untouched only if it was not opened yet — in practice the output is opened
  only after successful parsing, so it is not created at all).
- **No partial system is built.** If the input has three equations and the
  second one is broken, the first equation is discarded and no iteration is
  performed.
- **The LHS variable of a failed equation is not created.** For example, in  
  `A = B &` the `&` operator is missing its right operand, so `A` is not
  registered in the dictionary. Variables that appear on the right-hand side
  are created eagerly during expression parsing, so `B` in that example *would*
  have been created before the error was detected — this is the only observable
  side effect of a failed parse.
- **Unknown characters, missing operands, unbalanced parentheses,
  malformed fixed sets and trailing input** all trigger a syntax error. Examples
  that fail to parse:

  ```text
  A
  A =
  = {1}
  A = {1
  A = {1, }
  A = {1 2}
  A = {x}
  A = ( {1}
  A = & {1}
  A = {1} + 
  A = {1} extra
  ```

### Core classes

- **`Expression`** — abstract base for set expressions; `calculate()` returns a `data_t`.
- **`ConstantExpression`**, **`UnionExpression`**, **`IntersectionExpression`**, **`SumExpression`** — concrete operations that own their operands.
- **`VariableExpression`** — mutable named variable; the value is what the system converges to.
- **`ExpressionProxy`** — non-owning wrapper used to break ownership cycles (e.g. `A = A | {1}`).
- **`Parser`** — recursive-descent parser; builds an expression tree and registers a new `Equation` with the calculator.
- **`Equation`** — owns an expression and references a target variable; `calculate()` assigns the expression result to it.
- **`EquationCalculator`** — owns all variables and equations; `iterate()` evaluates every equation once.
- **`VariableDictionary`** — bidirectional name ↔ variable map used by the parser and printer.
- **`ResultPrinter`** — formats the final variable values and writes them to a file and/or stream.
- **`Options`** — command-line option parsing.
- **`Application`** — glue code tying everything together; `main()` just calls `Application::run()`.

## Testing

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```