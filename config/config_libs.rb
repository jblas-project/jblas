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
# Locating the libraries
#
# This is a bit involved since the standard ATLAS compilation produces
# a liblapack.so which only implements a subset of the lapack functions.
# So the basic strategy is to
#
# (a) find the path to the atlas specific library
# (b) find the path to a full lapack library
#
# Pass the lapack libraries and theirs paths first.
#
# Of course, if you want to build a lapack-only path, we only search
# for the full lapack libraries.

require 'config/config'
require 'config/lib_helpers'
require 'config/opts'
require 'config/config_os_arch'
require 'config/config_fortran'

include Config

ATLAS_REQUIRED_SYMBOLS = [
  'dsyev_', # eigenvalue function not yet included in ATLAS/LAPACK
  'ATL_dgemm',
  'dgemm_', # matrix-matrix multiplication
  'daxpy_', # blas-related function
  'cblas_daxpy', # atlas itself often uses cblas
  'ATL_caxpy'
]

LAPACK_REQUIRED_SYMBOLS = [ 'dsyev_', 'daxpy_' ]

ATLAS_LIBS = %w(lapack lapack_fortran lapack_atlas f77blas atlas cblas)
LAPACK_LIBS = %w(lapack_fortran lapack blas_fortran blas)

configure :libs => 'LOADLIBES'

desc 'determining build type'
configure 'LINKAGE_TYPE' do
  if $opts.defined? :static_libs
    CONFIG['LINKAGE_TYPE'] = 'static'
    CONFIG.add_xml '<property name="linkage" value="static" />'
  else
    CONFIG['LINKAGE_TYPE'] = 'dynamic'
    CONFIG.add_xml '<property name="linkage" value="dynamic" />'
  end
  ok(CONFIG['LINKAGE_TYPE'])
end

desc 'getting library path...'
configure :libpath => 'OS_NAME' do
  if $opts.defined? :libpath
    CONFIG[:libpath] = $opts[:libpath].split(':')
  else
    if CONFIG['OS_NAME'] == 'Mac\ OS\ X'
      CONFIG[:libpath] = ['/opt/local/lib']
    else
      CONFIG[:libpath] = %w(/usr/lib /lib /usr/lib/sse2)
    end
  end
  ok(CONFIG[:libpath].inspect)
end

desc 'determining whether to build for lapack or atlas'
configure 'BUILD_TYPE' do
  if $opts.defined? :lapack_build
    CONFIG['BUILD_TYPE'] = 'lapack'
    ok('lapack')
  else
    CONFIG['BUILD_TYPE'] = 'atlas'
    ok('atlas')
  end
end

desc 'looking for libraries...'
configure 'LOADLIBES' => ['LINKAGE_TYPE', :libpath, 'F77', 'BUILD_TYPE'] do
  case CONFIG['BUILD_TYPE']
  when 'atlas'
    result = LibHelpers.find_libs(CONFIG[:libpath], ATLAS_LIBS, ATLAS_REQUIRED_SYMBOLS)
  when 'lapack'
    result = LibHelpers.find_libs(CONFIG[:libpath], LAPACK_LIBS, LAPACK_REQUIRED_SYMBOLS)
  end

  case CONFIG['LINKAGE_TYPE']
  when 'dynamic'
    CONFIG['LDFLAGS'] += result.values.uniq.map {|s| '-L' + s}
    CONFIG['LOADLIBES'] += result.keys.map {|s| '-l' + s}
  when 'static'
    #CONFIG['LOADLIBES'] += ['-Wl,--allow-multiple-definition'] unless CONFIG['OS_NAME'] == 'Mac\ OS\ X'
    CONFIG['LOADLIBES'] += result.keys.map {|s| File.join(result[s], LibHelpers.libname(s)) }
    if CONFIG['F77'] == 'gfortran'
      CONFIG['LOADLIBES'] += ['-l:libgfortran.a']
    end
    if CONFIG['OS_NAME'] == 'Mac\ OS\ X'
      CONFIG['LOADLIBES'] += ['/opt/local/lib/gcc43/libgfortran.a']
    end 
  end
  ok
end

if __FILE__ == $0
  $opts = Opts.new(ARGV)
  Config.run :libs
  Config::CONFIG.dump($stdout)
end