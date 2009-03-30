require 'set'

require 'rubygems'
require 'redcloth'

def link_to_package(txt, pkg)
  txt.gsub!(pkg, 
            "<a href=\"#{pkg.tr('.', '/')}/package-summary.html\">#{pkg}</a>")
end

def link_to_class(txt, cls)
  fullname = SOURCES.
    grep(Regexp.new(cls))[0].
    tr('.', '/').
    gsub('/java', '.html').
    gsub('src/', '')
  txt.gsub!(/(\W)#{cls}(\W)/) do |m| 
    $1 + "<a href=\"#{fullname}\">#{cls}</a>" + $2
  end
end

SOURCES = Dir.glob('src/**/*.java')
CLASSES = SOURCES.map {|f| File.basename(f, '.java')}
PACKAGES = Set.new(SOURCES.map {|f| File.dirname(f)})

#puts "Classes:"
#CLASSES.each {|c| puts c}
#puts
#puts "Packages:"
#PACKAGES.each {|p| puts p}

#exit

ARGV.each do |f|
  newf = f.gsub /.textile$/, '.html'
  out = open(newf, 'w') do |o|
    txt = open(f, 'r').read
    html = RedCloth.new(txt).to_html
    html.gsub!(/<br \/>/, '')
    html.gsub!(/<table>/, '<table class="my">')
    PACKAGES.each {|p| link_to_package(html, p)}
    CLASSES.each {|c| link_to_class(html, c)}
    o.write('<html><body>' + html + '</body></html>')
  end
end
