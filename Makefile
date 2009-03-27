# Load the output of the configuration files
ifneq ($(wildcard configure.out),)
include configure.out
else
$(error Please run "./configure" first...)
endif

ifneq ($(LAPACK_HOME),)
LAPACK=$(LAPACK_HOME)/SRC
BLAS=$(LAPACK_HOME)/BLAS/SRC
endif

PACKAGE=org.jblas.la

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

# the default target
all	: compile-native

compile-native : bin/$(LIB)jblas.$(SO)

generate-wrapper: src/$(PACKAGE_PATH)/Blas.java native/Blas.c

clean:
	rm -f native/*.o native/*.$(SO) bin/*.$(SO) src/$(PACKAGE_PATH)/Blas.java

ifeq ($(LAPACK_HOME),)
realclean:
	@echo "Since you don't have LAPACK sources, I cannot rebuild stubs and deleting the cached information is not a good idea."
	@echo "(nothing deleted)"
else
realclean:
	rm -f fortranwrapper.dump
endif

# Generating the stubs. This target requires that the blas sources can be found in ~/src/blas/*.f
src/$(PACKAGE_PATH)/Blas.java native/Blas.c: scripts/fortranwrapper.rb scripts/fortran.rb scripts/fortran/java.rb scripts/java-class.java scripts/java-impl.c
	$(RUBY) scripts/fortranwrapper.rb $(PACKAGE) Blas \
	$(BLAS)/*.f \
	$(LAPACK)/[sd]gesv.f \
	$(LAPACK)/[sd]sysv.f \
	$(LAPACK)/[sd]syev.f \
	$(LAPACK)/[sd]syev[rdx].f \
	$(LAPACK)/[sd]posv.f \
	$(LAPACK)/[sd]geev.f

bin/$(LIB)jblas.$(SO) : native/Blas.$(SO)
	mv $< $@