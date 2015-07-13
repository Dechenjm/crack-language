# Crack Canonical Names #

Every named element in a crack program has a unique canonical name associated
with it.  These names are necessary for converting crack code to native
compiled formats so that traditional tools can deal with identically named
elements in different parts of the program without naming collisions.

In general, canonical names are hierarchical: the name of any element is
prefixed by the name of its parent element.

## Modules ##

Module names are the same as the names used in the import statement:

```
    import crack.io cout;
```

The `crack.io` module has the canonical name "**crack.io**".

## Variables ##

Variable names are the name of the context of the variable followed by the
variable itself.  For example, the name of the `cout` variable from `crack.io`
above is "**crack.io.cout**".

## Internal Elements ##

There are certain internal elements that are provided by the compiler (for
example, the built-in types and functions like "`int`" and "`puts`").  In
order to provide them with their own namespace that cannot clash with a the
module namespace, the names of all internal elements begin with a period.

In particular, the builtin types begin with ".builtin.".  So the `int` type
would be named "**.builtin.int**".

There is also a class of internal names that are scoped to existing objects -
these correspond to the names of special global variables and functions used
to store information about existing elements.  These names are the name of
their associated element followed by a colon and the name of the special
element. For example, every class has a runtime implementation object called
"body", so the class `foo.Bar` would have a variable named "**foo.Bar:body**".

## Classes ##

Class names are like variable names, they are simply the containing context
followed by the actual class name.

```
    # in module "foo"

    class Bar {} # the class name is "foo.Bar"
```

## Functions and Methods ##

Since crack supports function overloading, and there can be multiple functions
with the same base name, the canonical name of a function includes the names of
the types of its arguments:

```
    # in module "foo"
    
    void func(String val, int number) {}
```

`func()` would be named "foo.func(crack.lang.String, .builtin.int)"

The names of operators is just a normalized form of the operator name:

```
    # in module foo
   
    class Bar {
        # full name is "foo.Bar.oper +(foo.Bar)"
        Bar oper +(Bar other) { return Bar(); } 
    }
```

# Name Mangling #

Not all of the symbols that are legal for crack canonical names are legal for
most linker formats.  Crack currently uses the name mangling provided by its
LLVM back-end.  In the long term, we will probably want to use a format that
is a little terser.

The goals for name mangling are:
  * it should part of a stable, predictable ABI that allows crack functions to be callable from C code.
  * mangled names should be human-readable.

The current standard for name mangling is:

  * The underscore is an escape character.
  * Underscores in the canonical name translate to double underscores in the
> mangled name.
  * A period in a canonical name translates to `_p` (or perhaps a dollar sign,
> which is normally legal in mangled names but not in crack names).
  * Operator names are condensed to a more terse format ("oper +" to "`_op`",
> "oper -" to "`_om`" ...)  Additionaly, the "`_p`" period escape prior to an
> oper name may be elided.
  * builtin names are also condensed ("int32" to "`_i4`", "float32" to "`_f4`"
  * leading parenthesis are converted to "`_b`" (begin), trailing to "`_e`" (end).

So for example, "**foo.Bar.oper +(foo.Bar)**" would mangle to
"`foo_pBar_op_bfoo_pBar_e`".

