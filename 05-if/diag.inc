#ifndef DIAG
#define DIAG(ID, KIND, MSG)
#endif

// lexer

DIAG(err_unknown_char, Error, "unknown char '{0}'")

// parser

DIAG(err_expected, Error, "expected {0},but found {1}")

// sema

DIAG(err_redefine, Error, "redefine symbol '{0}'")
DIAG(err_undefine, Error, "undefine symbol '{0}'")
DIAG(err_lvalue, Error, "required lvalue on the assign operation left side")

#undef DIAG