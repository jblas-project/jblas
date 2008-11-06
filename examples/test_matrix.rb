require 'test/unit'
require 'jmatrix.rb'
require 'tictoc.rb'

include JBLAS

class TestMatrix < Test::Unit::TestCase
  def run_some_tests(mat,vec)
    # construct 3 * 3 matrix and fill in some values
    a = mat.new(3,3)
    for i in 0..2
      for j in 0..2
        a[i,j] = i + j
      end
    end
    
    puts a
    
    # construct a matrix from nested arrays
    x = mat[[1], [2], [3]]
    a = mat[[1, 2, 3], [4, 5, 6], [7, 8, 9]]
    
    puts x
    puts a
    
    # normal arithmetics work
    puts (y = a * vec[1, 2, 3])
    
    # elementwise exponential
    puts vec[1,2,3].exp
    
  
    # elementwise exponential on a long vector
    z = vec[*(0..10000).to_a]
    tic "applying exp to the whole matrix"
    x = z.dup.map {|x| Math.exp(x)}
    toc
    
    tic "the same in Java"
    x = z.dup.exp
    toc
    
    # computing norms
    puts "y = #{y}"
    puts "2-norm of y = #{y.norm2}"
    puts "1-norm of y = #{y.norm1}"
    puts "maximum norm of y = #{y.maxnorm}"
    
    puts "converting a vector to an array"
    puts y.to_array
    puts a.to_array
    
    # a big matrix
    n = 1000
    puts "filling #{n} * #{n} matrix"
    m = mat.new(n, n)
    for i in 0...10
      for j in 0...n
    m[i,j] = rand
      end
    end
    
    puts "Multiplying with itself"
    
    5.times do 
      tic "Timing"
      c = m * m
      toc
    end
    
    # rank one update
    x = vec.rand(1000)
    
    5.times do
      tic "Rank one update with xGER       "
      c = m.rank_one_update(x)
      toc
      tic "using explicit matrix operations"
      c = m + x.as_column_vector * x.as_row_vector
      toc
    end
    
    tic "compared to matrix - vector multiplication"
    c = m * x
    toc
    
  end

  def test_double
    run_some_tests(DoubleMatrix, DoubleVector)
  end

  def test_float
    run_some_tests(FloatMatrix, FloatVector)
  end
end
