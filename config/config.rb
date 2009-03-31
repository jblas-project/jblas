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

class ConfigError < Exception
  attr_reader :message

  def initialize(msg)
    @message = msg
  end
end

class Config
  def initialize
    @config = Hash.new {|h,k| Array.new }
    @log = open('configure.log', 'w')
    @xmlstuff = Array.new
  end

  def [](k)
    @config[k]
  end

  def []=(k, v)
    @config[k] = v
    log "Setting #{k} to #{v}"
  end

  def add_xml(str)
    @xmlstuff << str
    return
  end

  def dump_xml(io)
    io.puts "<project name=\"imported-configuration-settings\">"
    io.puts(@xmlstuff.join "\n")
    io.puts "</project>"
  end

  def log(msg)
    @log.puts(msg)
  end

  def <<(str)
    str.each_line do |line|
      next if line.chomp.empty?
      i = line.index /\A\s*([a-zA-Z_]+)\s*(\+?=)(.*)/
      if i.nil?
        puts "Warning: Cannot parse config definition '#{line}'"
        next
      end
      var = $1
      op = $2
      val = $3.strip
      if op == '='
        self[var] = val
      elsif op == '+='
        if @config.has_key? var
          self[var] = [self[var], val]
        else
          self[var] <<= val
        end
      end
    end
    return
  end

  def dump(io)
    @config.each_pair do |k, v|
      case v
      when Array
        io.puts "#{k}=#{v.join ' '}"
      else
        io.puts "#{k}=#{v}"
      end
    end
  end

  def msg(m)
    print m + "... "
    $stdout.flush
    if block_given?
      msg = yield
      if msg.nil?
        ok
      else
        puts msg
      end
    end
  end

  def ok
    puts "ok"
  end

  def fail(msg=false)
    puts "*failed*"
    raise ConfigError, msg
  end

  def check_cmd(*cmds)
    cmds.each do |cmd|
      log "Searching for command #{cmd}"
      self.fail("coulnd't find command #{cmd}") unless where cmd
    end
    yield self if block_given?
    return 
  end

  def check_files(path, *files)
    files.each do |file|
      file = File.join(path, *file)
      log "Searching for file #{file}"
      self.fail("couldn't find #{file}") unless File.exist? file
    end
    yield self if block_given?
    return
  end
end
