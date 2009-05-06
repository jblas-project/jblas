# This file defines the main types for representing
# functions/subroutines and their arguments.
#
# Two helper functions +subroutine+ and +function+ are defined which
# construct a Routine object representing either a subroutine (with
# return type 'VOID' or a function.
#
# Note that all types are basically represented by strings.
#
# Ideally, all the information about different types of arguments
# should be contained in the classes FortranType or Routine, and
# not encoded "by hand" elsewhere.
module Fortran
  # Mapping of some types to default names
  DefaultTypes = {
    'BYTE' => 'LOGICAL*1',
    'COMPLEX' => 'COMPLEX*8',
    'DOUBLE COMPLEX' => 'COMPLEX*16',
    'DOUBLE PRECISION' => 'REAL*8',
    'INTEGER' => 'INTEGER*4',
    'LOGICAL' => 'LOGICAL*4',
    'REAL' => 'REAL*4' }

  # Map fortran types to a unique name (via DefaultTypes).
  def self.standardize_type(name)
    name.strip!
    DefaultTypes.fetch name, name
  end

  # Return a Fortran subroutine object.
  # +args+ is an array of the names of the arguments
  def self.subroutine(name, args)
    Routine.new FortranType.new('VOID'), name, args
  end

  # Return a Fortran function object.
  def self.function(return_type, name, args)
    Routine.new FortranType.new(return_type), name, args
  end

  # Represents a FORTRAN data type
  class FortranType
    attr_accessor :basetype, :array, :comment

    # Construct new FortranType. Set array and comment later if you want to.
    def initialize(type, array=false, comment=nil)
      @basetype = Fortran.standardize_type type
      @array = array
      @comment = comment
    end

    def to_s # :nodoc:
      s = @basetype.clone
      s << ' (array)' if @array
      s << ' # ' + @comment if comment
      s
    end

    def output?
      comment.nil? or comment =~ /output/
    end
  end

  # This class describes a fortran subroutine or function. It
  # mainly collects all the information about the arguments and
  # their types.
  #
  # You can use Fortran.subroutine and Fortran.function to
  # construct the respective type.
  class Routine
    # name of the routine
    attr_reader :name
    # argument names of the routines (as array)
    attr_reader :args
    # hash mapping argument names to FortranType objects
    attr_reader :argtype
    # return type as FortranType object or nil
    attr_reader :return_type

    # Construct a new Routine object. If return_type is nil, it is a subroutine.
    def initialize(return_type, name, args)
      @return_type = return_type
      @name = name
      @args = args
      @argtype = Hash.new
    end

    # Check whether all arguments have at least types.
    def have_all_types?
      @args.each do |a|
        unless @argtype.has_key? a
          return false
        end
      end
      true
    end

    def each_arg
      args.each do |name|
        type = argtype[name]
        yield name, type
      end
    end

    def to_s # :nodoc:
      s = [ (@return_type ? "#@return_type function " : "subroutine ") + @name + "\n"]
      @args.each do |arg|
        s << "   #{arg} of type #{@argtype[arg]}"
      end
      unless workspace_arguments.empty?
        s << "Workspace Arguments: " + workspace_arguments.join(', ')
      end
      s.join
    end

    def number_of_scalar_output_arguments
      count = 0
      each_arg do |name, type|
        if type.comment =~ /output/ and not type.array
          count += 1
        end
      end
      count
    end

    # Generate some code for each argument and collect the results
    def gen_each_arg(joinwith="\n")
      s = []
      args.each do |name|
        t = yield name, argtype[name]
        s << t unless t.nil?
      end
      return s.join(joinwith)
    end

    def workspace_arguments
      results = []
      args.each_with_index do |a, i|
        results << a if workspace_argument? a
      end
      return results
    end

    def workspace_argument?(name)
      i = args.index(name)
      name =~ /WORK\Z/ and i < @args.size - 1 and args[i+1] == 'L' + name
    end

    def workspace_size_argument?(name)
      i = args.index(name)
      name =~ /\AL[A-Z]*WORK\Z/ and i > 0 and args[i-1] == name[1..-1]
    end

    def output?(name)
       @argtype[name].output?
    end
  end
end
