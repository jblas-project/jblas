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

require 'config/config'
require 'config/path'
require 'config/config_os_arch'

module LibHelpers
  module_function

  def libname(name)
    case JblasConfig::CONFIG['LINKAGE_TYPE']
    when 'static'
      'lib' + name + '.a'
    when 'dynamic'
      case JblasConfig::CONFIG['OS_NAME']
      when 'Linux'
        'lib' + name + '.so'
      when 'SunOS'
        'lib' + name + '.so'
      when 'Windows'
        'lib' + name + '.a'
      when 'Mac\ OS\ X'
        'lib' + name + '.dylib'
      else
        JblasConfig.fail "Sorry, OS '#{JblasConfig::CONFIG['OS_NAME']}' is not supported yet..."
      end
    else
      raise "LINKAGE_TYPE should be either dynamic or static, but is #{JblasConfig::CONFIG['LINKAGE_TYPE']}"
    end
  end

  # returns an array of the symbols defined in the library +fn+.
  def libsyms(fn)
    nmopt = File.extname(fn) == '.so' ? '-D' : ''
    %x(#{JblasConfig::CONFIG['NM']} -p #{nmopt} #{fn.escape}).split("\n").grep(/ T _?([a-zA-Z0-9_]+)/) {|m| $1}
  end

  def locate_lib(libpath, name, symbol=nil)
    p = Path.where(libname(name), libpath) do |fn|
      symbol.nil? or libsyms(fn).include? symbol
    end

    if not p
      JblasConfig.fail("couldn't find library '#{name}' in\npath #{libpath.join ':'}")
    end

    JblasConfig.log "found library #{name} in #{p}"
    return p
  end

  # Tries to find one of the libraries +names+ in LIBPATH,
  # potentially containing the +symbol+.
  # Returns the path and the actual name of the library
  def locate_one_of_libs(libpath, names, symbol=nil)
    p = nil
    l = nil
    JblasConfig.log "Searching for one of #{names.join ', '} in #{libpath.join ':'}#{if symbol then ' having symbol ' + symbol.to_s end}"
    for name in names
      JblasConfig.log "  Searching for #{libname(name)}"
      p = Path.where(libname(name), libpath) do |fn|
        symbol.nil? or libsyms(fn).include? symbol
      end

      if p
        l = name
        JblasConfig.log "Found at #{l} at #{p}"
        break
      end
    end

    if not p
      JblasConfig.log "Haven't found any of #{names.join ', '}!"
      JblasConfig.fail("couldn't find library '#{name}' in\npath #{LIBPATH.join ':'}")
    end

    JblasConfig.log "found library #{l} in #{p}"
    return p, l
  end

  # locate +libs+ in given +paths+ and look for the +symbols+
  #
  # What you get is a hash mapping libraries to their paths. If a symbol could
  # not be found, a Config::ConfigError is raised.
  def find_libs(paths, libs, symbols)
    found_symbols = {}
    paths.each do |path|
      libs.each do |lib|
        libfile = File.join(path, libname(lib))
        next unless test ?f, libfile
        syms = libsyms(libfile)
        symbols.each do |s|
          if syms.include? s
            found_symbols[s] ||= []
            found_symbols[s] << [path, lib]
          end
        end
      end
    end

    #p found_symbols

    not_found_symbols = symbols.reject {|s| found_symbols.include? s }
    unless not_found_symbols.empty?
      JblasConfig.fail "Could not locate libraries for the following symbols: #{not_found_symbols.join ', '}."
    end

    #found_symbols.each_pair {|k,v| printf "%20s: %s\n", k, v.inspect}
    result = {}
    found_symbols.each_pair do |k, v|
      result[v[0][1]] = v[0][0]
    end
    return result
  end
end

if __FILE__ == $0
  include LibHelpers
  symbols_needed = [
    'dsyev_', # eigenvalue function not yet included in ATLAS/LAPACK
    'ATL_dgetri', # an atlas-specific function
    'ATL_dgemm', 
    'dgemm_', # matrix-matrix multiplication
    'daxpy_', # blas-related function
    'xerbla_', # xerbla?
  ]

  paths = %w(/usr/lib/sse2 /usr/lib)

  libs = %w(atlas lapack blas f77blas cblas lapack_atlas)

  JblasConfig::CONFIG['BUILD_TYPE'] = 'static'
  JblasConfig::CONFIG['OS_NAME'] = 'Linux'
  p find_libs(paths, libs, symbols_needed)
end