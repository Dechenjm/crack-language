# Binary Operator Resolution #

```
<mmuller> the new rule is: look for a matching method on the lhs, look for a matching reverse method on the rhs, then look for a matching function
<mmuller> in each pass, we attempt to resolve first with, then without conversions
<mmuller> so in the case of "int32 + float": check for int32.oper +(int32 convertible)
<mmuller> 2) check for float.oper r +(float compatible)
<mmuller> 3) check for oper +(int32 compatible, float compatible)
<mmuller> in this case, we would stop at 2 because int32 is convertible to float
```