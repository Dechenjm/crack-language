# Introduction #

This page serves as an incubator for new ideas for the language. Once they've been fleshed out and accepted, they get their own Wiki page.


# Index #

  * Layers

# Layers #

## Summary ##
Layers provide the language with a means of defining blocks of code that are inserted and run only when when requested at runtime. Their purpose is to provide a "zero-cost" way to add assertions of invariants, boundary checking, benchmarking code and other items that are useful for debugging and testing, but not desired in production.

## Requirements ##

  * Language construct that defines the block naturally within the context it will run
  * Ability to run arbitrary code in the block
  * The entire block should be a noop unless requested at runtime, when it should run as if it were coded normally in that position
  * The ability to flag that a block of code should belong to a certain layer or layers
  * The ability to turn individual layers on/off at runtime (or native binary generation time)

## Details ##

> Proposed layers:
  * BOUNDS
  * DEBUG
  * BENCH
  * ARITHMETIC
  * ASSERT

## Implementation Details ##

Implementation will rely on the fact that LLVM allows us to programmatically change IR at runtime before adding it to the JIT (or compiling to native), and that dead code is automatically eliminated.

If we codegen the blocks normally, but keep track of their start and end points, can we  "cut and paste" the resulting IR to another section of the module? The downside here, even if possible, is that the module will wind up with invalid IR. Even if we're careful not to run it out of context, it won't run the verify pass successfully.

So, can we turn them into function calls? They would have to be parametrized by any locally scoped variables that were used in the blocks. Do we allow them to change those variables? Can they define new ones?

At compile time, we can replace a layer block with a function call to an empty function, say "crack\_layerCheck". We then decorate this instruction with metadata that points us to the function/ir block we will need to replace it with at runtime if requested.

If we're not adding in the layears at runtime, this function should automatically be removed by the DCE pass. If it's requested at runtime, we inline the function into the correct place before adding to the JIT or compiling native.

## Examples ##

```
// simplest bounds checking
class Array {
    oper [](uint i) {
        
        layer (BOUNDS) {
            if (i > arrayLen) {
                // arbitrary code? restrictions? throw exception?
                cerr `index out of bounds`;
            }
        }
        return data[i];

    }
}

```

```

// is this doable? definitions are most likely a problem
void superAlgorithm() {

    layer (BENCH) {
        starttime := microtime();
    }

    ... algorithm ...

    layer (BENCH) {
        endtime := microtime();
        cout `total: $(endtime-starttime)`;
    }

```

```
// should be doable
void myFunction(myObj o) {
    // layer exists in both ASSERT and DEBUG
    layer (ASSERT|DEBUG) {
        o.isInitialized();
    }
}

```

## Other Ideas ##

  * Our IDE can eventually hide/show individual layers on request. Hiding the layers will make it easier to focus on the core code instead of the debugging framework.
  * Custom layer identifiers. Allow people to define their own domain specific?

## Questions ##

  * Better name than Layer?

# Design By Contract #

See http://en.wikipedia.org/wiki/Design_by_contract

Crack can implement DBC as a special case on inheritence.  For example:

```

class PrimeInt : int (isPrime()) {
  @final bool isPrime() {
    for (int i = 2; i < this; ++i)
      if (!(i % this))
        return false;
    return true;
  }
}

int makeComposite(PrimeInt p, PrimeInt q) { return p * q; }

comp := makeComposite(7, 11);

makeComposite(24);  # raises crack.lang.ContractError 

```

The general syntax is "**class** _class\_name_ **:** _constrained\_class_ **(** _condition_ **)** _class\_body_"

_constrained\_class_ is automatically converted to _class\_name_ through a light-weight conversion in all contexts, with a runtime check to verify that the condition is satisfied.  Each method of the class and each attribute change operation will be followed by a runtime check for the condition.  The runtime checking can be disabled for performance in a non-debug version of the code.

_class\_name_ is a "Contract Class."  A contract class is special:

  * all methods must be final.
  * new instance variables are not allowed.
  * constructors are not allowed.

Contract classes also break the normal "single pass" rule in that the contract conditions are written before any methods that they use in the class body.

The constrained class is converted to the contract class through a light-weight conversion (actually a typecast without any opportunity for narrowing or widening: uintz(constrained class pointer) == uintz(contract class pointer)).

# Optimizations #

  * interning of Strings