# This file demonstrates how to access a java file contained in a
# jar file, how to extend the java object and make a numeric
# class interact seamlessly with normal ruby numeric objects

# load the jar file
require '../jBLAS.jar'

# We provide a short-cut for the class com.fhg.core.DoubleComplex
DoubleComplex = Java::edu.ida.core.DoubleComplex

# we extend the class
class DoubleComplex
  # add operators
  def +(o); add(promote(o)); end
  def -(o); sub(promote(o)); end
  def *(o); mul(promote(o)); end
  def /(o); div(promote(o)); end

  # extend the range of the operators to also deal with
  # non-complex numeric arguments
  def promote(o)
    if Numeric === o
      DoubleComplex.new(o, 0)
    else
      o
    end
  end

  # make sure that it also works when the left hand side operator
  # is not a double
  def coerce(o)
    unless DoubleComplex === o
      [self, o]
    end
  end

  # to make sure that we look pretty in irb
  def inspect
    toString
  end
end

# imaginary number
I = DoubleComplex.new(0, 1)

# and voila, it works!
puts (1 + 3*I)/ 2*I
