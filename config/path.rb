require 'set'

PATH = ENV['PATH'].split(':')

def where(file, path=PATH)
  path.each do |p|
    fn = File.join(p, file)
    return p if File.exist? fn
  end
  return
end

def collect_paths(files, path=PATH)
  paths = Set.new
  files.each do |f|
    p = where(f, path)
    return unless p
    paths << p
  end
  return paths.to_a
end
