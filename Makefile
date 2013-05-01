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

VERSION=1.2.1

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

# Generate the code for the wrapper (both Java and C)
generate-wrapper: $(GENERATED_SOURCES) $(GENERATED_HEADERS)


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
	$(LAPACK)/[sd]sygvx.f
	ant javah
	touch $@

$(SRC_C)/NativeBlas.c: generated-sources

$(TARGET_C):
	mkdir $(TARGET_C)

$(TARGET_C)/NativeBlas.o: $(SRC_C)/NativeBlas.c $(TARGET_C)
	$(CC) $(CFLAGS) $(INCDIRS) -c $(SRC_C)/NativeBlas.c -o $@

$(TARGET_C)/jblas_arch_flavor.o: generated-sources
	$(CC) $(CFLAGS) $(INCDIRS) -c $(SRC_C)/jblas_arch_flavor.c -o $@

# Move the compile library to the machine specific directory.
$(FULL_LIB_PATH)/$(LIB)jblas.$(SO) : $(TARGET_C)/NativeBlas.$(SO)
	mkdir -p $(FULL_LIB_PATH)
	mv "$<" "$@"

$(LIB_PATH)/$(LIB)jblas_arch_flavor.$(SO): $(SRC_C)/jblas_arch_flavor.$(SO)
	mkdir -p $(LIB_PATH)
	mv "$<" "$@"

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

######################################################################
#
# Packaging
#


# Build different kind of jars:
#
# * with dynamic libraries
# * with static libraries
# * a "fat" jar with everything
#
# FIXME: I think this build target assumes that the current configuration
# is "dynamic"
all-jars:
	ant clean-jars
	./configure --keep-options $$(cat configure.options)
	ant jar 
	ant lean-jar
	./configure --keep-options --static-libs $$(cat configure.options)
	make
	ant static-jar fat-jar

# Build static jars
all-static-jars:
	./configure --keep-options --static-libs $$(cat configure.options)
	make
	for os_name in native-libs/*; do \
	  for os_arch in $$os_name/* ; do \
	    ant static-jar -Dos_name=$$(basename $$os_name) \
		-Dos_arch=$$(basename $$os_arch); \
	  done; \
	done
