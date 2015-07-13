# Functions and Functors #

**This is a rough draft, subject to change.**

Given the following function:

```

void f(int x) {}

```

The type of `f` is currently voidptr, but it should be `function[void, int]`:

```
   function[void, int] funcPtr = f;
```

"function" needs to be a built-in primitive pointer type, similar to array.
Extension functions that accept pointers to functions should be defined to
accept "function" types, for example:

```
   void setMyCallback(function[bool, Event] callback);
```

## Functors ##

Functors are objects that can be called like functions.  We define a functor
as follows:

```
   class MyCallback {
      bool oper call(Event event) { ... }
   }
```

For every "oper call" method in the class, there should be an implicit "oper
to Functor" method allowing us to convert the object an instance of the
special class Functor with the corresponding argument list.

So in this case, we should be able to do this:

```
   Functor[bool, Event] f = MyCallback();
```

This gets ugly because what actually has to happen here is we have to create
an instance of something derived from Functor[bool, Event] object that
references the original object and calls the other method on it.  It would be
really nice to do magic here.

As the name and usage implies, Functor is an aggregate generic type.  Sadly,
it relies on the presence of variadic generic arguments which isn't designed
yet.  It should be defined in lang.crk something like this:

```
   class Functor[RetType, *Args] : Object {
      RetType oper call(*Args) { raise NotImplementedError(); }
   }
```

The class generated for the example above would be something like this:

```
   class <Anonymous> : Functor[bool, Event] {
      MyCallback __impl;
      oper init(MyCallback impl) : __impl = impl {}
      bool oper call(Event arg0) {
         return __impl.oper call(argg0);
      }
   }
```

We should be able to avoid the penalty of double-virtual function invocation
by inheriting directly from Functor:

```
   class MyCallback : Functor[bool, Event] {
      bool oper call(Event evt) { ... }
```

In the interests of flexibility, Functor should probably be an interface
instead of a plain class.  With this combination of features, it is questionable whether the implicit conversion approach described above adds any value.

**Note:** how do we deal with keyword arguments in functors?

## Interoperability between function and functor ##

Functions and functors cannot have the same implementation.  A function has to
correspond to the low-level C-style implementation of a raw function pointer.
A functor has to have data associated with it, so you end up needing two
pointers: one essentially a raw function pointer, one the implicit "this"
argument.

We can skimp on this somewhat and use a single pointer to an object with a
vtable - the vtable gives us the pointer to the function.

For this reason, users should generally only use "function" when dealing with
an extension.  In all other cases they should use Functor.  We can convert
from a function to a functor with an implicit "oper to" by defining an adapter
class similar to that used for normal functor classes, for example:

```
   class <Anonymous> : Functor[void, int] {
      function[void, int] __f;
      oper init(function[void, int] f) : __f = f {}
      void oper call(int i) {
         __f(i);
      }
   }
```