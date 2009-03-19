require 'set'

PATH = ENV['PATH'].split(':')

def where(file, path=PATH)
  path.each do |p|
    fn = File.join(p, file)
    if File.exist? fn
      if block_given? 
        return p if yield fn
      else
        return p
      end
    end
  end
  return
end
