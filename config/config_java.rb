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

# Configure Java Environment
require 'config/config'
require 'config/path'
require 'config/config_os_arch'

include JblasConfig
include Path

configure :java => ['FOUND_JAVA', 'JAVA_HOME']

desc 'checking for java, javac'
configure 'FOUND_JAVA' do
  check_cmd('java', 'javac')
  %x(javac config/PrintProperty.java)
  CONFIG['FOUND_JAVA'] = true
  ok
end

desc 'locating the Java Development Kit'
configure 'JAVA_HOME' => ['FOUND_JAVA', 'OS_NAME'] do
  if ENV.include? 'JAVA_HOME'
    java_home = ENV['JAVA_HOME']
  else
    java_home = dir(File.dirname(%x(java -cp config PrintProperty java.home)))
  end
  #if CONFIG['OS_NAME'] == 'Mac\ OS\ X'
  #  java_home = File.join(java_home, 'Home')
  #end
  check_files java_home, ['include', 'jni.h'] do
    CONFIG['JAVA_HOME'] = java_home #.escape
  end
  ok(java_home)
end

if __FILE__ == $0
  ConfigureTask.run :java
end
