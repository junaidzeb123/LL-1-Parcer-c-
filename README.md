
# LL(1) Parser with Left Factoring & Left Recursion Removal

## Overview
This C++ project reads a context-free grammar (CFG) from a file and performs:
- **Left Factoring:** Removes common prefixes.
- **Left Recursion Removal:** Eliminates left recursion.
- **FIRST & FOLLOW Sets Computation:** Calculates FIRST and FOLLOW sets for non-terminals.
- **LL(1) Parsing Table Construction:** Builds the parsing table for predictive parsing.

## Files
- **main.cpp:** Source code.
- **input.cfg:** Sample CFG file.

## Usage
1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/LL1-Parser.git
   cd LL1-Parser
   ```
2. **Compile:**
   ```bash
   g++ main.cpp -o ll1parser
   ```
3. **Run:**
   ```bash
   ./ll1parser input.cfg
   ```

## Example CFG
```
E -> E + T | T
T -> T * F | F
F -> ( E ) | id
```

The program outputs the transformed CFG (after left factoring and left recursion removal), the computed FIRST/FOLLOW sets, and the LL(1) parsing table.
