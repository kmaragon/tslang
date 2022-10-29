tsclex
===========

The typescript lexer.

# Design Considerations

While moving forward, here are some high level design considerations while working on the lexer

## Preserving Microsoft Conceptual structure

The public interface of the entire typescript front-end should reflect the
[microsoft implementation](https://github.com/microsoft/TypeScript) as much as possible. The intention is that
the lexer and first order AST can be ported into the microsoft code base, which is itself written in typescript. 
We can then compile that and build out a new typescript based front-back-end that does the AST linguistic analysis 
in typescript rather than C++ using the LLVM backend to generate the compiled code. But before we can do that, 
we need a compiler that can compile Typescript to machine code. So this is a design strategy toward a multi-year 
long-term goal rather than a project plan.