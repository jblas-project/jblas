## --- BEGIN LICENSE BLOCK ---
# Copyright (c) 2009, Mikio L. Braun
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
# 
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
# 
#     * Neither the name of the Technische Universität Berlin nor the
#       names of its contributors may be used to endorse or promote
#       products derived from this software without specific prior
#       written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
## --- END LICENSE BLOCK ---

######################################################################
#
# Load the output of the configuration files
#
ifneq ($(wildcard configure.out),)
include configure.out
else
$(error Please run "./configure" first...)
endif

ifneq ($(LAPACK_HOME),)
LAPACK=$(LAPACK_HOME)/SRC
BLAS=$(LAPACK_HOME)/BLAS/SRC
endif

PACKAGE=org.jblas

# generate path from package name
PACKAGE_PATH=$(subst .,/,$(PACKAGE))

LIB_PATH=native-libs/$(LINKAGE)/$(OS_NAME)/$(OS_ARCH)

#######################################################################
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

#
#
#

# The default target
all	: prepare generate-wrapper compile-native

prepare :
	test -d native || mkdir native

# Generate the JNI dynamic link library
compile-native : $(LIB_PATH)/$(LIB)jblas.$(SO)

# Generate the code for the wrapper (both Java and C)
generate-wrapper: src/$(PACKAGE_PATH)/NativeBlas.java native/NativeBlas.c
	ant javah

# Clean all object files
clean:
	rm -f native/*.o native/*.$(SO) $(LIB_PATH)/*.$(SO) src/$(PACKAGE_PATH)/NativeBlas.java

# Also remove the code extracted from the fortranwrappers.
ifeq ($(LAPACK_HOME),)
realclean:
	@echo "Since you don't have LAPACK sources, I cannot rebuild stubs and deleting the cached information is not a good idea."
	@echo "(nothing deleted)"
else
realclean:
	rm -f fortranwrapper.dump
endif

# Generating the stubs. This target requires that the blas sources can be found in ~/src/blas/*.f
src/$(PACKAGE_PATH)/NativeBlas.java native/NativeBlas.c: scripts/fortranwrapper.rb scripts/fortran/types.rb scripts/fortran/java.rb scripts/java-class.java scripts/java-impl.c
	$(RUBY) scripts/fortranwrapper.rb $(PACKAGE) NativeBlas \
	$(BLAS)/*.f \
	$(LAPACK)/[sd]gesv.f \
	$(LAPACK)/[sd]sysv.f \
	$(LAPACK)/[sd]syev.f \
	$(LAPACK)/[sd]syev[rdx].f \
	$(LAPACK)/[sd]posv.f \
	$(LAPACK)/[sdcz]geev.f \
	$(LAPACK)/[sd]getrf.f \
    $(LAPACK)/[sd]potrf.f 

$(LIB_PATH)/$(LIB)jblas.$(SO) : native/NativeBlas.$(SO)
	mkdir -p $(LIB_PATH)
	mv $< $@

#
# For testing
#
VERSION=0.3.1

test-dist:
	ant clean tar
	rm -rf jblas-$(VERSION)
	tar xzvf jblas-$(VERSION).tgz
	cd jblas-$(VERSION)
	./configure
	ant clean jar
	java -cp jblas-$(VERSION).jar org.jblas.util.SanityChecks
	cd ..

#
# Building different kinds of jar files
#
all-jars:
	ant clean-jars
	./configure --keep-options $$(cat configure.options)
	ant jar 
	ant lean-jar
	./configure --keep-options --static-libs $$(cat configure.options)
	ant compile-native static-jar fat-jar

all-static-jars:
	./configure --keep-options --static-libs $$(cat configure.options)
	ant compile-native
	for os_name in native-libs/*; do \
	  for os_arch in $$os_name/* ; do \
	    ant static-jar -Dos_name=$$(basename $$os_name) \
		-Dos_arch=$$(basename $$os_arch); \
	  done; \
	done
