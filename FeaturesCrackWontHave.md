# Summary #

This is a list of features that have been requested but that we've decided not to have in the language.

# Wildcard imports ([Issue 107](https://code.google.com/p/crack-language/issues/detail?id=107)) #

In Java and Python, Wildcard imports allow you to import all of the symbols in a module without explicitly naming all of them.  There are several reasons why we've chosen not to implement this feature in crack:

1) It makes it difficult to determine the source of an imported symbol
2) It makes it impossible for module authors to guarantee backwards compatibility due to the risk of introducing namespace collisions.
3) The advantages can be more safely implemented with annotations or explicit scoping.

Let's say we allowed importing all symbols from the module "foo":

```
import foo *;
import bar *;

someFunc();  # someFunc comes from foo
```

In the example above, in the absence of the comment, there is no way for a reader of the code to know that someFunc() comes from "foo".

A more serious problem is that the author of "foo" can not safely add a new public symbol and release a version of "foo" that doesn't break existing code.  That new symbol might conflict with an existing symbol in the importing module.

For example, if the author of "foo" added "otherFunc()" and otherFunc() were already implemented in "bar", the module above would now fail to compile because of the naming conflict.

The best way to get the benefits of wildcard imports is with scoped names.  In the example above, we should be able to do this:

```
import foo;
import bar;

foo.someFunc();
```

Since module names can sometimes be uncomfortably long, it may be desirable to accomplish this with an alias:

```
import m = really_long_module_name;

m.doSomething();
```

Scoped names and module aliases have not been implemented yet, but will be.

Another way of safely getting the benefits of wildcard imports is through annotations.  As a trivial example, we could currently define the "foo.ann" module which contains a macro "import\_all":

```
@import crack.ann define, export, exporter;
@exporter;

@define foo_import_all() {
    import foo someFunc, otherFunc;
}

@export foo_import_all;

```

Our module could then be written as follows:

```
@import foo.ann foo_import_all;
@foo_import_all;

someFunc();
```

This usage is still problematic in that it is still difficult for the reader to determine that someFunc() was imported from foo.  But it does address the more serious concern of allowing module writers to extend their interfaces.  If the authors of foo want to add a new symbol, they can do so but should leave the original version of the macro intact so the same symbols are imported (they can create a new version of the macro to implement the newer symbols).

Additional annotation features could continue to refine this technique.

# Tuples ([Issue 128](https://code.google.com/p/crack-language/issues/detail?id=128)) #

In other languages, tuples provide a convenient method of passing aggregates without having to define formal structures.  For example, in Python we could define a function that parses a string into a set of fields:

```
def getUserInfo(record):
  # Where record is a string of the form 'name,dept,salary'
  name, dept, salary = record.split(',')
  return name, dept, float(salary)

name, dept, salary = getUserInfo(line)
```

While convenient, tuples create two problems:

  1. The lack of a formal structure means that result is defined based on the order of the return values rather than the more literate representation of names corresponding to typed values.  In the example above, it is up to the author of getUserInfo() to record that the function returns two strings and a float corresponding to to name, department and salary, and the user is likely to have to consult the documentation for every use.  This is less likely to be the case for structure fields because field names are inherently more mnemonic than positions in most cases.
  1. The use of tuples in interfaces makes it impossible for the providers of those interfaces to extend the aggregates represented by tuples in a way that is backwards compatible.  For example:
```
   import m f;

   a, b := f();
```

> Assuming Python's strict semantics (where the number of
> variables must equal the number of items in the tuple), the
> provider of the module `m` would not be able to add new values
> to the returned tuple without breaking all external users of the
> interface.  We could make this work with relaxed semantics (where
> if the number of items is greater than the number of variables,
> the remaining items are ignored), but then erroneous usage would
> be silently ignored.

The second issue is seen as a compelling case against the introduction
of tuples.  Facilitating clean change in dependent interfaces is a
major design goal in Crack, so we try to avoid features that impede
this.

As an alternative to tuples, we are open to alternate means of
facilitating light-weight aggregates.  In fact, this is already something supported by the `@struct` annotation.  We could implement
the code above as follows:

In the `m` module:

```
@struct FResult { String a; int b; };
FResult f() {
    return FResult('foo', 100);
}
```

And in the client module:

```
import m f;

result := f();
# Use result.a and result.b.
```

This is slightly more verbose than the tuple approach, but not significantly so.

There is also the possibility of introducing anonymous structures.
There is currently no design proposal for anonymous structures, but
by way of example, they might look something like this:

The `m` module:

```
@anon { String a; int b } f() {
    return @anon('foo', 100);
}
```

Usage would be the same as in the previous example.  The introduction of something analogous to the "auto" type in C++ could reduce the syntax
to:

```
@auto f() { return @anon('foo', 100); }
```


