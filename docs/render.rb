require 'rubygems'
require 'redcloth'

ARGV.each do |f|
  newf = f.gsub /.textile$/, '.html'
  out = open(newf, 'w') do |o|
    txt = open(f, 'r').read
    o.write RedCloth.new(txt).to_html
  end
end
