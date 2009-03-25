require 'set'

PATH = ENV['PATH'].split(':')

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
