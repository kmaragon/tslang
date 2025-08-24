tslang
===========

An overly ambitious project to try to build a full compiler for typescript

# Intended Requirements

* Support enough of a subset of the compiler that the compiler (pre-Go-rewrite) can be compiled with it
* Support the C++ ABI for binary interoperability with C++ (probably through clang based parsers for ts call through to c++ header gen utilities for the inverse
* Build the threading models and workers on top of a cooperative scheduler in a core language runtime library

# Explicit non-Requirements

* Full support of every part of TypeScript, especially the least compilable stuff inherited from JavaAcript

# Overall Design Plan

The plan is to have a lexer that feeds a first order parser. The first order parser does basic syntactic validation and creates a simple graph of the code. 

The parsed graph will need to be fed into a static analyzer that will have to do a ton of stuff:
* Remap `any` and `unknown` types based on implementations based on cascading out inferred types
* Do escape analysis to figure out how to allocate objects
* TBD

Then as a last pass, based on the escape analysis, the final types will be resolved into a compilable graph. That compilable graph can be passed through an LLVM pass to produce the backend

# Backend Support

LLVM does the heavy lifting here but the plan is to build out the tooling to be able to compile to native code and WebAssembly.

# Current Status

Very little, sadly, with what time is there. The lexer is mostly functional but still lacks support for /<regex>/<flag> style regular expressions and there's no JSX. These are absolutely planned though.

It's been slow to evolve just due to time constraints but I decided to make it public in spit of how incomplete it is.
