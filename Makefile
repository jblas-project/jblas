# setting up compilers and flags

ifeq ($(JAVA_HOME),)
$(error JAVA_HOME undefined. Please set JAVA_HOME to your java installation directory.)
endif

ifeq ($(LAPACK_HOME),)
$(error LAPACK_HOME undefined. Please set LAPACK_HOME to the files containg the lapack libraries and blas and lapack sources.)
endif

LAPACK=$(LAPACK_HOME)/SRC
BLAS=$(LAPACK_HOME)/BLAS/SRC

#
# GNU/Linux (actually, debian) settings
#
ifeq ($(shell uname -o),GNU/Linux)
CC=gcc
CFLAGS=-fPIC -ggdb
INCDIRS=-Iinclude -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux
SO=so
LIB=lib
LD=ld
LDFLAGS=-shared -L/usr/lib/atlas
LOADLIBES=-lblas -llapack
RUBY=ruby
#
# cygwin settings
#
else 
ifeq ($(ATLAS_HOME),)
$(error ATLAS_HOME undefined. Please set ATLAS_HOME to the files containg the atlas libraries.)
endif
CC=gcc
CFLAGS=-ggdb -D__int64='long long'
JAVADIR=$(shell cygpath -u $$JAVA_HOME)
ATLASDIR=$(shell cygpath -u $$ATLAS_HOME)
LAPACKDIR=$(shell cygpath -u $$LAPACK_HOME)
INCDIRS=-I"$(JAVADIR)/include/" -I"$(JAVADIR)/include/win32" -Iinclude
LD=gcc
LDFLAGS=-mno-cygwin -shared -Wl,--add-stdcall-alias -L$(ATLASDIR) -L$(LAPACKDIR) 
LOADLIBES=-llapack_WINXP -llapack -lf77blas -lcblas -latlas -lg2c
SO=dll
LIB=
RUBY=ruby
endif

PACKAGE=edu.ida.la

# generate path from package name
PACKAGE_PATH=$(subst .,/,$(PACKAGE))

#
# Pattern rules
#
# The crazy thing is, with these rules, you ONLY need to specify which
# object files belong to a source file the rest is determined
# automatically by make.
#

# rule to compile files
%.o : %.c
	$(CC) $(CFLAGS) $(INCDIRS) -c $< -o $@

# rule to generate shared library from object file 
%.$(SO) : %.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LOADLIBES)

bin/$(LIB)j%Array.$(SO) : native/%.$(SO)
	mv $< $@

# the default target
all	: compileNative

compileNative : bin/$(LIB)jBlasArray.$(SO)

generateWrapper: src/$(PACKAGE_PATH)/Blas.java native/Blas.c

clean:
	rm -f native/*.o native/*.$(SO) bin/*.$(SO) src/$(PACKAGE_PATH)/Blas.java

# Generating the stubs. This target requires that the blas sources can be found in ~/src/blas/*.f
src/$(PACKAGE_PATH)/Blas.java native/Blas.c: scripts/fortranwrapper scripts/fortran.rb scripts/fortran/java.rb scripts/java-class.java scripts/java-impl.c
	$(RUBY) scripts/fortranwrapper $(PACKAGE) Blas \
	$(BLAS)/*.f \
	$(LAPACK)/[sd]gesv.f \
	$(LAPACK)/[sd]sysv.f \
	$(LAPACK)/[sd]syev.f \
	$(LAPACK)/[sd]syev[rdx].f \
	$(LAPACK)/[sd]posv.f \
	$(LAPACK)/[sd]geev.f

bin/$(LIB)Blas.$(SO) : native/Blas.o