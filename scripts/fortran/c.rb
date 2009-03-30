## --- BEGIN LICENSE BLOCK ---
# Copyright (c) 2009, Mikio L. Braun
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
# 
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
# 
#     * Neither the name of the Technische Universität Berlin nor the
#       names of its contributors may be used to endorse or promote
#       products derived from this software without specific prior
#       written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
## --- END LICENSE BLOCK ---

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
