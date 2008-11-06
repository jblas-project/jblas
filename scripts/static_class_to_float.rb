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
  copybody.gsub! /([0-9]+\.[0-9]+)/, '\1f'
  copybody.gsub! /double/, 'float'
  copybody.gsub! /Blas\.dz/, 'Blas.sc'
  copybody.gsub! /Blas\.d/, 'Blas.s'
  copybody.gsub! /Blas\.z/, 'Blas.c'
  copybody.gsub! /Blas\.idamax/, 'Blas.isamax'
  copybody.gsub! /Blas\.izamax/, 'Blas.icamax'
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

