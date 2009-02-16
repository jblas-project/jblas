# This module contains a minimum of parsing routines and data types to
# be able to write automatic wrappers of blas and lapack routines
module Fortran

  # Fortran 70 types
  # Source: http://www.nacs.uci.edu/dcslib/sun/compilers/fortran/f77rm/2_data.doc.html
  Types = ['BYTE',
           'CHARACTER',
           'CHARACTER\*[0-9]+',
           'CHARACTER\*\(\s*\*\s*\)',
           'COMPLEX(?:\*(?:8|16))?',
           'DOUBLE\ COMPLEX',
           'DOUBLE\ PRECISION', 
           'INTEGER(?:\*(?:2|4|8))?',
           'LOGICAL(?:\*(?:1|2|4|8))?',
           'REAL(?:\*(?:4|8))?']

  # Mapping of some types to default names
  DefaultTypes = { 
    'BYTE' => 'LOGICAL*1',
    'COMPLEX' => 'COMPLEX*8',
    'DOUBLE COMPLEX' => 'COMPLEX*16',
    'DOUBLE PRECISION' => 'REAL*8',
    'INTEGER' => 'INTEGER*4',
    'LOGICAL' => 'LOGICAL*4',
    'REAL' => 'REAL*4' }
            

  # Return a regular expression which matches against all Fortran::Types.
  def self.types_as_regexp
    Fortran::Types.join('|')
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

  # Concatenate all continuated lines. These are lines where there is a 
  # mark ('$' or 'C') in the 5th column. For example:
  #
  #  1234567890
  #       SUBROUTINE DGEEVX( BALANC, JOBVL, JOBVR, SENSE, N, A, LDA, WR, WI,
  #      $                   VL, LDVL, VR, LDVR, ILO, IHI, SCALE, ABNRM,
  #      $                   RCONDE, RCONDV, WORK, LWORK, IWORK, INFO )
  #
  # Returns an array of the resulting lines.
  def self.parse_continuations(lines)
    m = []
    lines.each do |l|
      l.chomp!
      if l =~ /^     [$C]\ *( .+)/ then 
        m[-1] << $1
      else 
        m << l
      end
    end
    m
  end

  # Map fortran types to a unique name (via DefaultTypes).
  def self.standardize_type(name)
    name.strip!
    DefaultTypes.fetch name, name
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
        s << "   #{arg} of type #{@argtype[arg]}\n"
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
  end

  # In order to pass additional information to the template code,
  # subclass Template Context and put the information into instance
  # variables. These will then be accessible from the template.
  class TemplateContext

    # Generate output from a template file.
    # This function also uses the new-line suppressing extension as
    # in rails. Put a minus after or before the '%' to suppress the 
    # previous or following newline.
    #
    #    <%= some expression %>\nblahblah  <- newline is output
    #    <%= some expression -%>\nblahblah <- newline is removed
    def generate(template_file)
      f = open(template_file)
      template = f.read
      f.close

      # Add the littel newline suppression hack as in rails.
      # Minus in front of closing or after opening '%' suppresses newlines.
      template.gsub! /-%>\n/, '%>'
      template.gsub! /\n<%-/, '<%'
      
      ERB.new(template).result(binding)
    end
  end

  #
  # Regular expressions used below.
  #
  # Stored here for better performance
  #
  
  # Matches e.g. SUBROUTINE DAXPY(N,DA,DX,INCX,DY, INCY)
  SubroutineDecl = 
    Regexp.compile /SUBROUTINE ([A-Z0-9]+)\(\ *([A-Z0-9, ]+)\ *\)/
  
  # Matches e.g. DOUBLE PRECISION FUNCTION DDOT(N,DX,INCX,DY,INCY)
  FunctionDecl = 
    Regexp.compile /\ +([A-Z ]+) FUNCTION ([A-Z0-9]+)\(\ *([A-Z0-9, ]+)\ *\)/ 
  
  # Matches e.g. INTEGER INCX,INCY,N
  VariableDecl = 
    Regexp.compile /\ +(#{Fortran.types_as_regexp})\ +([A-Z,()*0-9 ]+)/
    
  # Matches e.g. *  LDA     (input) INTEGER
  MetaComment = 
    Regexp.compile /\A\*\s*([A-Z0-9,]+)\s+\(([a-zA-Z\/]*)\)/
  
  # Matches e.g. A, but also DX(*)
  ArgumentParens = 
    Regexp.compile /[A-Z0-9]+(?: *\([A-Z, 0-9*]+\))?/
  

  # Parse a FORTRAN file, return a Routine object describing the
  # defined object.
  #
  # This function is really tailored towards the blas and lapack
  # files, although it can in principle parse all FORTRAN types. In
  # addition, it parses comments to look for metadata, in particular
  # whether a variable is used as input or output. Lines like
  #
  #
  #    *  JOBVL   (input) CHARACTER*1
  #
  # are parsed and the information in parenthesis is extracted.
  def self.parse_file(f)
    routine = nil
    lines = f.readlines
    lines = Fortran.parse_continuations(lines)

    lines.each do |l|
      #puts l
      case l
        #
        # parse headings, e.g.
        #  
        #     DOUBLE PRECISION FUNCTION DDOT(N,DX,INCX,DY,INCY)
        #     SUBROUTINE DAXPY(N,DA,DX,INCX,DY,INCY)
        #
      when SubroutineDecl 
        name = $1
        args = $2.scan /[A-Z0-9]+/
        routine = Fortran.subroutine name, args
      when FunctionDecl
        return_type = $1
        name = $2
        args = $3.scan /[A-Z0-9]+/
        routine = Fortran.function return_type, name, args
        #
        # parse argument types, e.g.
        #
        #      DOUBLE PRECISION DX(*),DY(*)
        #      COMPLEX*16       A( LDA, * ), VL( LDVL, * ), VR( L...
        #
      when VariableDecl
          puts "#$1 -> #$2" if $debug
        type = $1
        args = $2.scan ArgumentParens 
        args.each do |argname| 
          puts "  #{argname} -> #{type}" if $debug
          if argname =~ /([A-Z0-9]+)\ *\(.*\)/
            argname = $1
            array = true
          else
            array = false
          end
          if routine.args.member? argname
            routine.argtype[argname] = Fortran::FortranType.new(type, array)
          end
        end
        # 
        # parse comments which with respect to input/output, e.g.
        #
        # *  JOBVL   (input) CHARACTER*1
        #
      when MetaComment
        args = $1; comment = $2
        #puts "#{args} -> #{comment}"
        args.split(',').each do |argname|
          at = routine.argtype[argname]
          if at
            at.comment = comment
          else
            puts "Warning: cannot add comment to variable #{argname} in file #{f.path} line \"#{l}\" (argument not defined)"
          end
        end
      end
    end
    return routine
  end
end
