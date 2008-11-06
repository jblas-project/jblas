require 'test/unit'
require 'jmatrix'
require 'tictoc'

include JBLAS

import 'edu.ida.la.Solve'
import 'edu.ida.la.Eigen'
import 'edu.ida.la.Geometry'

#
# Radial Basis Function Kernel
#
class RbfKern
  def initialize(width)
    @width = width
  end

  def [](x, y=nil)
    #puts "upsi"
    if not y
      y = x
    end

    Geometry.pairwiseSquaredDistances(x, y).muli(-1.0/(2.0*@width)).expi()
  end
end

def normalize_mse(yhat, y)
  (y - yhat).norm2 / (y - y.mean).norm2
end

class KRR
  def initialize(kernel)
    @kernel = kernel
  end

  def train(tau, x, y)
    n = y.length
    tic "Setting up kernel matrix"
    k = @kernel[x]
    toc
    tic "Computing alpha (sysv)"
    @alpha = Solve.solveSymmetric(k + tau * k.class.eye(n), y)
    toc
    tic "Computing alpha (posv)"
    @alpha = Solve.solvePositive(k + tau * k.class.eye(n), y)
    toc
    
    tic "computing eigenvalues"
    ev = Eigen.symmetricEigenvalues(k)
    toc

    tic "computing eigenvectors and eigenvalues"
    ev = Eigen.symmetricEigenvectors(k)
    toc
    
    puts ev
    if n < 50
      puts ev[0]
      puts 
      puts ev[1]
    end 

    @trainX = x
  end

  def [](x)
    tic "Setting up kernel matrix for prediction"
    k = @kernel[x, @trainX]
    toc
    tic "Predicting"
    yhat = k * @alpha
    toc
    return yhat
  end
end


class ABitOfML < Test::Unit::TestCase
  def krr_on_sinc(n, vec, ext)
    
    tic "Generating the data..."
    #x = 8 * vec.rand(n).sort - 4
    x = 8 * vec.rand(n) - 4
    s = x.sinc()
    y = s + vec.randn(n) * 0.1
    toc

    krr = KRR.new(RbfKern.new 1.0)
    krr.train(1e-3, x, y)
    yhat = krr[x]

    fit_error = normalize_mse(yhat, y)
    
    puts "Fit error #{fit_error}"

    assert_in_delta(0.3, fit_error, 0.1)

    #
    # generate a matlab script to plot the result
    #
    open("sincdemo_#{ext}.m", "w") do |o|
      o.write "X = #{x};"
      o.write "Y = #{y};"
      o.write "Yhat = #{yhat};"
      o.write "plot(X, Y, '.', X, Yhat, 'r-', 'LineWidth', 5);"
    end
  end

  def test_double
    krr_on_sinc(1000, DoubleVector, 'double')
  end

  def test_float
    krr_on_sinc(1000, FloatVector, 'float')
  end
end
