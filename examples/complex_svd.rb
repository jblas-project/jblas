require 'java'
require 'jblas-1.0.3.jar'

java_import 'org.jblas.ComplexDoubleMatrix'
java_import 'org.jblas.DoubleMatrix'
java_import 'org.jblas.Singular'

Matrix = ComplexDoubleMatrix

M = 4
N = 3

m = Matrix.new(M, N)

for i in 0...M
  for j in 0...N
    m.put(i, j, i+j, i*j)
    #m.put(i, j, i+j)
  end
end

puts m.dup

u, s, v = Singular.sparseSVD(m) 

s = Matrix.diag(s)

puts "u = #{u}"
puts "s = #{s}"
puts "v = #{v}"

puts u.mmul(s).mmul(v.transpose).sub(m).norm2()

puts Singular.SVDValues(m)
