# The Module Extension Interface #

Although it is possible for crack code to directly load shared libraries and declare functions to call in them, this approach is not very portable:

  * Shared library naming conventions differ on different operation systems.
  * C functionality is often implemented as macros, which are not available from a shared library.
  * Name mangling standards can vary across different C++ compilers.

Because of this, a language binding standard is highly desirable.  Therefore, Crack provides an "extension API" (an API for extending crack code with C and C++ code).  Currently only C++ bindings are implemented.

When importing a module, if the crack executor discovers a shared library on the module search path with a name matching that of the module, it will attempt to load it and resolve an "init" function.  The init function should be the _mangled form_ of the module name + ':init' (see CanonicalNames).  (**Note:** since we don't yet have name mangling implemented, the initial implementation of the extension API will just use a simple, underscore-separated name, so the module "foo.bar" would have an init function with the name "foo\_bar\_init".

The module init function will be allowed to interact with the executor by defining functions, classes, and variables.

## C++ API ##

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

At this time, there is no way to get a type by its name.

### Defining Functions ###

```

    void myfunc(char *data) { printf("my data: %s", data); }

    // define a function returning void
    Func *func = mod->addFunc(mod->getVoidType(), "myfunc", (void *)myfunc);
    // add the "data" argument, parameters are the type and the parameter name
    func->addArg(mod->getByteptrType(), "data");
```

`addFunc()` has an optional fourth argument to allow you to pass in the actual symbol name used by the compiler - this should be used in cases where automatic symbol name lookup fails:

```

    Func *func = mod->addFunc(mod->getVoidType(), "myfunc", (void *)myfunc, "myfunc");
```



### Defining New Types ###

The addType() method allows you to add a new type to an extension module:

```
    Type *myType = mod->addType("MyType");
    myType->finish(); // this is only necessary if you want to use the type in
                      // subsequent definitions, otherwise the type will be finished
                      // for you.
```

You can add static and normal methods to the new type:

```

    class MyType {
        public:
            static MyType *oper_new() {
                return new MyType();
            }

        const char *echo(const char *data) { return data; }
    };
    

    Type *myType = mod->addType("MyType");
    myType->addStaticMethod(myType, "oper new", (void *)MyType::oper_new);
    myType->addMethod(myType, "echo", (void *)&MyType::echo);
    myType->finish();
```

You can also define base classes:

```

    myType->addBase(mod->getObjectType());
```

You can add member variables to your classes as follows:

```
    myType->addInstVar(mod->getIntType(), "a");
```

This allows you to use native structures that mirror your Crack types _to a limited extent._  Padding issues may result in incompatibilities.  We may end up adding the variable offset as a parameter to `addInstVar()` or we may need to provide some special macros on the native side to insure matching alignment.

For now, using instance variables to try to create a common structure definition should be used very cautiously.

## Proposed C API ##

The C API will be a simple wrapper around the C++ API.  By example:

```
   void foo_bar_init(Module *mod) {
       Func *func = Module_addFunc(Module_getVoidType(mod), "myfunc", (void *)myfunc);
       Func_addArg(func, Module_getByteptrType(mod), Type(mod), "data");  
```