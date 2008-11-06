module Fortran
  module C
    COMPLEX_TYPE = 'fortran_complext_t'
    DOUBLE_COMPLEX_TYPE = 'fortran_double_complex_t'

    class Generator < TemplateContext
      attr_reader :filename, :routines

      def initialize(filename, routines)
        @filename = filename
        @routines = routines
      end
    end

    def self.generate(template_file, filename, routines)
      g = Generator.new(filename, routines)
      g.generate(template_file)
    end

    def self.reference?(r, argname)
      argtype = r.argtype[argname]
      argtype and (argtype.array or argtype.comment =~ /output/)
    end
    
    # Generate declaration for wrapper in C.
    def self.declaration_for r
      name = r.name.downcase
      args = []

      if not r.rettype
        rettype = 'void'
      elsif r.rettype.basetype == 'COMPLEX*8'
        rettype = 'void'
        args << COMPLEX_TYPE + '* retval'
      elsif r.rettype.basetype == 'COMPLEX*16'
        rettype = 'void'
        args << DOUBLE_COMPLEX_TYPE + '* retval'
      else
        rettype = C.map_basetype(r.rettype)
      end

      r.args.each do |argname|
        argtype = r.argtype[argname]
        cargname = argname.downcase
        ctype = C.map_basetype(argtype)
        ctype << '*' if reference?(r, argname)
        args << ctype + ' ' + cargname
      end

      "#{rettype} #{name}(#{args.join ', '})"
    end
     
    def self.fortran_declaration_for r
      name = r.name.downcase + '_'
      args = []

      if not r.rettype
        rettype = 'void'
      elsif r.rettype.basetype == 'COMPLEX*8'
        rettype = 'void'
        args << COMPLEX_TYPE + '* retval'
      elsif r.rettype.basetype == 'COMPLEX*16'
        rettype = 'void'
        args << DOUBLE_COMPLEX_TYPE + '* retval';
      else
        rettype = C.map_basetype(r.rettype)
      end
      
      r.args.each do |argname|
        argtype = r.argtype[argname]
        argname = argname.downcase
        ctype = C.map_basetype(argtype) + '*';
        args << ctype + ' ' + argname
      end
    
      "#{rettype} #{name}(#{args.join ', '})"      
    end

    def self.call_fortran_routine r
      name = r.name.downcase + '_'

      args = []

      if not r.rettype
        retstmt = ''
      elsif r.rettype.basetype == 'COMPLEX*8'
        retstmt = ''
        args << 'retval'
      elsif r.rettype.basetype == 'COMPLEX*16'
        retstmt = ''
        args << 'retval'
      else
        retstmt = 'return '
      end
      
      r.args.each do |argname|
        if reference?(r, argname)
          args << argname.downcase
        else
          args << '&' + argname.downcase
        end
      end
    
      "#{retstmt}#{name}(#{args.join ', '})"      
    end

    # Generate the actual wrapper.
    def self.wrapper_for r
      return <<EOS
#{declaration_for r}
{
  extern #{fortran_declaration_for r};
  #{call_fortran_routine r};
}
EOS
    end

    def self.map_basetype(type)
      return 'void' unless type
      t = ''
      case type.basetype
      when 'CHARACTER' then t << 'char'
      when 'CHARACTER*1' then t << 'char'
      when /CHARACTER\*[0-9]+/ then t << 'char*'
      when 'REAL*4' then t << 'float'
      when 'REAL*8' then t << 'double'
      when 'INTEGER*2' then t << 'short'
      when 'INTEGER*4' then t << 'int'
      when 'INTEGER*8' then t << 'long'
      when 'LOGICAL*1' then t << 'byte'
      when 'LOGICAL*2' then t << 'short'
      when 'LOGICAL*4' then t << 'int'
      when 'LOGICAL*8' then t << 'long'
      when 'COMPLEX*8' then t << COMPLEX_TYPE
      when 'COMPLEX*16' then t << DOUBLE_COMPLEX_TYPE
      else
        raise ArgumentError, "Don't know how to convert '#{type}'"
      end
    end
  end
end
