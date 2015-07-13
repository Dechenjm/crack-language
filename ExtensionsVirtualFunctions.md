# Bidirectional Virtual Function Calls Across Language Barriers #

This is a proposal for implementing bidirection virtual function calls between
crack code and extension code.  We are specifically concerned with the
implementation of this mechanism between Crack and C++, although it should be
possible to leverage this mechanism for other languages.

Bidirectional virtual function calls are a property of the extension system
that allows an extension class to be extended in Crack such that virtual
methods defined in the extension class can be overriden in any Crack classes
that derive from it.

  * Crack code calling non-overriden virtual methods will call the final implementation in the extension (by "final", we mean the implementation of the most specific implementing class that the Crack class derives from).
  * Extension code calling an overriden virtual method on a Crack derived class will call the Crack implementation.  This property is required to implement callbacks.
  * An unimplemented abstract virtual method in the extension will remain abstract in the Crack class until it is overriden by a Crack derived class.

To implement this functionality, we are limited by the fact that C++ does not
provide a standard ABI - so there's no way we can simply adapt Crack types to
extend the C++ vtable.  However, we can leverage the fact that Crack _does_
provide a standard ABI.

We make one assumption about C++ classes that we will be allowed
to derive from in Crack: an instance of the class uses no memory before the
pointer to the instance, and none after the pointer + the instance size.
Without these properties, it would not be possible to safely extend the class
in C++, therefore we do not expect such a class would be extended in Crack.

This assumption allows us to extend the class using the standard extension
mechanism for dealing with structures in Crack.  The instance area of a class A
is regarded as an opaque memory area of size sizeof(A) encapsulated in a
VTableBase derived Crack class instance.  That is to say, it will be preceeded
by a single pointer to the Crack vtable for the instance.  Conceptually, the
"Crack proxy class" for A is roughly defined as follows:

```
    class A : VTableBase {
        _OpaqueDataA __data;  # opaque data area of size sizeof(A)
        
        oper init( ... ) {
            # Call new(__data) A( ... )
        }
        
        oper del() {
            # Call A::~A(__data)
        }
    }
```

The proxy class would also contain virtual, final and static functions
corresponding to the same functions in A.  Static functions can simply use
pointers to the C++ static function.  Final methods can be pointers to wrapper
functions, for example:

```
    void Foo_setVal(Foo *foo, int val) { 
        foo->setVal(val):
    }
```

Virtual functions must be implemented as crack function wrappers that dispatch
to the most specialized implementation  of the function that they wrap, for
example:

```
    void someVirtual() {
        # Call this->A::someVirtual();
    }
```


This implementation effectively allows us to derive from an extension class in
the same way that we can from a Crack class: method calls result in calls to
the underlying methods and virtual methods can be overriden in derived classes.

What we haven't dealt with so far is the ability to call virtual functions
on a derived class from the extension language: attempting to call
A::someVirtual() from within the extension on a Crack object that overrides
someVirtual() will result in A::someVirtual() being called, not the crack
override.

To do this, we need an "adapter class" written on the extension side, in C++.
The adapter class is derived from our target class ("A" in the examples above)
and it implements all virtual functions as calls to the corresponding Crack
vtable entries.

The code to implement the virtual functions for "AdapterA" would be something
like this:

```
    class AdapterA : public A {
        public:
            virtual someVirtual();
    };

    struct ProxyA;

    // A's proxy vtable.  Methods are in the order in which they are defined.
    struct ProxyA_VTable {
        void *classInst;   // the class instance object
        void (*oper_del)(ProxyA *);
        void (*someVirtual)(ProxyA *);
    };
    
    // A's proxy structure - this should directly correspond to the layout 
    // of the "A" proxy class in Crack, but it need not - the position of the 
    // VTable at the beginning of the structure is really all that matters.  
    // "adapter" is primarily documentation.
    struct ProxyA {
        ProxyA_VTable *vtable;
        AdapterA adapter;
    };
    
    void AdapterA::someVirtual() {
        ProxyA *inst = crack::ext::getCrackProxy<ProxyA>(this);
        inst->someVirtual(inst);
    }
```

We implement AdapterA::someVirtual() to call the virtual function in Crack's
vtable, allowing any Crack overrides to be called from C++.

This approach has the unfortunate performance penalty of requiring two levels
of VTable reference.  However, it is portable and relatively efficient as
cross-language bindings go.

It is also necessary to implement all pure virtual functions in the original
class in the same manner as an implemented virtual function: the method can be
flagged as "abstract" when it is defined using the extension interface,
passing the implementation requirement on to the Crack level.