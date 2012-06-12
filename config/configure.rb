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

$: << "."

require 'config/path'
require 'config/config'
require 'config/opts'

require 'config/config_tools'
require 'config/config_os_arch'
require 'config/config_cc'
require 'config/config_fortran'
require 'config/config_make'
require 'config/config_lapack_sources'
require 'config/config_libs'

include JblasConfig
include Path

args = []
paste = false
ARGV.each do |arg|
  if arg[-1] == ?\\
    paste_next = true
    arg = arg[0...-1] + ' '
  else
    paste_next = false
  end

  if paste
    args[-1] += arg
  else
    args << arg
  end

  paste = paste_next
end

$opts = Opts.new(args, {}, <<EOS)
Usage: ./configure [options]

options summary:
  --lapack=DIR             lapack-lite directory
  --lapack-build           build against fortran lapack instead of ATLAS
  --libpath=DIR1:DIR2:...  comma separated list of directories to contain
                           the ATLAS libraries
  --download-lapack        try to download and compile lapack if not
                           found
  --static-libs            look for static libraries only. Results in a
                           dynamically loaded jblas library which does
                           not depend on lapack or atlas libraries. 
                           (default for Windows!)
  --ptatlas                Link against multithreaded versions of ATLAS libraries
  --arch-flavor=...        Set architectural flavor (e.g. --arch-flavor=sse2)
  --libs=lib1,lib2,...     Override libraries to search in
EOS

configure :all => [:os_arch, :tools, :java, :cc, :fortran, :make, :lapack_sources, :libs]

run :all

puts
puts 'Configuration succesfull, writing out results to configure.out'
open('configure.out', 'w') {|f| CONFIG.dump f}
open('configure.xml', 'w') {|f| CONFIG.dump_xml f}
