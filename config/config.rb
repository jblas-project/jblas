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
  end

  def [](k)
    @config[k]
  end

  def []=(k, v)
    @config[k] = v
    log "Setting #{k} to #{v}"
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
      val = $3.chomp
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
