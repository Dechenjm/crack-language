# Introduction #

This page provides an introduction to Crack from a student's perspective and a source of project ideas for those who want to try their hands at the _exciting_ world of open source compiler writing and language development.

Crack is a "new" programming language that aims to have the feel of a scripting language but the speed of a compiled language. It's new in the sense that the source code is not based on any previous project, but the basis for much of the syntax, semantics, runtime library API, and other aspects are all inspired by existing languages. This means we intend for anyone who knows some (for example) C/C++, Java, PHP or Python to pick up Crack very quickly. It's also designed to interact well with existing C/C++ code.

Crack does diverge from most scripting languages in that it is statically typed and it is always compiled to the native instruction set before execution (JIT or AOT). It also offers advanced features like annotations (crack code run at compile time that modifies the compile), zero-cost exceptions, generics and first class functions. There's a video of an introductory talk that Michael Muller gave on Crack at the 2010 LLVM Developers Meeting [available here](http://www.llvm.org/devmtg/2010-11/).

Crack is already fully functional, but it's still missing a few core language constructs and features we'd like to see, and the runtime library is small and immature.

We're currently a small development team and this is just the start of an ambitious project. If you work with us, you'll be working directly with the core developers on IRC and email.

BTW, if you're not yet committed to studying compiler development during your academic career, keep in mind what the [Computing Science Curriculum 2008 Report](http://www.acm.org/education/curricula/ComputerScience2008.pdf) noted: "... Although many companies do not engage in anything related to compilers, compiler writing tended to be seen as a microcosm for realistic software development. So good compiler writers are often seen as desirable; they tend to be good software engineers"

The project list below is organized into a few basic areas and contains ideas that we'd love to see real implementations for. Don't let this list box you in, though! We'd be happy to entertain other ideas. Drop by #crack-lang on Freenode, or mail us at out our [mailing list](http://groups.google.com/group/crack-lang-dev?pli=1).

The current [Issues list](http://code.google.com/p/crack-language/issues/list) might give you some ideas, too.

## Skillset ##

Crack is written in C++ and developed mainly on Linux (although porting is one of the project ideas!). We depend on [LLVM](http://llvm.org) but no other software libraries (other than STL).

## Language Implementation Ideas ##

This is where you can jump in and get your hands dirty with the core compiler. These will generally involve strong C++ coding skills and coming to terms with the guts of the parser and code generator. Also involves learning LLVM concepts and API.

  * Implement some planned core language constructs: !is, enums. These will be similar to constructs that exist in other languages.

  * Propose a novel idea for the language, perhaps based on your past programming experience. The language feature you've always wanted.

  * Integrate large numeric constants with "bignum" extension idea (in runtime section). Currently, the parser will truncate a large constant. We'd like to capture the full value and create a Bignum object automatically.

  * Port to another platform: OSX, Windows, Non-Linux Unix.

  * Advanced projects: Virtual inheritance

  * Investigate the [Layers](Ideas.md) idea

  * Prototype a concurrency model, for compile time or run time.

  * Implement a new backend (the builder::Builder interface).  Desired backends include:
    * portable bytecode interpreter.
    * direct-to-x86 JIT.
    * Dalvik VM
    * Javascript

## Crack Runtime Ideas ##

The Crack runtime library really consists of two parts: the low-level C++ extension that hides some platform specific details away ("crack's libc"), and the (much larger) higher level interfaces written in crack (in the lib/crack directory). In general, we prefer to write as much as possible in crack rather than C++.

Therefore, these ideas mostly require writing in the crack language itself and using it to further the supporting runtime libraries available to crack programmers. But they may also involve C++ which is used to write extensions that connect to 3rd party C/C++ libraries.

This may be a good opportunity to implement some of those algorithms and data structures you've learned about in crack, and have it actually be used by future crack programmers!


  * Crack Library: Implement a native distributed object/RPC mechanism.
  * Extension: Use the new GObject introspection functionality to create bindings to GObject, Gtk+ and related libraries
  * Extension: Implement a "Bignum" class using the [GNU MP](http://gmplib.org/) library
  * Extension: Implement a binding to libICU for unicode support
  * Extension: Pick a popular 3rd party library (e.g. Mysql) and make an extension for it
  * Extension: Extend the existing SDL/OpenGL extension and create a full functional game in crack

## Embedded / IDE /Development Related ##

This involves code to support crack being used in an embedded environment, such as from with an IDE.

  * Adapt an editor or IDE to do syntax highlighting and compiling etc. of the crack language. Ideally this will be done by expanding the interface in LibCrackLang so that an IDE can use it to tokenize and execute crack code directly. Bonus points for highlighting syntax errors in real time or other goodies.

  * Write an IDE for crack, in crack.

  * Better (command line) diagnostics output, including listing syntax highlighted source context, with pointer to error a la clang

  * Write or adapt an existing documentation generation system similar to Doxygen, Sphinx (Python) or Rdoc/YARD/etc. (Ruby) to generate documentation from Crack source code.