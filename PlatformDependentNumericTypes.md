# Introduction #

This is a design document for the implementation of Crack's platform dependent numeric types (PDNTs) and the rules for conversion between them and the Universal Numeric Types (UNTs - e.g. int32, uint64, float32, byte...)

## Definitions ##

The "target compiler" is the C++ compiler used to build the crack executor for a given platform.

## The Types ##

The PDNTs are:

| int | Signed integer the size of "int" on the target compiler.  `int` should be at least 32 bits and no more than 64 bits in size. |
|:----|:-----------------------------------------------------------------------------------------------------------------------------|
| uint | Unsigned integer the size of "int" on the target compiler.  `uint` should be at least 32 bits and no more than 64 bits in size. |
| intz | Signed size integer, suitable for describing memory sizes and offsets.  Equivalent to `ssize_t` on the target compiler.  Should be at least 32 bits and no more than 64 bits in size.  Furthermore, sizeof(`intz`) should be >= sizeof(`int`) |
| uintz | Unsigned size integer, suitable for describing memory sizes and offsets.  Equivalent to `size_t` on the target compiler.  Should be at least 32 bits and no more than 64 bits in size.  Furthermore, sizeof(`uintz`) should be >= sizeof(`uint`) |
| float | Floating point value the size of `float` on the target compiler.  Should be at least 32 bits and no more than 64 bits in size. |

**Note:** Since crack only guarantees compatibility within a major version, we reserve the right to change the maximum sizes of PDNTs between major versions.

## Conversion Rules ##

You can implicitly convert from any UNT to any PDNT.  Likewise, you can implicitly convert between any two PDNTs.

You may only implicitly convert a PDNT to a UNT in cases where there can be shown to be no precision loss.  For example, you can implicitly convert from int -> int64 because int is at most 64 bits.