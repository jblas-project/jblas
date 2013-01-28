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

require 'config/path'
require 'config/config'
require 'config/config_cc'
require 'config/config_os_arch'

include JblasConfig
include Path

configure :fortran => ['F77', 'LD']

desc 'deciding whether to use g77 or gfortran'
configure 'F77', 'LD' => ['OS_NAME', 'CC'] do
  #unless ENV['CC'].nil?
  #  CONFIG['CC'] = ENV['CC']
  #  return
  #end

  if CONFIG['OS_NAME'] == 'Mac\ OS\ X'
    CONFIG['LD'] = CONFIG['CC']
    CONFIG['F77'] = 'gfortran'
    CONFIG['CCC'] = 'c99'
  elsif CONFIG['OS_NAME'] == 'Windows'
    if CONFIG['OS_ARCH'] == 'amd64'
      CONFIG['LD'] = W64_PREFIX + 'gfortran'
      CONFIG['F77'] = W64_PREFIX + 'gfortran'
      CONFIG['CCC'] = 'c99'
    else
      CONFIG['LD'] = W32_PREFIX + 'gfortran'
      CONFIG['F77'] = W32_PREFIX + 'gfortran'
      CONFIG['CCC'] = 'c99'
    end
  else
    g77 = Path.where('g77')
    gfortran = Path.where('gfortran')
    f77 = Path.where('f77')
    if g77
      CONFIG['LD'] = 'g77'
      CONFIG['F77'] = 'g77'
      CONFIG['CCC'] = 'f2c'
    elsif gfortran
      CONFIG['F77'] = 'gfortran'
      CONFIG['LD'] = CONFIG['CC']
      CONFIG['CCC'] = 'c99'
    elsif f77
      CONFIG['F77'] = 'f77'
      CONFIG['LD'] = 'f77'
      CONFIG['CCC'] = 'f2c'
    else
      CONFIG.fail <<EOS.indent(2)
Either g77 or gfortran have to be installed to compile against the
fortran libraries.
EOS
    end
  end
  ok(CONFIG['F77'])
end

if __FILE__ == $0
  ConfigureTask.run :fortran
end
