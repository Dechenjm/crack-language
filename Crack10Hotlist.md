
```
-   Fix EXTRA_DIST in Makefile.am.in so that we only distribute stuff we need.
    avoid .orig files, .o, .lo, .so (except test/bad_extension.so), binary
    files in tools/ and test/, Manual.html, Style.html, test/.deps, test/.libs,
    test/.dirstamp
-   ensure that all objects in meta-data files have an "optional fields" blob
    so we can extend them.
-   If we get a failure during read of the post-header metadata or builder
    cachefile, we should probably delete the cachefiles that caused the failure.
-   Try uncommenting the code in JitBuilder that deletes a materialized module
    after merging it.
    -   still exposes a bug.  I think this is my original write-up on it:

            possibly related to the test_numericarray_multi problem, when we
            delete old modules after a merge we get LLVM errors complaining
            that one of the base class arrays is an unknown constant.  You
            can see this by simply instantiating crack.cont.array.Array (with
            code that deletes after the merge for cached modules.

-   With "-t Caching", we don't get any information about generic
    ephemeral modules.  In test_generic_caching, run with -t
    Serializer,Caching and look at the trace for module
    testmod.G[.builtin.int].G.  It gets serialized and deserialized but
    there's no caching trace for it.
-   "g := f" for an "f" that is an imported function should break because
    even if there's only one overload of "f", it would restrict the ability of
    the module owner to add another overload. (see Overloads.txt)
-   In generics, stop serializing the filename with every token.
-   consistency notes:
    -   see if we've got NumericArray.clear()
    -   containers should all have:
        clear() method which deletes all.
    -   should we do a cleanup of @final?  Method should be final if it
        doesn't make sense to override it given the implementation.
        We can always remove @final.  Currently thinking that all container
        methods that aren't overrides of Object virtuals should be final.
-   In doing code cleanups, I got as far as crack/cont/array.crk.  Do the
    other modules, too.
-   We can define a class as @abstract even if it doesn't have a VTable.
    Should give an error.
-   need to implement 'byteptr - int'
    -   byteptr oper !
-   second order imports are not prohibited correctly for overloads (we should:
    -   only import those overloads that are defined in the imported module
    -   fail if the function is only a second-order unless it is exported
    -   import both first and second order if the symbol is exported.
-   When loading a broken cachefile, clean up and retry with compile.
-   add cache_tests/xx* tests to the framework, these test specific
    scenarios that are problematic for caching.  More tests:
    - verify that we can recompile against meta-data files containing
      secondary imports.
    - verify that we get correct dependencies from all cases including
      transitively from members of a type
    - against precompiled test.testmod, compile:
        import test.testmod aImportVar; aImportVar.dump();
    - test importing constants from a cached module.
        mod: const int i = 100; const int f = 1.1;
        prog: import mod i, f; printint(i); printint(f * 10);
        This is another case where you need to compile the program against a
        precompiled module.
    - make sure we test deserializing class A { array[A] a; }, need to make
        sure that a class name is registered prior to generic lookup.
    - Verify that we can use a base class from another module that is already
        compiled in a module that is not yet compiled.
-   Right now "oper class" gets created in LLVMBuilder::emitBeginClass().  I
    don't think it should.  If we change this, we're changing the Builder
    interface and we need to do that before 1.0!
-   copyright stuff
    -   review files in README with no obvious license, track down their
    -   add copyright notices for crkt files.
        packages and determine their licensing.
    -   long term changes for the script:
        -   need to exclude considering change bbe08ed67c8d (automatic
            application of copyrights) as a Google owned change.

-   cleaning up exceptions
    -   I think we need a wrapper object to store in the thread-local variable
        that indicates whether this is a C++ exception or a crack exception.
        We can also use that to store crack stack trace info for a C++
        exception!
        -   for a C++ exception, how will we clean it up?
    -   if the module has no rinit/cinit funcs, we get a seg-fault because the
        module pointer is null
    -   test throwing an exception from a Crack destructor called while
        processing a C++ exception.
-   Bug:
    [/home/mmuller/w/crack.new/screen/tests/compound/014_cache_collisions.crkt]: cache files are immune to write collisions
    JIT: FAIL 
    ---- Diff ----
    Expected: [ok
    ]
    Actual: [Bad output from 4: '0'
    ok
    ]
    --------------
-   We need to generate an error if we generate non-binding/releasing uses of 
    a class and then bind/release are subsequently defined:
    
    This lets us break reference counting:
    class A { 
        oper init() {} 
        A f() { return this; } 
        oper bind() { puts("ok"); } 
    } 
    
    A().f()
    
-   It looks like if in an extension, you define a variable with the same name
    as a method in a type it can cause an internal exception.

-   when distributing:
    -   create a clean distro repo, _every time_
    -   run ./bootstrap; mkdir build; cd build; ../configure; make -j5 distcheck
```