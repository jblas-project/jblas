# This file contains a very unfinished sketch of how you can use the matrix
# classes from jruby. Note that we make heavy use of ruby's ability to extend
# existing classes, for example, in order to devine arithmetic operators for
# the matrix classes.
require 'java'
require '../jBLAS.jar'

module JBLAS
  include Java
  import 'edu.ida.la.DoubleMatrix'
  import 'edu.ida.la.FloatMatrix'
  import 'edu.ida.la.DoubleVector'
  import 'edu.ida.la.FloatVector'
  import 'edu.ida.la.SimpleBlas'
  import 'edu.ida.la.DoubleFunction'
  import 'edu.ida.la.FloatFunction'
  
  import 'edu.ida.la.Solve'
  import 'edu.ida.la.Eigen'
  import 'edu.ida.la.Geometry'

  # This module contains the matrix object extensions which will later be mixed-in
  # to DoubleMatrix and FloatMatrix. This safes us from unnecessary code
  # duplication.
  module MatrixExt
    def [](i, j)
      get(i, j)
    end
    
    def []=(i, j, v)
      put(i, j, v)
    end
    
    def +(o); add(o); end
    def -(o); sub(o); end
    def *(o); mul(o); end
    
    def coerce(o)
      unless self.class === o
        [self, o]
      end
    end
    
    def t; transpose; end
  end
  
  module MatrixStaticExt
    def [](*data)
      n = data.length
      if data.reject{|l| Numeric === l}.size == 0
        a = self.new(1, n)
        (0...data.length).each do |i|
          a[0, i] = data[i]
        end
        return a
      else
        begin
          lengths = data.collect{|v| v.length}
        rescue
          raise "All columns must be arrays"
        end
        raise "All columns must have equal length!" if lengths.min < lengths.max
        a = self.new(n, lengths.max)
        for i in 0...n
          for j in 0...lengths.max
            a[i,j] = data[i][j]
          end
        end
        return a
      end
    end
  end

  # Extend the matrix classes with the functionalities defined above. 
  class DoubleMatrix
    class << self
      include MatrixStaticExt
    end
    include MatrixExt
  end
  
  class FloatMatrix
    class << self
      include MatrixStaticExt
    end
    include MatrixExt
  end

  # Double Vector and Float Vector, fully spelled out this time.
  class DoubleVector
    def [](i); get(i); end
    
    def []=(i, v); put(i, v); end
    
    def +(o); add(o); end
    def -(o); sub(o); end
    def *(o); mul(o); end
    
    def self.[](*data)
      a = self.new(data.length)
      for i in (0...data.length)
        a[i] = data[i].to_f
      end
      return a
    end
    
    def coerce(o)
      unless self.class === o
        [self, o]
      end
    end
    
    def to_s
      "[" + toString.gsub(/\t/, ', ') + "]"
    end
    
    class Fct
      include DoubleFunction
      
      def initialize(block)
        @block = block
      end
      
      def compute(x)
        @block.call(x)
      end
    end
    
    alias java_map map

    # a slightly more rubyesque map function which takes a block
    def map(&block)
      java_map(Fct.new block)
    end
  end

  class FloatVector
    def [](i); get(i); end
    
    def []=(i, v); put(i, v); end
    
    def +(o); add(o); end
    def -(o); sub(o); end
    def *(o); mul(o); end
    
    def self.[](*data)
      a = self.new(data.length)
      for i in (0...data.length)
        a[i] = data[i].to_f
      end
      return a
    end
    
    def coerce(o)
      unless self.class === o
        [self, o]
      end
    end
    
    def to_s
      "[" + toString.gsub(/\t/, ', ') + "]"
    end
    
    class Fct
      include FloatFunction
      
      def initialize(block)
        @block = block
      end
      
      def compute(x)
        @block.call(x)
      end
    end
    
    alias java_map map

    # a slightly more rubyesque map function which takes a block
    def map(&block)
      java_map(Fct.new block)
    end
  end
end
