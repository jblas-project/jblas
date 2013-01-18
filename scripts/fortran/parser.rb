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

# This file contains a parser which takes fortran file and parses out the
# function definition.
#
# Needless to say, the parser is highly adapted to the BLAS and LAPACK
# sources when it comes to extracting additional information from the
# comments (for example, which variables are input/output), but the
# extraction of the types should be rather generic.
#
# Main entry point is +parse_file+

require 'fortran/types'

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

  # Return a regular expression which matches against all Fortran::Types.
  def self.types_as_regexp
    Fortran::Types.join('|')
  end

  #
  # Regular expressions used in the parser
  #
  # Stored here for better performance
  #

  # Matches e.g. SUBROUTINE DAXPY(N,DA,DX,INCX,DY, INCY)
  SubroutineDecl =
    Regexp.compile /\A\s+SUBROUTINE ([A-Z0-9]+)\(\ *([A-Z0-9, ]+)\ *\)/

  # Matches e.g. DOUBLE PRECISION FUNCTION DDOT(N,DX,INCX,DY,INCY)
  FunctionDecl =
    Regexp.compile /\A\s+([A-Z ]+) FUNCTION ([A-Z0-9]+)\(\ *([A-Z0-9, ]+)\ *\)/

  # Matches e.g. INTEGER INCX,INCY,N
  VariableDecl =
    Regexp.compile /\A\s+(#{Fortran.types_as_regexp})\ +([A-Z,()*0-9 ]+)/

  # Matches e.g. *  LDA     (input) INTEGER
  MetaComment =
    Regexp.compile /\A\*\s*([A-Z0-9,]+)\s+\(([a-zA-Z\/]*)\)/

  # Matches e.g. A, but also DX(*)
  ArgumentParens =
    Regexp.compile /[A-Z0-9]+(?: *\([A-Z, 0-9*]+\))?/

  # Matches e.g. "*  LDA     - "
  BLASArgumentCommentStart =
    Regexp.compile /\A\*\s+([A-Z]+)\s+-\s/

  UnchangedOnExit =
    Regexp.compile /\A\*\s+Unchanged on exit./

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
    blas_arg = nil

    lines.each do |l|
      #puts '[' + l.tr(' ','.') + ']'
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
        puts "--- " + name if $debug
        routine = Fortran.subroutine name, args
      when FunctionDecl
        return_type = $1
        name = $2
        args = $3.scan /[A-Z0-9]+/
        routine = Fortran.function return_type, name, args
        puts "   FunctionDecl matched" if $debug
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
        type = type.sub(/\(\s+/, "(").sub(/\s+\)/, ")")
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
        puts "#{args} -> comment \"#{comment}\"" if $debug
        args.split(',').each do |argname|
          at = routine.argtype[argname]
          if at
            at.comment = comment
          else
            puts "Warning: cannot add comment to variable #{argname} in file #{f.path} line \"#{l}\" (argument not defined)"
          end
        end
        #
        # Parser beginning argument comments for BLAS
        #
      when BLASArgumentCommentStart
        blas_arg = $1
        puts ">> #{l} << #{$1}" if $debug
      when UnchangedOnExit
        unless blas_arg.nil?
          at = routine.argtype[blas_arg]
          if at
            puts "   adding (input)" if $debug
            at.comment = 'input'
          else
            puts "Warning: cannot add input flag to variable #{blas_arg} in file #{f.path} line #{l} (argument not defined)"
          end
          blas_arg = nil
        end
      end
    end
    return routine
  end
end
