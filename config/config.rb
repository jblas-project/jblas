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

module JblasConfig
  class ConfigError < Exception
    attr_reader :message

    def initialize(msg)
      @message = msg
    end
  end

  # A class for collecting configuration information.
  #
  # Basically, this class is like a hash function and is able to output
  # it's results in a format which cCan be read by Makefile.
  #
  # It basically looks like this
  #
  #   KEY=VALUE
  #
  # However, you can also use "<<" on keys, or "+=" in the bulk update
  # sections to add, for example, compile options.
  class ConfigStore
    # Create a new empty config object
    def initialize
      @config = Hash.new {|h,k| Array.new }
      @xmlstuff = Array.new
    end

    # Lookup an existing key.
    def [](k)
      @config[k]
    end

    def add(k, v)
      @config[k] = v
    end

    # Define a key.
    def []=(k, v)
      @config[k] = v
      log "Setting #{k} to #{v}"
    end

    # Add stuff to the xml file.
    def add_xml(str)
      @xmlstuff << str
      return
    end

    # Dump the xml configuration options to a file.
    def dump_xml(io)
      io.puts "<project name=\"imported-configuration-settings\">"
      io.puts(@xmlstuff.join("\n"))
      io.puts "</project>"
    end

    # Bulk add some keys.
    #
    # The format is lines of "KEY=VALUE".
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
            self[var] = [val]
          end
        end
      end
      return
    end

    # Dump the config to a file.
    def dump(io)
      @config.keys.select {|s| String === s}.sort.each do |k|
        v = @config[k]
        case v
        when Array
          io.puts "#{k}=#{v.join ' '}"
        else
          io.puts "#{k}=#{v}"
        end
      end
    end
  end

  class AutomaticConfigStore < ConfigStore
    def [](key)
      ConfigureTask.run(key) unless @config.include? key
      return super(key)
    end

    def []=(key, value)
      super(key, value)
    end
  end

  CONFIG = ConfigStore.new

  class ConfigureTask
    attr_reader :symbols
    attr_reader :dependencies

    def initialize(symbols, dependencies, &block)
      @symbols = symbols
      @dependencies = dependencies
      @block = block
      @has_run = false
      @description = ConfigureTask.get_description
    end

    def run(*args)
      unless @has_run
        @dependencies.each {|d| ConfigureTask.run(d, *args) }
        unless @description.nil?
          print @description + '... '
          $stdout.flush
        end
        @block.call(*args) unless @block.nil?
        @has_run = true
      end
    end

    def self.add(task)
      @tasks ||= {}
      task.symbols.each {|s| @tasks[s] = task}
    end

    def self.run(name, *args)
      task = @tasks[name]
      if task.nil?
        puts "Error: Cannot find task '#{name}'"
      else
        task.run(*args)
      end
    end

    def self.run_all(*args)
      @tasks.values.each {|c| c.run(*args)}
    end

    def self.dump
      p @tasks
    end

    def self.add_description(desc)
      @desc = desc
    end

    def self.get_description
      result = @desc
      @desc = nil
      return result
    end

    def self.log(msg)
      @log ||= open('configure.log', 'w')
      @log.puts(msg)
    end
  end

  module_function

  # Define a new configuration task.
  #
  # Syntax is similar to rake tasks:
  #
  #   * configure :foobar do ... end - task without prerequisites
  #   * configure :foobar => :quux do ... end - task depend on quux
  #
  # The block is optional
  def configure(*args, &block)
    if Hash === args[-1]
      syms = args[0...-1] + [args[-1].keys[0]]
      deps = args[-1].values[0]
      deps = [deps] unless Array === deps
    else
      syms = args
      deps = []
    end
    task = ConfigureTask.new(syms, deps, &block)
    ConfigureTask.add(task)
  end

  # Define the description for a task.
  #
  # This affects the next task defined. If a a description is given,
  # it will be printed before the task is run. The task must then
  # call +ok+ to finish the line.
  def desc(description)
    ConfigureTask.add_description(description)
  end

  # Print 'ok', potentially followed by a comment.
  def ok(msg='')
    if msg.empty?
      puts "ok"
    else
      puts "ok (#{msg})"
    end
  end

  # Print error message and raise a ConfigError.
  def fail(msg='')
    puts "*failed*"
    raise ConfigError, msg
  end

  # Print a message on the screen.
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

  # Print a log message.
  def log(msg)
    ConfigureTask.log(msg)
  end

  def run(target)
    begin
      ConfigureTask.run target
    rescue ConfigError => e
      puts
      puts "Configuration failed!"
      puts
      puts "Reason: #{e.message}"
      exit
    end
  end
end

if __FILE__ == $0
  include JblasConfig

  configure :say_hello do
    puts "Hello"
    CONFIG[:say_hello] = 42
  end

  configure :say_goodbye do
    p CONFIG[:say_hello]
    puts "Goodbye!"
  end

  configure :default => :say_goodbye

  ConfigureTask.run :default
end