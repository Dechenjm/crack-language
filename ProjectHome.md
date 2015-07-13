# The Crack Programming Language #

Crack aims to provide the ease of development of a scripting language with the performance of a compiled language.  The "crack" program is a "script executor" that compiles source
to machine code on the fly (it will eventually cache the code to intermediate formats as appropriate).  It can also compile a script to a native binary.

The crack language itself derives concepts from C++, Java and Python, incorporating object-oriented programming, operator overloading and strong typing.

Crack is still evolving.  For minor version changes there is currently no guarantee of compatibility between versions, this situation will change when we get to 1.0, at which point backwards compatibility will be guaranteed until the next major version change.  In the meantime, tertiary version changes should be backwards compatible, and some features may be added to the executor to help users during a version upgrade.

Crack is released under the Mozilla Public License, version 2.0

## News ##

  * <a href='http://crack-lang.blogspot.com/2015/03/crack-010-released.html'>Crack 0.10 Released</a>
  * <a href='http://crack-lang.blogspot.com/2015/02/i-just-pushed-change-that-fixes-long.html'>Fixing Interfaces</a>
  * <a href='http://crack-lang.blogspot.com/2014/08/fast.html'>Fast</a>
  * <a href='http://crack-lang.blogspot.com/2014/02/crack-09-released.html'>Crack 0.9 Released</a>
  * <a href='http://crack-lang.blogspot.com/2014/01/threads.html'>Threads</a>
  * <a href='http://crack-lang.blogspot.com/2013/10/crack-08-released.html'>Crack 0.8 Released</a>
  * <a href='http://crack-lang.blogspot.com/2013/09/annotation-semantics-changing-for.html'>Annotation Semantics Changing for Generics</a>
  * <a href='http://crack-lang.blogspot.com/2012/10/crack-071-released.html'>Crack 0.7.1 Released</a>
  * <a href='http://crack-lang.blogspot.com/2012/09/crack-07-released.html'>Crack 0.7 Released</a>
  * <a href='http://crack-lang.blogspot.com/2011/12/crack-06-released.html'>Crack 0.6 Released</a>
  * <a href='http://crack-lang.blogspot.com/2011/08/screen-new-test-suite-for-crack.html'>screen: a new test suite for crack</a>
  * <a href='http://crack-lang.blogspot.com/2011/07/crack-05-released.html'>Crack 0.5 Released</a>
  * <a href='http://crack-lang.blogspot.com/2011/05/crack-04-released.html'>Crack 0.4 Released</a>
  * <a href='http://crack-lang.blogspot.com/2011/04/exciting-new-changes.html'>Exciting New Changes</a>
  * <a href='http://crack-lang.blogspot.com/2011/03/platform-dependent-versus-universal.html'>Platform Dependent Versus Universal Numeric Types</a>
  * <a href='http://crack-lang.blogspot.com/2011/03/aot-for-crack-04.html'>AOT for Crack 0.4</a>
  * <a href='http://crack-lang.blogspot.com/2011/01/crack-03-released.html'>Crack 0.3 released</a>
  * <a href='http://crack-lang.blogspot.com/2010/12/annotations.html'>Annotations</a>
  * <a href='http://crack-lang.blogspot.com/2010/11/extension-api.html'>Extension API</a>
  * <a href='http://crack-lang.blogspot.com/2010/10/crack-021-released.html#comment-form'>2 Comments</a>
  * <a href='http://crack-lang.blogspot.com/2010/10/crack-021-released.html'>Crack 0.2.1 Released</a>
  * <a href='http://crack-lang.blogspot.com/2010/10/crack-02-released_03.html'>Crack 0.2 Released</a>


## Get Crack! ##

You'll need: [LLVM 3.3](http://llvm.org), and [Crack 0.10](http://www.crack-lang.org/downloads/crack-0.10.tar.gz).

Follow the instructions in the INSTALL file in the crack tarball.

## Documentation ##

See [The Manual](http://www.mindhog.net/~mmuller/projects/crack/Manual-0.10.html).

Contributors should also see [The Style Guide](http://www.mindhog.net/~mmuller/projects/crack/Style.html)

See also [the notes](http://code.google.com/p/crack-language/source/browse/notes.txt) for lots of disorganized thoughts on what the language will be like.

## Installing from the Source Repository ##

You probably don't want to do this unless you want to do some development on Crack:  most users will want to download the tarballs as identified above.

Check the "Source" folder for information on how to pull from the CVS repositories.  You'll need both the spug++ and crack repositories.

  1. Get the latest crack code:
```
  hg clone https://crack-language.googlecode.com/hg/ crack-language
```
  1. Build and install [LLVM Version 3.3](http://www.llvm.org)
```
  tar -xvzf llvm-3.3.src.tar.gz
  cd llvm-3.3.src
  ./configure --enable-assertions
  make REQUIRES_RTTI=1
  su -c make install
```
> > Add the options `--disable-optimized --enable-debug-runtime` during configure if you want to be able to debug LLVM, but be advised that this will significantly decrease performance and increase binary size.  Avoid it unless you need to debug something through to the LLVM code.
  1. Build and install Crack:
```
  cd crack-language; ./bootstrap; ./configure; make; make check; sudo make install
```
  1. try it out:
```
  echo 'import crack.io cout; cout `hello world!\n`;' | crack -l lib - 
```

## Contact Info ##

For questions, send mail to crack-lang-dev@googlegroups.com or stop by the IRC channel on FreeNode: #crack-lang.

## Sample Code ##

This script builds a set of unique values from the argument list and prints its contents.

```
# import what we need from the appropriate modules
import crack.io cout, Formatter;
import crack.sys argv;
import crack.strutil StringArray;
import crack.cont.treemap TreeMap;

## Cheesey set implementation.
class ArgSet {
    
    # set of all args
    TreeMap[String, int] __raw = {};
    
    ## initialize the set from the arg list.
    oper init(StringArray args) {

        # transfer args from the array to the set
        uint i = 1;
        while (i < args.count()) {
            __raw[args[i++]] = 1;
        }
    }

    ## Dump the set
    void dump() {
        for (item :in __raw)
            cout `got $(item.key)\n`;
    }
}

# initialize the arg set from the actual args
ArgSet a = {argv};

# write the set
a.dump();
```