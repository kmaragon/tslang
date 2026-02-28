# Architecture {#architecture}

TSCC is organized as a multi-pass compiler pipeline. Each pass is implemented
as a separate library with well-defined inputs and outputs.

## Pipeline

```
Source Text → [tsclex] → Token Stream → [tscparse] → AST → ...
```

Future passes (symbol resolution, type checking, code generation) will consume
the AST produced by the parser.

## tsccore

Core utilities shared across all components:

- **UTF-8/UTF-32 conversion** — `tsccore/utf8.hpp`
- **JSON processing** — used for configuration and tsconfig.json

## tsclex

The lexer converts a `std::istream` into a lazy token stream accessed via
iterators. Key design points:

- 100+ token types, each with its own class inheriting from `basic_token`
- Tokens managed through `std::variant` in the `token` class
- Visitor pattern for type-safe token dispatch
- Source location tracking on every token
- ECMAScript language version support (ES3–ESNext)

## tscparse

The parser eagerly consumes all tokens and produces a `source_file_node`
containing all top-level declarations. See @ref parser_design for the
state machine architecture.

Key design points:

- Stack-based state machine with type-safe transitions
- Exception-based error handling with TypeScript-compatible error codes
- AST nodes store all consumed tokens for exact source location preservation
- Optional trivia index for comment/whitespace tracking
