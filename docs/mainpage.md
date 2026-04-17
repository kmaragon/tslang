# TSCC — A TypeScript Compiler {#mainpage}


## Components

- **tsccore** — Core utilities: UTF-8 handling, JSON processing
- **tsclex** — Lexer: tokenizes TypeScript source into a token stream
- **tscparse** — Parser: builds a complete AST from the token stream

## Architecture

See the \ref architecture page for a high-level overview, and
\ref parser_design for details on the parser state machine.

## Building

```bash
mkdir cmake-build-debug && cd cmake-build-debug
cmake ..
make
```

## Running Tests

```bash
cmake-build-debug/tsclex/tsclex.test
cmake-build-debug/tsccore/tsccore.test
cmake-build-debug/tscparse/test/tscparse.test
```

## Generating Documentation

```bash
ninja -C cmake-build-debug docs
```
