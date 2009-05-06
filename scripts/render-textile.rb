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
