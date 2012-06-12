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

def readfile(fn)
  f = open(fn)
  s = f.read
  f.close
  s
end

def writefile(fn, s)
  f = open(fn, 'w')
  f.write s
  f.close
end

def classfile(classname)
  classname.tr('.', File::SEPARATOR) + '.java'
end

def translate(s)
  s.gsub! /DoubleMatrix/, 'FloatMatrix'
  s.gsub! /DoubleBuffer/, 'FloatBuffer'
  s.gsub! /DoubleVector/, 'FloatVector'
  s.gsub! /ComplexDouble/, 'ComplexFloat'
  s.gsub! /([0-9]+\.[0-9]+(?:[eE]-?[0-9]+)?)/, '\1f'
  s.gsub! /double/, 'float'
  s.gsub! /(\s)NativeBlas\.dz/, '\1NativeBlas.sc'
  s.gsub! /(\s)NativeBlas\.d/, '\1NativeBlas.s'
  s.gsub! /(\s)NativeBlas\.z/, '\1NativeBlas.c'
  s.gsub! /nextDouble/, 'nextFloat'
  s.gsub! /DoubleFunction/, 'FloatFunction'
  s.gsub! /TestBlasDouble/, 'TestBlasFloat'
  s.gsub! /Double\./, 'Float.'
  s.gsub! /(\s)Double(\s)/, '\1Float\1'
  s.gsub! /<Double>/, '<Float>'
  s.gsub! /readDouble/, 'readFloat'
  s.gsub! /java.lang.Double/, 'java.lang.Float'

  # go through lines. If a line starts with "//FLOAT//" replace the next line by the following line
  discard_lines = 0
  result = []
  s.split("\n").each do |line|
    if discard_lines > 0
      discard_lines -= 1
    else
      i = line.index('//FLOAT//')
      if i
        discard_lines = 1
        result <<= line[(i + "//FLOAT//".length)..-1]
      else
        result <<= line
      end
    end
  end

  result.join("\n")
end

if ARGV.size < 2
  puts "Usage: ruby class_to_float.rb srcprefix classname"
  exit
end

prefix = ARGV[0]
source = ARGV[1]
target = source.gsub(/Double/, 'Float')

sourcefn = File.join(prefix, classfile(source))
targetfn = File.join(prefix, classfile(target))

if File.exist? sourcefn
  s = readfile(sourcefn)
  writefile(targetfn, translate(s))
else
  puts "Could not read file #{sourcefn}"
end
