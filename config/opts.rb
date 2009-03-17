class Opts
  attr_accessor :opts, :args

  def initialize(args, shortcuts={})
    @opts = Hash.new
    @args = Array.new
    args.each do |a|
      case a
      when /\A--/
        a = a[2..-1]
        if a.index('=')
          key, value = a.split('=')
          opts[key.to_sym] = value
        else
          opts[a.to_sym] = true
        end
      when /\A-/
        key = a[1..1].to_sym
        key = shortcuts[key] if shortcuts.has_key? key
        value = a[2..-1]
        if not value.empty?
          opts[key] = value
        else
          opts[key] = true
        end
      else
        @args << a
      end
    end

    def defined?(v)
      @opts.has_key? v
    end

    def [](key)
      @opts[key]
    end

    def get(key, default)
      if @opts.include? key
        @opts[key]
      else
        default
      end
    end
  end
end
