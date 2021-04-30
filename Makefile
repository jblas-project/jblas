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
#     * Neither the name of the Technische Universitaet Berlin nor the
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

VERSION=1.2.5

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

SRC=src/main
SRC_JAVA=$(SRC)/java
SRC_C=$(SRC)/c
RESOURCES=$(SRC)/resources

JAVAC=javac

TARGET_C=target/c

LIB_PATH=$(RESOURCES)/lib/$(LINKAGE_TYPE)/$(OS_NAME)/$(OS_ARCH)
FULL_LIB_PATH=$(RESOURCES)/lib/$(LINKAGE_TYPE)/$(OS_NAME)/$(OS_ARCH_WITH_FLAVOR)

GENERATED_SOURCES=$(SRC_JAVA)/$(PACKAGE_PATH)/NativeBlas.java $(SRC_C)/NativeBlas.c
GENERATED_HEADERS=$(SRC_C)/org_jblas_NativeBlas.h $(SRC_C)/org_jblas_util_ArchFlavor.h
SHARED_LIBS=$(FULL_LIB_PATH)/$(LIB)jblas.$(SO) $(LIB_PATH)/$(LIB)jblas_arch_flavor.$(SO) 

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

######################################################################
#
# Main section
#

# The default target
all	:  $(SHARED_LIBS)

#
# main library jblas (from NativeBlas)
#
$(FULL_LIB_PATH)/$(LIB)jblas.$(SO) : $(TARGET_C)/NativeBlas.$(SO)
	@mkdir -p "$(dir $@)"
	mv "$<" "$@"

$(TARGET_C)/NativeBlas.o: $(SRC_C)/NativeBlas.c
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) $(INCDIRS) -c $(SRC_C)/NativeBlas.c -o $@

$(SRC_C)/NativeBlas.c: generated-sources


#
# Arch Flavor
#
$(LIB_PATH)/$(LIB)jblas_arch_flavor.$(SO): $(SRC_C)/jblas_arch_flavor.$(SO)
	@mkdir -p "$(dir $@)"
	mv "$<" "$@"

$(TARGET_C)/jblas_arch_flavor.o: generated-sources
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS) $(INCDIRS) -c $(SRC_C)/jblas_arch_flavor.c -o $@


#
# Generating JNI files and headers
#

# Generate the code for the wrapper (both Java and C)
generate-wrapper: $(GENERATED_SOURCES) $(GENERATED_HEADERS)


# Generating the stubs. This target requires that the blas sources can
# be found in the $(BLAS) and $(LAPACK) directories.
generated-sources: \
  scripts/fortranwrapper.rb scripts/fortran/types.rb \
  scripts/fortran/java.rb scripts/java-class.java scripts/java-impl.c \
  $(SRC_JAVA)/org/jblas/util/ArchFlavor.java #src/org/jblas/NativeBlas.java
	$(RUBY) scripts/fortranwrapper.rb --complexcc $(CCC) $(PACKAGE) NativeBlas \
	$(BLAS)/[sdcz]copy.f \
	$(BLAS)/[sdcz]swap.f \
	$(BLAS)/[sdcz]axpy.f \
	$(BLAS)/[sdcz]scal.f \
  $(BLAS)/[cz][sd]scal.f \
	$(BLAS)/[sdcz]dot*.f \
	$(BLAS)/[sd]*nrm2.f \
	$(BLAS)/[sd]*asum.f \
	$(BLAS)/i[sdcz]amax.f \
	$(BLAS)/[sdcz]gemv.f \
	$(BLAS)/[sdcz]ger*.f \
	$(BLAS)/[sdcz]gemm.f \
	$(LAPACK)/[sd]gesv.f \
	$(LAPACK)/[sd]sysv.f \
	$(LAPACK)/[sd]syev.f \
	$(LAPACK)/[sd]syev[rdx].f \
	$(LAPACK)/[sd]posv.f \
	$(LAPACK)/[sdcz]geev.f \
	$(LAPACK)/[sd]getrf.f \
	$(LAPACK)/[sd]potrf.f \
	$(LAPACK)/[sdcz]gesvd.f \
	$(LAPACK)/[sd]sygvd.f \
	$(LAPACK)/[sd]gelsd.f \
	$(LAPACK)/ilaenv.f \
	$(LAPACK)/[sd]geqrf.f $(LAPACK)/[sd]ormqr.f \
	$(LAPACK)/[sd]orgqr.f \
	$(LAPACK)/[sd]sygvx.f
	$(JAVAC) -classpath $(SRC_JAVA) $(SRC_JAVA)/org/jblas/util/ArchFlavor.java -h $(SRC_C)
	$(JAVAC) -classpath $(SRC_JAVA) $(SRC_JAVA)/org/jblas/NativeBlas.java -h $(SRC_C)
	touch $@


####################################################################################
#
# Cleaning
#

# Clean all object files
clean:
	rm -f native/*.o native/*.$(SO) \
	 $(LIB_PATH)/$(LIB)jblas.$(SO) $(LIB_PATH)/$(LIB)jblas_arch_flavor.$(SO) \
	 $(FULL_LIB_PATH)/$(LIB)jblas.$(SO) $(FULL_LIB_PATH)/$(LIB)jblas_arch_flavor.$(SO) \
	 $(SRC_JAVA)/$(PACKAGE_PATH)/NativeBlas.java $(SRC_C)/NativeBlas.c generated-sources

# Full clean, including information extracted from the fortranwrappers.
# You will need the original fortran sources in order to rebuild
# the wrappers.
ifeq ($(LAPACK_HOME),)
realclean: clean
	@echo "Since you don't have LAPACK sources, I cannot rebuild stubs and deleting the cached information is not a good idea."
	@echo "(nothing deleted)"
else
realclean: clean
	rm -f fortranwrapper.dump
endif



######################################################################
#
# Testing etc.
#

# run org.jblas.util.SanityChecks
sanity-checks:
	java -cp jblas-$(VERSION).jar org.jblas.util.SanityChecks

# Create a tar, extract in a directory, and rebuild from scratch.
test-dist:
	make clean all
	ant clean tar
	rm -rf jblas-$(VERSION)
	tar xzvf jblas-$(VERSION).tgz
	(cd jblas-$(VERSION); ./configure; make -j3; ant jar; LD_LIBRARY_PATH=$(FULL_LIB_PATH):$(LIB_PATH) java -cp jblas-$(VERSION).jar org.jblas.util.SanityChecks)

################################################################################
#
# Docker development
#

#------------------------------------------------
# x86_64 / amd64
#
# Compile with 18.04, 20.04 for testing


#
# Ubuntu 18.04
#
shell-ubuntu1804:
	docker build -f docker/dev-ubuntu1804.Dockerfile -t jblas/dev-ubuntu1804 .
	docker run -it --rm -v $(PWD):/home/dev jblas/dev-ubuntu1804 /bin/bash

test-ubuntu1804:
	docker build -f docker/test-ubuntu1804.Dockerfile -t jblas/test-ubuntu1804 .
	docker run --rm jblas/test-ubuntu1804 /bin/bash


#
# Ubuntu 20.04: Does not work on Ubuntu 18.04 if you use these!
#
shell-ubuntu2004:
	docker build -f docker/dev-ubuntu2004.Dockerfile -t jblas/dev-ubuntu2004 .
	docker run -it --rm -v $(PWD):/home/dev jblas/dev-ubuntu2004 /bin/bash

test-ubuntu2004:
	docker build -f docker/test-ubuntu2004.Dockerfile -t jblas/test-ubuntu2004 .
	docker run --rm jblas/test-ubuntu2004 /bin/bash

shell-debian-testing:
	docker build -f docker/dev-debian-testing.Dockerfile -t jblas/dev-debian-testing .
	docker run -it --rm -v $(PWD):/home/dev jblas/dev-debian-testing /bin/bash


#----------------------------------------------------------
# aarch64 / arm64
#
# Compile with 20.04, use 18.04 for testing (other way round!)

shell-ubuntu1804-arm64:
	docker build -f docker/dev-ubuntu1804-arm64.Dockerfile -t jblas/dev-ubuntu1804-arm64 .
	docker run -it --rm -v $(PWD):/home/dev jblas/dev-ubuntu1804-arm64 /bin/bash

test-ubuntu1804-arm64:
	docker build -f docker/test-ubuntu1804-arm64.Dockerfile -t jblas/test-ubuntu1804-arm64 .
	docker run --rm jblas/test-ubuntu1804-arm64 /bin/bash

shell-ubuntu2004-arm64:
	docker build -f docker/dev-ubuntu2004-arm64.Dockerfile -t jblas/dev-ubuntu2004-arm64 .
	docker run -it --rm -v $(PWD):/home/dev jblas/dev-ubuntu2004-arm64 /bin/bash

test-ubuntu2004-arm64:
	docker build -f docker/test-ubuntu2004-arm64.Dockerfile -t jblas/test-ubuntu2004-arm64 .
	docker run --rm jblas/test-ubuntu2004-arm64 /bin/bash

#
# meta build target.
#
build:
	./configure
	make clean all
	mvn package -Dmaven.test.skip=True
