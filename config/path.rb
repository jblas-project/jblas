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

require 'set'
require 'config/config'

module Path
  PATH = ENV['PATH'].split(':')

  module_function

  def where(file, path=PATH)
    path.each do |p|
      fn = File.join(p, file)
      #print "  Checking #{fn}"
      if File.exist? fn
        #puts "  found"
        if block_given?
          return p if yield fn
        else
          return p
        end
      else
        #puts
      end
    end
    return
  end

  # returns the path to the command as specified by
  # line or nil if the command does not exist, or
  # it did not produce the right result
  def where_with_output(line, output)
    cmd = line.split[0]
    p = where(cmd)
    return unless p
    out = %x(#{File.join(p,line)})
    if out =~ output
      return p
    else
      return
    end
  end

  # Check whether a cmd could be found.
  def check_cmd(*cmds)
    cmds.each do |cmd|
      JblasConfig.log "Searching for command #{cmd}"
      JblasConfig.fail("coulnd't find command #{cmd}") unless Path.where cmd
    end
    yield self if block_given?
    return
  end

  # Check whether files could be found in the given path.
  def check_files(path, *files)
    files.each do |file|
      file = File.join(path, *file)
      JblasConfig.log "Searching for file #{file}"
      JblasConfig.fail("couldn't find #{file}") unless File.exist? file
    end
    yield if block_given?
    return
  end

  # translate dir (mainly necessary for cygwin)
  def dir(s)
    case JblasConfig::CONFIG['OS_NAME']
    when 'Windows'
      s = s.gsub(/\\/, '\\\\\\\\')
      %x(cygpath -u '#{s}').chomp
    else
      s # safe default...
    end
  end
end