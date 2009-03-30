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

#
# For testing
#
VERSION=0.1

make test-dist:
	ant clean tar
	rm -rf jblas-$(VERSION)
	tar xzvf jblas-$(VERSION).tgz
	cd jblas-$(VERSION)
	./configure
	ant clean jar
	cd ..
