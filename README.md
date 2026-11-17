# Source Code to HTML Converter

## Project Description

Source Code to HTML Converter is a C-based project that converts C source code into an HTML file with syntax highlighting.

It identifies different elements of C programming, such as keywords, identifiers, operators, constants, strings, and comments, and generates an HTML file with appropriate styling.

## Features

- Converts C source code into HTML.
- Highlights C keywords and identifiers.
- Recognizes operators and special symbols.
- Identifies constants and string literals.
- Handles comments.
- Uses CSS for syntax highlighting.
- Generates an HTML output file.

## Technologies Used

- C Programming
- HTML
- CSS
- GCC Compiler
- Linux
- Makefile

## Project Structure

```text
Source-Code-to-HTML-Converter/
│
├── html/
├── html_converter.c
├── html_converter.h
├── lexer.c
├── lexer.h
├── main.c
├── makefile
├── sample.c
├── sample.c.html
└── style.css
```

## Compilation

Compile the project using:

```bash
make
```

## Execution

Run the generated executable:

```bash
./s2html sample.c
```

## Output

The program generates an HTML file from the input C source file.

Open the generated `.html` file in a web browser to view the syntax-highlighted source code.

## Learning Outcomes

- Understanding lexical analysis.
- Working with file handling in C.
- Understanding source code processing.
- Generating HTML dynamically using C.
- Learning how CSS can be used for syntax highlighting.

## Author

**Varsha M**

GitHub: [varsha1907-m](https://github.com/varsha1907-m)
