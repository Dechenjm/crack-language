# The Extension Module Interface

# The Module Extension Interface #

Although it is possible for crack code to directly load shared libraries and declare functions to call in them, this approach is not very portable:

  * Shared library naming conventions differ on different operation systems.
  * C functionality is often implemented as macros, which are not available from a shared library.
  * Name mangling standards can vary across different C++ compilers.

Because of this, a language binding standard is highly desirable.  Therefore, Crack will provide an "extension API" (an API for extending crack code with C and C++ code).

When importing a module, if the crack executor discovers a shared library on the module search path with a name matching that of the module, it will attempt to load it and resolve an "init" function.  The init function should be the _mangled form_ of the module name + ':init' (see CanonicalNames).  (**Note:** since we don't yet have name mangling implemented, the initial implementation of the extension API will just use a simple, underscore-separated name, so the module "foo.bar" would have an init function with the name "foo\_bar\_init".

The module init function will be allowed to interact with the executor by defining functions, classes, and variables.

## Proposed C++ API ##

The prototype of the module init function in C++ will accept an argument of type **Module `*`**:

```
    #include <crack/ext/Module.h>

    void foo_bar_init(Module *mod) {
        // this module contains nothing
    }
```

### Getting built-in types ###

The Module class will contain convenience methods for accessing built-in types:

```
    Type *int32Type = mod->getInt32Type();
    Type *objectType = mod->getObjectType();
```

The **Type** class is a wrapper class that hides objects internal to the compiler.  Once created, these wrappers are cached by the Module object.  Do not try to delete them or store them somewhere external to the init function - they will be deleted by the **Module** object after the module is loaded.

### Defining Functions ###

```

    void myfunc(char *data) { printf("my data: %s", data); }

    // define a function returning void
    Func *func = mod->addFunc(mod->getVoidType(), "myfunc", (void *)myfunc);
    // add the "data" argument, parameters are the type and the parameter name
    func->addArg(mod->getByteptrType(), "data");
```

XXX to be continued XXX

## Proposed C API ##

The C API will be a simple wrapper around the C++ API.  By example:

```
   void foo_bar_init(Module *mod) {
       Func *func = Module_addFunc(Module_getVoidType(mod), "myfunc", (void *)myfunc);
       Func_addArg(func, Module_getByteptrType(mod), Type(mod), "data");  
```