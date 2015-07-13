# Introduction #
This is a makefile with some useful rules to compile and install Crack sourcode. The example is for a project called **dawn**.

# Project directory structure #

For the purposes of this makefile, the main project directory has two subdirectories:
  * **test**
  * **dawn**


# Makefile targets #
  * **all** default target to build all targets
  * **tests** builds all test binaries
  * **install** installs the module source code under **PREFIX/dawn** in the system path so that the _crack_ and _crackc_ compilers can use the code.
> > The default install prefix is determined from the directory where the _crack_ and _crackc_ binaries are installed.
> > If the prefix is /usr/local and the Crack version is _0.7_, the module directory will be **/usr/local/lib/crack-0.7/dawn**.
> > That way the module code can be imported using
`import dawn.scgi SCGIServer`.

The makefile also includes an implicit rule telling make how to compile **.crk** files.

# Makefile #
```
ifndef CRACKC
  CRACKC=crackc
 endif

ifndef PREFIX
  PREFIX=$(shell ldd `which crack` | grep -e 'libCrackLang'| sed  's/.*libCrackLang.* => \(.*\)\/lib\/libCrackLang.*/\1/g')
endif

VERSION=$(lastword $(shell crack --version))

INSTALLDIR=${PREFIX}/lib/crack-${VERSION}/dawn


all: test/test_scgi

tests: test/test_scgi

test/test_scgi: test/test_scgi dawn/scgi.crk


% : %.crk
	$(CRACKC) -l $(PREFIX)/lib $<


install:
	mkdir -p ${INSTALLDIR}
	install -C -D -d dawn ${INSTALLDIR}

clean:
	rm -fv $(tests) test/*.o test/*~ dawn/*.o

```