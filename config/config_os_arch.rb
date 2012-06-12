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

#
# Check the Operating System
#

require 'config/config'
require 'config/string_ext'
require 'config/config_java'

include JblasConfig

def detect_os
  os_name = %x(java -cp config PrintProperty os.name).chomp
  if os_name.start_with? 'Windows'
    os_name = 'Windows'
  end
  return os_name
end

configure :os_arch => ['OS_NAME', 'OS_ARCH', 'ARCH_FLAVOR']

desc 'determining operating system'
configure 'OS_NAME' => 'FOUND_JAVA' do
  os_name = detect_os
  CONFIG['OS_NAME'] = os_name.gsub(' ', '\ ')
  CONFIG.add_xml "<property name=\"os_name\" value=\"#{os_name}\" />"
  ok(os_name)
end

desc 'determining architecture'
configure 'OS_ARCH' => 'FOUND_JAVA' do
  os_arch = %x(java -cp config PrintProperty os.arch).chomp
  CONFIG['OS_ARCH'] = os_arch
  CONFIG.add_xml "<property name=\"os_arch\" value=\"#{os_arch}\" />"
  ok(os_arch)
end

desc 'determining architecture flavor'
configure 'ARCH_FLAVOR' => 'OS_ARCH' do
  if $opts.defined? :arch_flavor
    arch_flavor = $opts[:arch_flavor]
  elsif %w(i386 amd64 x86 x86_64).include? CONFIG['OS_ARCH']
    Path.check_cmd('gcc')
    out = %x(gcc -o config/arch_flavor config/arch_flavor.c)
    fail('couldn\'t compile the config script') unless out.empty?
    arch_flavor = %x(config/arch_flavor).chomp
  else
    arch_flavor = ''
  end

  if arch_flavor.empty? or arch_flavor == 'none'
    CONFIG['OS_ARCH_WITH_FLAVOR'] = CONFIG['OS_ARCH']
  else
    CONFIG['OS_ARCH_WITH_FLAVOR'] = CONFIG['OS_ARCH'] + File::SEPARATOR + arch_flavor
  end

  ok(arch_flavor)
end

if __FILE__ == $0
  ConfigureTask.run :os_arch
end
