require 'tictoc'
require 'jmatrix'
require 'benchmark'

include JBLAS

def char(s)
  case s
  when String
    java.lang.Character.new(s[0])
  when Fixnum
    java.lang.Character.new(s)
  end
end

n = 1000

def time_it(n, i)
  x = DoubleVector.randn(n).sort
  k = Geometry.pairwiseSquaredDistances(x, x).muli(-2).expi
  ev = DoubleVector.new(n)

  puts "Benchmark for n = #{n} (reported numbers are #{i} iterations)"

  Benchmark.benchmark do |r|
    r.report("syev eigenvalues:  ") { i.times { SimpleBlas.syev(char('N'), char('U'), k.dup, ev) } }    
    r.report("syevd eigenvalues: ") { i.times { SimpleBlas.syevd(char('N'), char('U'), k.dup, ev) } }    
    r.report("syev eigenvectors: ") { i.times { SimpleBlas.syev(char('V'), char('U'), k.dup, ev) } }    
    r.report("syevd eigenvectors:") { i.times { SimpleBlas.syevd(char('V'), char('U'), k.dup, ev) } }    
  end
end

time_it(100, 100)
time_it(500, 20)
time_it(1000, 5)