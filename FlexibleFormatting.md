# Introduction #

In trying to create a JSON Formatter class I ran into some inflexibility that should be relatively simple to improve upon.

At the moment classes implement `writeTo(Writer)` methods to create string representations of themselves, although the base Formatter object actually calls `writeTo(Formatter)`.

The `writeTo(Writer)` methods usually contains something like (for Array):
```
    void writeTo(Writer out) {
        FStr fmt = {};
        out.write('[');
        ...
        out.write('[');
     }
```

That is, it creates a `StringFormatter` to create the representations, making it _impossible_ to create meaningfully different Formatters for non-elementary types.

# Proposal #

How about adding `writeTo(Formatter)` methods to the standard classes, so that it becomes possible to have (for Array again):
```
    void writeTo(Formatter fmt) {
        fmt.write(fmt.ARRAYSTART);
        for (elem :in this) {
        fmt.write(fmt.ELEMSEP);
            fmt.format(elem);
        }
        out.write(fmt.ARRAYEND);
     }
```

The `JsonFormatter` class then need to implement `format()` methods for the basic types like the current base `Formatter` already does. This would also make more flexible formatters for generic classes possible.

With a small list of constants we can construct very flexible formatters for JSON, BSON, YAML, etc:

|`ARRAYSTART`   | `[`|
|:--------------|:---|
|`ELEMSEP`      | `,`|
|`ARRAYEND`     | `]`|
|`MAPPINGSTART` | `{`|
|`MAPPINGEND`   | `}`|
|`KEYVALSEP`    | `:`|

This wouldn't break existing code AFAICS. The base `Formatter` class needs to be abstract for this to work, or we need virtual inheritance.

# Comments ? #