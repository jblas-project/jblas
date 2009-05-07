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

require 'config/path'
require 'config/config'
require 'config/opts'

# I shouldn't assume that things have certain layout but instead just try
# to find the resources:
#
# - LAPACK and BLAS sources if I want to recompile (otherwise, not)
# - lapack_build: complete lapack and blas libraries
# - atlas_build: complete lapack libraries, atlas libraries

#Todo:
#  - find/download/compile LAPACK
#  - find ATLAS
#  - write out stuff to .config
# EOS

$opts = Opts.new(ARGV, {}, <<EOS)
Usage: ./configure [options]

options summary:
  --lapack=DIR             lapack-lite directory
  --lapack-build           build against fortran lapack instead of ATLAS
  --libpath=DIR1:DIR2:...  comma separated list of directories to contain
                           the ATLAS libraries
  --download-lapack        try to download and compile lapack if not
                           found
  --static-libs            look for static libraries only. Results in a
                           dynamically loaded jblas library which does
                           not depend on lapack or atlas libraries. 
                           (default for Windows!)
EOS

config = Config.new

# translate dir (mainly necessary for cygwin)
def dir(s)
  case $os_name
  when 'Windows'
    s = s.gsub /\\/, '\\\\\\\\'
    %x(cygpath -u #{s}).chomp
  else
    s # safe default... 
  end
end

def libname(name)
  if $opts.defined? :static_libs
    'lib' + name + '.a'
  else
    case $os_name
    when 'Linux'
      'lib' + name + '.so'
    when 'SunOS'
      'lib' + name + '.so'
    when 'Windows'
      'lib' + name + '.a'
    end
  end
end

# returns an array of the symbols defined in the library +fn+.
def libsyms(fn)
  nmopt = File.extname(fn) == '.so' ? '-D' : ''
  %x(nm -p #{nmopt} #{fn}).grep(/ T _?([a-zA-Z0-9_]+)/) {|m| $1}
end

# indent a multiline text
class String
  def indent(cnt)
    split("\n").map {|l| ' ' * cnt + l}.join("\n")
  end
end

begin
  ######################################################################
  config.msg('checking for java, javac') do 
    config.check_cmd('java', 'javac')
    %x(javac config/PrintProperty.java)
    nil
  end

  ######################################################################
  config.msg('checking for nm') do
    config.check_cmd 'nm'
  end

  ######################################################################
  config.msg('determining operating system') do
    $os_name = %x(java -cp config PrintProperty os.name).chomp
    if $os_name.start_with? 'Windows'
      $os_name = 'Windows'
    end
    if $os_name == 'Windows'
      config.msg 'checking for cygpath' do
        config.check_cmd 'cygpath'
      end
    end
    config['OS_NAME'] = $os_name
    config.add_xml "<property name=\"os_name\" value=\"#{$os_name}\" />"
    $os_name
  end

  config.msg('determining architecture') do
    $os_arch = %x(java -cp config PrintProperty os.arch).chomp
    config['OS_ARCH'] = $os_arch
    config.add_xml "<property name=\"os_arch\" value=\"#{$os_arch}\" />"
    $os_arch
  end

  ######################################################################
  config.msg('locating the Java Development Kit') do
    $java_home = dir(File.dirname(%x(java -cp config PrintProperty java.home)))
    p $java_home
    config.check_files $java_home, ['include', 'jni.h'] do
      config['JAVA_HOME'] = $java_home
    end
  end
  
  ######################################################################
  config.msg('deciding whether to use g77 or gfortran') do
    g77 = where('g77')
    gfortran = where('gfortran')
    f77 = where('f77')
    if g77
      config['LD'] = 'g77'
    elsif gfortran
      config['LD'] = 'gfortran'
    elsif f77
      config['LD'] = 'f77'
    else
      config.fail <<EOS.indent 2
Either g77 or gfortran have to be installed to compile against the
fortran libraries.
EOS
    end
  end
  
  ######################################################################
  config.msg('Setting up gcc and flags') do
    config.check_cmd('gcc', 'make', 'ld')
    case $os_name
    when 'Linux'
      config << <<EOS
CC = gcc
CFLAGS = -fPIC
INCDIRS += -Iinclude -I#{$java_home}/include -I#{$java_home}/include/linux
SO = so
LIB = lib
RUBY=ruby
LDFLAGS += -shared
EOS
    when 'SunOS'
      config << <<EOS
CC = gcc
CFLAGS = -fPIC
INCDIRS += -Iinclude -I#{$java_home}/include -I#{$java_home}/include/solaris
SO = so
LIB = lib
RUBY=ruby
LDFLAGS += -G
EOS
    when 'Windows'
      config.check_cmd('cygpath')
      config << <<EOS
CC = gcc
CFLAGS = -ggdb -D__int64='long long'
INCDIRS += -I"#{dir $java_home}/include" -I"#{dir $java_home}/include/win32" -Iinclude
LDFLAGS += -mno-cygwin -shared -Wl,--add-stdcall-alias
SO = dll
LIB = 
RUBY = ruby
EOS
    else
      config.fail "Sorry, the OS #{$os_name} is currently not supported"
    end
  end

  ######################################################################
  config.msg('looking for version of make') do
     if where('gmake')
	puts 'gmake'
        config['MAKE'] = 'gmake'
	config.add_xml '<property name="make" value="gmake" />'
     else
        if where_with_output('make -v', /GNU Make/).nil?
      	  config.fail('I need GNU make to run...')
	end
        config['MAKE'] = 'make'
	config.add_xml '<property name="make" value="make" />'
     end
  end

  ######################################################################
  def check_lapack_home(config)
    config.check_files($lapack_home, 
                       ['BLAS', 'SRC', 'dgemm.f'], 
                       ['SRC', 'dsyev.f']) do
      config['LAPACK_HOME'] = $lapack_home
    end
  end
  
  config.msg('locating lapack sources') do
    $lapack_home = $opts.get :lapack, './lapack-lite-3.1.1'
    begin
      check_lapack_home(config)
    rescue ConfigError => e
      if $opts.defined? :download_lapack
        puts "trying to download lapack (about 5M)"
        lapack_tgz = File.join('.', 'lapack-lite-3.1.1.tgz')
        File.delete(lapack_tgz) if File.exist?(lapack_tgz)
        system("wget http://www.netlib.org/lapack/lapack-lite-3.1.1.tgz")
        system("tar xzvf lapack-lite-3.1.1.tgz")
        check_lapack_home(config)
      else
        puts <<EOS.indent 2
Couldn\'t locate sources for LAPACK and BLAS. Supply the location
of the lapack sources with the --lapack=... switch. Sources for lapack can be
found at http://www.netlib.org/lapack/lapack-lite-3.1.1.tgz or try
running configure with the --download-lapack option.

Compling jblas should work, but unless you have the LAPACK and BLAS sources
you cannot add further stubs.
EOS
        config['LAPACK_HOME'] = ''
        $lapack_home = ''
      end
    end
    $lapack_home
  end

  ######################################################################
  #
  # Locating the libraries
  #
  # This is a bit involved since the standard ATLAS compilation produces
  # a liblapack.so which only implements a subset of the lapack functions.
  # So the basic strategy is to
  # 
  # (a) find the path to the atlas specific library
  # (b) find the path to a full lapack library
  #
  # Pass the lapack libraries and theirs paths first.
  #
  # Of course, if you want to build a lapack-only path, we only search
  # for the full lapack libraries.

  if $opts.defined? :libpath
    LIBPATH = $opts[:libpath].split(':')
  else
    LIBPATH = %w(/usr/lib /lib /usr/lib/sse2)
  end

  $libpaths = []
  
  # Like Set, but retains order in which elements were added
  def addpath(path)
    $libpaths << path unless $libpaths.include?
  end
  
  def locate_lib(config, name, symbol=nil)
    p = where(libname(name), LIBPATH) do |fn| 
      symbol.nil? or libsyms(fn).include? symbol
    end

    if not p
      config.fail("couldn't find library '#{name}' in\npath #{LIBPATH.join ':'}")
    end
    
    config.log "found library #{name} in #{p}"
    return p
  end

  # Tries to find one of the libraries +names+ in LIBPATH,
  # potentially containing the +symbol+.
  # Returns the path and the actual name of the library
  def locate_one_of_libs(config, names, symbol=nil)
    p = nil
    l = nil
    config.log "Searching for one of #{names.join ', '} in #{LIBPATH.join ':'}#{if symbol then ' having symbol ' + symbol.to_s end}"
    for name in names
      config.log "  Searching for #{libname(name)}"
      p = where(libname(name), LIBPATH) do |fn| 
        symbol.nil? or libsyms(fn).include? symbol
      end

      if p
        l = name
        config.log "Found at #{l} at #{p}"
        break
      end
    end

    if not p
      config.log "Haven't found any of #{names.join ', '}!"
      config.fail("couldn't find library '#{name}' in\npath #{LIBPATH.join ':'}")
    end

    config.log "found library #{l} in #{p}"
    return p, l
  end

  ######################################################################
  config.msg('locating lapack and blas libraries') do 
    begin      
      $lapack_path, $lapack_name = 
        locate_one_of_libs(config, ['lapack_fortran', 'lapack'], 'dsyev_')
      $blas_path, $blas_name = 
        locate_one_of_libs(config, ['blas_fortran', 'blas', 'f77blas'], 'daxpy_')
    rescue ConfigError => e
      config.fail <<EOS.indent 2
Couldn\'t locate LAPACK and BLAS libraries.

Reason: #{e.message}

You can try to pass the location via --libpath=...
or build your own LAPACK and BLAS libraries (see INSTALL)
EOS
    end
    $libpaths << $lapack_path
    $libpaths << $blas_path
    nil
  end
  
  unless $opts.defined? :lapack_build
    config.msg('locating atlas libraries') do
      begin
        $atlas_path = locate_lib(config, 'atlas')
        $lapack_atlas_path, $lapack_atlas_name = 
          locate_one_of_libs(config, ['lapack_atlas', 'lapack'], 'ATL_dgetri')
        $cblas_path = locate_lib(config, 'cblas', 'cblas_daxpy')
      rescue ConfigError => e
        config.fail <<EOS.indent 2
Couldn\'t locate ATLAS libraries.

Reason: #{e.message}

You can try to pass the location via --libpath=...
or build your own ATLAS libraries (see INSTALL)
EOS
      end
      $libpaths << $atlas_path
      $libpaths << $lapack_atlas_path
      $libpaths << $cblas_path
      nil
    end
  end
  

  ######################################################################
  # Some sanity checks, in particular that ATLAS's and LAPACK's lapack
  # library doesn't have the same name... . 
  if $lapack_name == $lapack_atlas_name
    config.fail <<EOS.indent 2
The full lapack library and the one from ATLAS have the same name which
makes it impossible to link in both. Either fiddle with --libpath=... or
try renaming some of the libraries:

lapack's #{libname $lapack_name} to #{libname 'lapack_fortran'}
ATLAS' #{libname $lapack_name} to #{libname 'lapack_atlas'}
EOS
  end

  # Okay, then we're done!
  config << "LDFLAGS += #{$libpaths.map {|s| '-L' + s}.join ' '}"

  if $opts.defined? :lapack_build
    loadlibes = [$lapack_name, $blas_name]
  else
    loadlibes = [$lapack_atlas_name, $lapack_name, $blas_name, 'cblas', 'atlas']
  end

  if $opts.defined? :static_libs
    config << "LOADLIBES = -Wl,--allow-multiple-definition #{loadlibes.map {|l| ' -l:lib' + l + '.a'}} -l:libgfortran.a"
  else
    config << "LOADLIBES =#{loadlibes.map {|l| ' -l' + l}}"
  end

  ######################################################################
  # dumping results
  puts
  puts 'Configuration succesfull, writing out results to configure.out'
  open('configure.out', 'w') {|f| config.dump f}
  open('configure.xml', 'w') {|f| config.dump_xml f}

rescue ConfigError => e
  puts 
  puts "Configuration failed!"
  puts
  puts "Reason: #{e.message}"
end
