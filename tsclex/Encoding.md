Encoding Handling while Lexing
=================================

tscc only supports utf-8 source code files. The rules that
apply to typescript still apply though. That is, non-utf-8 
control sequences are still processed. This only pertains to
the encoding of the source code file itself.

As such, all internal symbol representations are in utf-8 to
make for the easiest compatibility with llvm. However, the
language itself has native string support for unicode strings
which are represented as u32string values. This ensures 
consistent behavior with respect to things like `string.length`.

As a result, the lexer reads in wide character code points from
an encoded stream of data in utf-8. String constants retain their
wide-character-ness. Everything else is normalized back to a 
consistent utf-8 representation. Therefore native system-level
debugging and instrumentation tools just work while the language
runtime operates in unicode.