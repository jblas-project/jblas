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

require 'config/config'
require 'config/path'
require 'config/opts'
require 'config/string_ext'

include JblasConfig

configure :lapack_sources => 'LAPACK_HOME'
def check_lapack_home(lapack_home)
  Path.check_files(lapack_home,
        ['BLAS', 'SRC', 'dgemm.f'],
        ['SRC', 'dsyev.f'])
end

desc 'search for lapack sources (configure by --lapack=dir)'
configure 'LAPACK_HOME' do
  lapack_home = $opts.get :lapack, './lapack-lite-3.1.1'
  begin
    check_lapack_home(lapack_home)
  rescue ConfigError => e
    if $opts.defined? :download_lapack
      puts "trying to download lapack (about 5M)"
      print "Looking for wget..."; check_cmd 'wget'; JblasConfig.ok
      lapack_tgz = File.join('.', 'lapack-lite-3.1.1.tgz')
      File.delete(lapack_tgz) if File.exist?(lapack_tgz)
      system("wget http://www.netlib.org/lapack/lapack-lite-3.1.1.tgz")
      system("tar xzvf lapack-lite-3.1.1.tgz")
      check_lapack_home(lapack_home)
    else
      CONFIG['LAPACK_HOME'] = ''
      puts <<EOS.indent(2)
Couldn\'t locate sources for LAPACK and BLAS. Supply the location
of the lapack sources with the --lapack=... switch. Sources for lapack can be
found at http://www.netlib.org/lapack/lapack-lite-3.1.1.tgz or try
running configure with the --download-lapack option.

Compling jblas should work, but unless you have the LAPACK and BLAS sources
you cannot add further stubs.
EOS
    end
  end
  CONFIG['LAPACK_HOME'] = lapack_home
  ok(lapack_home)
end

if __FILE__ == $0
  $opts = Opts.new(ARGV)
  JblasConfig.run :lapack_sources
end