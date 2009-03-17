require 'set'

PATH = ENV['PATH'].split(':')

def where(file, path=PATH)
  path.each do |p|
    fn = File.join(p, file)
    return p if File.exist? fn
  end
  return
end

def collect_paths(files, path=PATH, config=nil)
  paths = Set.new
  files.each do |f|
    config.log "Collecting paths for file #{f}" if config
    p = where(f, path)
    return unless p
    config.log "   found in #{p}" if config
    paths << p
  end
  return paths.to_a
end
