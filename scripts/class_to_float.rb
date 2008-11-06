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
  s.gsub! /([0-9]+\.[0-9]+)/, '\1f'
  s.gsub! /double/, 'float'
  s.gsub! /(\s)Blas\.dz/, '\1Blas.sc'
  s.gsub! /(\s)Blas\.d/, '\1Blas.s'
  s.gsub! /(\s)Blas\.z/, '\1Blas.c'
  s.gsub! /nextDouble/, 'nextFloat'
  s.gsub! /DoubleFunction/, 'FloatFunction'
  s.gsub! /TestBlasDouble/, 'TestBlasFloat'
  s.gsub! /Double\./, 'Float.'
  s.gsub! /readDouble/, 'readFloat'
  s
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

