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

LICENSE = <<EOS.split("\n")
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of the Technische Universität Berlin nor the
      names of its contributors may be used to endorse or promote
      products derived from this software without specific prior
      written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EOS

DEFAULT_COPYRIGHT = "2009, Mikio L. Braun"

BLB = '--- BEGIN LICENSE BLOCK ---'
ELB = "--- END LICENSE BLOCK ---"

SUFFIXMAP = {
  '.rb' => :hash,
  '.c' => :doubleslash,
  '.h' => :doubleslash,
  '.java' => :java
}
SUFFIXMAP.default = :hash

# Comment a piece of text (given as an array of lines) for
# a given code type
def comment_block(file, txt)
  #puts File.extname(file)
  code = SUFFIXMAP[File.extname(file)]
  case code
  when :hash
    ['## ' + BLB] + txt.map {|l| '# ' + l} + ['## ' + ELB]
  when :java
    ['// ' + BLB, '/* '] + txt.map{|l| ' * ' + l} + [' */', '// ' + ELB, ]
  when :doubleslash
    ['/// ' + BLB] + txt.map {|l| '// ' + l} + ['/// ' + ELB]
  else
    LICENSE
  end
end

# Remove the license block and return the found copyrights or
# DEFAULT_COPYRIGHT
def remove_license_block(code)
  copyrights = []
  if code[0] =~ Regexp.new(BLB)
    code.shift
    while not code.empty? and code[0] !~ Regexp.new(ELB)
      if code[0] =~ /Copyright \(c\) (.+)/
        copyrights << $1
      end
      code.shift
    end
    code.shift
    code.shift
  end
  #return [DEFAULT_COPYRIGHT]
  return copyrights.empty? ? [DEFAULT_COPYRIGHT] : copyrights
end

# Add a license to a file
def add_license(file)
  #puts "--- #{file}"
  code = open(file).read
  open(file + '~', 'w') {|f| f.print code}
  code = code.split("\n")
  copyrights = remove_license_block(code)
  license_block = copyrights.map {|cr| 'Copyright (c) ' + cr} + LICENSE
  license_block = comment_block(file,license_block)
  code = license_block + [''] + code
  open(file, 'w') {|f| f.puts code.join("\n")}
end

FILES = ['{src,test}/**/*.java',
         'scripts/*.{c,h,java}', 
         '{scripts,config}/**/*.rb',
         'include/*.h', 'Makefile']
#FILES = 'Makefile.lb'
files = FILES.map {|p| Dir.glob(p)}.flatten

files.each do |file| add_license file end
