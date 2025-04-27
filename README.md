
# LL(1) Parser with Lexical Analysis and Grammar Processing

## Project Description
A C++ implementation of an LL(1) parser that performs lexical analysis, grammar transformations (left factoring and left recursion removal), and predictive parsing table generation. Designed to parse a subset of C-like syntax including control structures, expressions, and type declarations.

## Key Features
- **Grammar Processing**
  - Left Factoring
  - Immediate/Non-Immediate Left Recursion Removal
- **Compiler Components**
  - Lexical Analyzer (Tokenizer)
  - FIRST/FOLLOW Set Computation
  - LL(1) Parsing Table Generation
- **Language Support**
  - Control Structures: `if/else`, `while`, `for`
  - Logical Operators: `&` (AND), `|` (OR)
  - Arithmetic Operators: `+`, `-`, `*`, `/`, `%`
  - Relational Operators: `<`, `>`, `@` (custom comparison), `!=`
  - Type Declarations: `int`, `float`, `double`, `char`

## File Structure

```
├── parsingtable.cpp   # Grammar processing and parsing table logic
├── lexer.cpp          # Lexical analyzer implementation
├── index.cpp          # Main driver and parser
├── cfg.txt            # Input context-free grammar
├── code.txt           # Sample source code to parse
└── parsinglogs.txt    # Output parsing log (auto-generated)
```

## Installation & Usage

### Prerequisites
- C++17 compiler (g++ or clang++)
- Make (optional)

### Compilation
```bash
g++ index.cpp -o parser -std=c++17
```

### Execution
1. Prepare your CFG in `cfg.txt`
2. Write source code to parse in `code.txt`
3. Run:
```bash
./parser
```

### Example `cfg.txt` Structure
```txt
S → int main ( ) { <stmt_list> }
<stmt_list> → <stmt> <stmt_list> | ε
<stmt> → if ( <expr> ) { <stmt_list> } <else_opt> | ...
... (see provided cfg.txt for full syntax)
```

### Example `code.txt`
```c
int main() {
  float x = 3.14;
  if (x @ 3.14) {
    x = x + 1;
  }
  return 0;
}
```

## Output
- **Parsing Table:** Printed to console in formatted table
- **Parsing Logs:** Saved to `parsinglogs.txt` with:
  - Token sequence
  - Parsing stack operations
  - Syntax error reports with line numbers
  - Final ACCEPTED/REJECTED status

## Error Handling
- Skips invalid tokens during lexical analysis
- Implements panic-mode error recovery during parsing
- Reports line numbers for syntax errors

## Custom Operators
- `@`: Comparison operator (equivalent to `==` in C)
- `|`: Logical OR (written as `orOpt` in CFG)
- `&`: Logical AND

