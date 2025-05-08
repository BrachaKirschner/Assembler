# 20465 Assembler

This project is an assembler for a simplified assembly language, developed as part of the 20465 Systems Programming Lab.

It reads `.as` files, performs a two-pass assembly process, and generates output files with machine code, external references, and entry points.

## Authors
- Noa Rosenbom  
- Bracha Kirschner

## How It Works
1. **First Pass:** Scans the source to build symbol and data tables.
2. **Second Pass:** Generates the final output files using the tables and resolves symbols.

## Output Files
- `.ob` - Object file with machine code
- `.ent` - Entry symbols
- `.ext` - External symbols used

## Compilation
Compile with:

```bash
make
