module Fortran

  class FortranType
    # extend FortranType by converters to java and C
    ArrayTypeMap = { 'REAL*4' => 'float[]',
      'REAL*8' => 'double[]',
      'INTEGER*2' => 'short[]',
      'INTEGER*4' => 'int[]',
      'INTEGER*8' => 'long[]',
      'LOGICAL*1' => 'byte[]',
      'LOGICAL*2' => 'short[]',
      'LOGICAL*4' => 'int[]',
      'LOGICAL*8' => 'long[]',
      'COMPLEX*8' => 'float[]',
      'COMPLEX*16' => 'double[]',
      'VOID' => 'void'
    }
    
    StdTypeMap = { 'CHARACTER' => 'char',
      'CHARACTER*1' => 'char',
      'CHARACTER*N' => 'String',
      'REAL*4' => 'float',
      'REAL*8' => 'double',
      'INTEGER*2' => 'short',
      'INTEGER*4' => 'int',
      'INTEGER*8' => 'long',
      'LOGICAL*1' => 'byte',
      'LOGICAL*2' => 'short',
      'LOGICAL*4' => 'int',
      'LOGICAL*8' => 'long',
      'COMPLEX*8' => 'ComplexFloat',
      'COMPLEX*16' => 'ComplexDouble',
      'VOID' => 'void'
    }
    
    # Map a Fortran type to Java type or throw an ArgumentError
    def to_java
      if basetype =~ /CHARACTER\*[0-9]+/
        self.basetype = 'CHARACTER*N'
      end
      
      result = if array
                 ArrayTypeMap[basetype]
               else
                 StdTypeMap[basetype]
               end

      if not result
        raise ArgumentError, "Don't know how to convert '#{to_s}'"
      end
      
      return result
    end
    
    def to_c
      javatype = to_java
      if javatype =~ /\[\]$/
        'j' + javatype[0...-2] + 'Array'
      elsif javatype == 'void'
        'void'
      elsif javatype =~ /Complex/
        'jobject'
      else
        'j' + javatype.downcase
      end
    end
  end

  # This module contains code for generating Java native declarations
  # for Fortran routines.
  module Java

    # Generate
    def self.generate(template_file, package, class_name, routines)
      g = Generator.new(package, class_name, routines)
      g.generate(template_file)
    end

    ######################################################################

    # A special TemplateContext for Java. In addition, the package
    # name and classname is stored.
    #
    # Usually, you would use the generate method instead of explicitly
    # constructing an object
    class Generator < TemplateContext
      attr_reader :package, :classname, :routines

      # Create a new Generator context. You can use generate
      # to directly construct and write a template file
      def initialize(package, classname, routines)
        @package = package
        @classname = classname
        @routines = routines
      end

      def generate_native_declaration(r)
        writer = WrapperCodeGenerator.new(header_file_name, r)
        writer.native_declaration
      end

      def generate_wrapper(r)
        writer = WrapperCodeGenerator.new(header_file_name, r)
        writer.wrapper
      end

      def header_file_name
        package.tr('.', '_') + '_' + classname
      end
    end
    
    ######################################################################

    # Generator of wrapper code for one routine, both for Java
    # (native_declaration) and C (wrapper)
    class WrapperCodeGenerator
      attr_reader   :prefix
      attr_accessor :return_type, :fct_name, :decl_args
      attr_accessor :fortran_args, :fortran_return_type
      attr_accessor :conversions
      attr_accessor :call_pre, :call_post, :fortran_fct_name, :call_args
      attr_accessor :arrays
      
      def initialize(prefix, r)
        @package = prefix
        @routine = r
        @return_type = ''
        @fct_name = prefix + '_' + r.name.downcase
        @decl_args = ''
        @conversions = ''
        @fortran_args = []
        @fortran_return_type = ''
        @call_pre = ''
        @fortran_fct_name = ''
        @call_args = []
        @call_post = ''
        @arrays = [] # already seen arrays
      end
      
  public
      # Generate the JNI-wrapper code for the fortran routine
      def wrapper
        make_fortran_fct_name r.name

        code_for_return_type(r.return_type)

        r.each_arg do |name, type| 
          code_for_argument(name, type)
        end

        return <<EOS
JNIEXPORT #{return_type} JNICALL Java_#{fct_name}(JNIEnv *env, jclass this#{decl_args})
{
  extern #{fortran_return_type} #{fortran_fct_name}(#{fortran_args.join(', ')});
  
#{conversions}
  savedEnv = env;
  #{call_pre}#{fortran_fct_name}(#{call_args.join(', ')});
#{call_post}
}
EOS
      end
      
      # Generate the native function declaration within a Java class.
      def native_declaration
        begin
          return_type = ''
          fct_name = r.name.downcase
          args = []
          
          if r.return_type.basetype == 'VOID' and r.args.last == 'INFO'
            return_type = 'int'
          else
            return_type = r.return_type.to_java
          end
          
          r.args.each do |name|
            type = r.argtype[name]
            args << type.to_java + " " + name.downcase unless name == 'INFO'
            if type.to_java =~ /\[\]/
              args << "int #{name.downcase}Idx"
            end
          end
          "  public static native #{return_type} #{fct_name}(#{args.join(", ")});"
        rescue
          puts
          puts "---Error for routine #{@routine}"
          puts
          raise
        end
      end

  private ############################################################

      # convenience accessor
      def r; @routine; end

      def make_fortran_fct_name name
        fortran_fct_name << name.downcase + '_'
      end

      def code_for_return_type type
        javatype = type.to_java
        ctype = type.to_c

        gen = generator(type)
        gen.set_type(type, javatype, ctype)

        gen.make_return_type
        gen.make_fortran_return_type
        gen.make_convert_return_type
        gen.make_call_return
      end

      def code_for_argument(name, type)
        javatype = type.to_java
        ctype = type.to_c
        name = name.downcase

        gen = generator(type, name)
        gen.set_type(type, javatype, ctype)
        gen.set_name(name)

        gen.make_decl_arg
        gen.make_fortran_arg
        gen.make_convert_arg
        gen.make_call_arg
      end

      # find the correct code generator for the given type
      def generator(type, name=nil)
        javatype = type.to_java
        if javatype == 'void'
          Java::NilArgument.new(self)
        elsif javatype =~ /\[\]/
          Java::BufferArgument.new(self)
        elsif javatype =~ /Complex/
          Java::ComplexArgument.new(self)
        elsif javatype == 'char'
          Java::CharArgument.new(self)
        elsif javatype == 'int' and name == 'info' and @routine.return_type.basetype == 'VOID'
          Java::InfoArgument.new(self)
        else
          Java::GenericArgument.new(self)
        end
      end
    end

    ######################################################################
    #
    # Actual code generators
    #
    
    # The default case: Just convert types to their java and C
    # equivalents, pass the reference to the fortran routine and 
    # return the result 
    class GenericArgument
      attr_reader :code, :type, :javatype, :ctype, :name

      def initialize(code)
        @code = code
      end

      def set_type(type, javatype, ctype)
        @type = type
        @javatype = javatype
        @ctype = ctype
      end

      def set_name(name)
        @name = name
      end

      def make_fortran_return_type
        if ctype == 'jfloat'
          code.fortran_return_type << 'jdouble'
        else
          code.fortran_return_type << ctype
        end
      end

      def make_return_type
        code.return_type << ctype
      end

      def make_convert_return_type
      end

      def make_call_return
        code.call_pre << ctype + " retval = "
        if ctype == 'jfloat'
          code.call_post << "\n  return (jdouble) retval;"
        else
          code.call_post << "\n  return retval;"
        end
      end

      def make_decl_arg
        code.decl_args << ", " + ctype + " " + name
      end

      def make_fortran_arg
        code.fortran_args << javatype + " *"
      end
      
      def make_convert_arg
      end
      
      def make_call_arg
        code.call_args << "&#{name}"
      end
    end

    # For arrays: Add an additional argument to the declaration,
    # and get the array from the direct buffer
    class BufferArgument < GenericArgument
      def make_decl_arg
        super
        code.decl_args << ", jint " + name + "Idx"
      end

      def make_fortran_arg
        code.fortran_args << ctype[1...-5] + " *"
      end
      
      # Okay, my apologies for this method. What makes it so difficult
      # is that it can happen that you pass in an array twice. If you
      # then call Get<Type>ArrayElements twice you get two copies
      # and any attempt to do something truly inplace won't work (for
      # example dswap). So I have to check before each array if 
      # I maybe already have it. And the same thing around in the end... .
      def make_convert_arg
        basectype = ctype[0...-5]
        code.conversions << <<EOS + '    '
  #{basectype} *#{name}PtrBase = 0, *#{name}Ptr = 0;
  if (#{name}) {
EOS
        unless code.arrays.empty?
          code.arrays.each do |a, t|
            if t == basectype
              code.conversions << "if((*env)->IsSameObject(env, #{name}, #{a}) == JNI_TRUE)\n      #{name}PtrBase = #{a}PtrBase;\n    else\n      "
            end
          end
        end
        code.conversions << <<EOS
#{name}PtrBase = (*env)->Get#{basectype[1..-1].capitalize}ArrayElements(env, #{name}, NULL);
    #{name}Ptr = #{name}PtrBase + #{'2*' if type.basetype =~ /COMPLEX/}#{name}Idx;
  }
EOS
        
        # and releasing the stuff again...
        release = "  if(#{name}PtrBase"
        code.arrays.each do |a, t| 
          if t == basectype
            release << " && #{name}PtrBase != #{a}PtrBase"
          end
        end
        release << ") {" + <<EOS
        
    (*env)->Release#{basectype[1..-1].capitalize}ArrayElements(env, #{name}, #{name}PtrBase, 0);
  }
EOS
        code.call_post = release + code.call_post
        
        # store information about the arrays we have already handled
        code.arrays << [name, basectype]
      end

      def make_call_arg
        code.call_args << "#{name}Ptr"
      end
    end

    # For complex values (scalars only!): fortran returns the value in the
    # first argument, therefore declare the return value, modify the 
    # fortran argument list, and generate the *Complex object.
    #
    # for arguments, extract the values from the Java object
    class ComplexArgument < GenericArgument
      def make_fortran_return_type
        code.fortran_args << javatype + " *"
        code.fortran_return_type << "void"
      end

      def make_convert_return_type
        code.conversions << "  #{javatype} retval;\n"
      end

      def make_call_return
        code.call_post << "\n  return create#{javatype}(env, &retval);"
        code.call_args << "&retval"
      end

      def make_convert_arg
        code.conversions << "  #{javatype} #{name}Cplx;\n"
        code.conversions << "  get#{javatype}(env, #{name}, &#{name}Cplx);\n"
      end

      def make_call_arg
        code.call_args << "&#{name}Cplx"
      end
    end
    
    # For characters: This handles only the first byte. Now idea if it
    # is really worth dealing with UTF-8 and all the rest...
    class CharArgument < GenericArgument
      def make_convert_arg
        code.conversions << "  char #{name}Chr = (char) #{name};\n"
      end

      def make_call_arg
        code.call_args << "&#{name}Chr"
      end
    end
    
    # For nil arguments (only return value): don't add a return value
    class NilArgument < GenericArgument
      def make_call_return
      end
    end

    # For info arguments
    class InfoArgument < GenericArgument
      def make_decl_arg
        code.return_type = 'jint'
      end

      def make_convert_arg
        code.conversions << "  int info;\n"
        code.call_post << "\n  return info;"
      end
    end
  end # module Java
end # module Fortran
