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

def translate(cl, s)
  mark = "public class #{cl.split('.').last} {\n"
  i = s.index(mark) + mark.length
  j = s.rindex('}')
  head = s[0...i]
  body = s[i...j]

  # remove part copied in last time
  body.gsub! /\/\/BEGIN.*?\/\/END\n/m, ''

  copybody = body.dup

  # remove part which should not be added as float
  copybody.gsub! /\/\/STOP.*?\/\/START\n/m, ''

  # do necessary substitutions
  copybody.gsub! /DoubleMatrix/, 'FloatMatrix'
  copybody.gsub! /DoubleBuffer/, 'FloatBuffer'
  copybody.gsub! /DoubleVector/, 'FloatVector'
  copybody.gsub! /ComplexDouble/, 'ComplexFloat'
  copybody.gsub! /([0-9]+\.[0-9]+(?:[eE]-?[0-9]+)?)/, '\1f'
  copybody.gsub! /double/, 'float'
  copybody.gsub! /NativeBlas\.dz/, 'NativeBlas.sc'
  copybody.gsub! /NativeBlas\.d/, 'NativeBlas.s'
  copybody.gsub! /NativeBlas\.z/, 'NativeBlas.c'
  copybody.gsub! /NativeBlas\.idamax/, 'NativeBlas.isamax'
  copybody.gsub! /NativeBlas\.izamax/, 'NativeBlas.icamax'
  copybody.gsub!(/([a-z]+)Double/, '\1Float')

  # construct output file
  out = []
  out << head
  out << body
  out << "//BEGIN\n"
  out << "  // The code below has been automatically generated.\n"
  out << "  // DO NOT EDIT!\n"
  out << copybody
  out << "//END\n"
  out << "}\n"
  return out.join
end

if ARGV.size < 2
  puts "Usage: ruby static_class_to_float.rb srcprefix classname"
  exit
end

prefix = ARGV[0]
source = ARGV[1]

fn = File.join(prefix, classfile(source))

if File.exist? fn
  s = readfile(fn)
  writefile(fn, translate(source, s))
else
  puts "Could not read file #{fn}"
end
