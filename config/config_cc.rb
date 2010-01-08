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
require 'config/config_java'

include Config
include Path

# Set up flags for different environments.
configure :cc => 'CC'

desc 'Setting up gcc and flags'
configure 'CC', 'CFLAGS' => ['OS_NAME', 'OS_ARCH', 'JAVA_HOME'] do
  os_name = Config::CONFIG['OS_NAME']
  java_home = Config::CONFIG['JAVA_HOME'].escape
  case os_name
  when 'Linux'
    Path.check_cmd('gcc', 'make', 'ld')
    Config::CONFIG << <<EOS
CC = gcc
CFLAGS = -fPIC
INCDIRS += -Iinclude -I#{java_home}/include -I#{java_home}/include/linux
SO = so
LIB = lib
RUBY=ruby
LDFLAGS += -shared
EOS
  when 'SunOS'
    Path.check_cmd('gcc', 'make', 'ld')
    Config::CONFIG << <<EOS
CC = gcc
CFLAGS = -fPIC
INCDIRS += -Iinclude -I#{java_home}/include -I#{java_home}/include/solaris
SO = so
LIB = lib
RUBY=ruby
LDFLAGS += -G
EOS
  when 'Windows'
    Path.check_cmd('gcc', 'make', 'ld')
    Path.check_cmd('cygpath')
    Config::CONFIG << <<EOS
CC = gcc
CFLAGS = -ggdb -D__int64='long long'
INCDIRS += -I"#{dir java_home}/include" -I"#{dir java_home}/include/win32" -Iinclude
LDFLAGS += -mno-cygwin -shared -Wl,--add-stdcall-alias
SO = dll
LIB =
RUBY = ruby
EOS
  when 'Mac\ OS\ X'
    Path.check_cmd('gcc-mp-4.3', 'make')
    Config::CONFIG << <<EOS
CC = gcc-mp-4.3
LD = gcc-mp-4.3
CFLAGS = -fPIC
INCDIRS += -Iinclude -I#{java_home}/include
SO = jnilib
LIB = lib
RUBY = ruby
LDFLAGS += -shared
EOS
  else
    Config.fail "Sorry, the OS #{os_name} is currently not supported"
  end

  if %w(i386 x86 x86_64 amd64).include? Config::CONFIG['OS_ARCH']
    Config::CONFIG['CFLAGS'] << ' -DHAS_CPUID'
  end

  ok(CONFIG['CC'])
end

if __FILE__ == $0
  ConfigureTask.run :cc
end