require 'config/path'
require 'config/config'
require 'config/opts'

#Todo:
#  - find/download/compile LAPACK
#  - find ATLAS
#  - write out stuff to .config
# EOS

opts = Opts.new(ARGV, {}, <<EOS)
Usage: ./configure [options]

options summary:
  --lapack=DIR       lapack-lite directory
  --lapack_build     build against fortran lapack instead of ATLAS
  --libpath          comma separated list of directories to contain
                     the ATLAS libraries
  --build-lapack     try to compile lapack if not found
  --download-lapack  try to download and compile lapack if not
                     found
EOS

config = Config.new

def dir(s)
  case $os_name
  when 'Linux'
    s
  when 'Windows XP'
    s = s.gsub /\\/, '\\\\\\\\'
    %x(cygpath -u #{s}).chomp
  else
    raise ConfigError, "The OS #{$os_name} is currently unsupported!"
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
  config.msg('determining operating system') do
    $os_name = %x(java -cp config PrintProperty os.name).chomp
    if $os_name == 'Windows XP'
      config.msg 'checking for cygpath' do
        config.check_cmd 'cygpath'
      end
    end
    $os_name
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
    if g77
      config['LD'] = 'g77'
    elsif gfortran
      config['LD'] = 'gfortran'
    else
      config.fail <<EOS
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
CFLAGS = -fPIC -ggdb
INCDIRS += -Iinclude -I#{$java_home}/include -I#{$java_home}/include/linux
SO = so
LIB = lib
RUBY=ruby
LDFLAGS += -shared
EOS
    when 'Windows XP'
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
  unless opts.defined? :lapack_build
     config.msg('locating atlas libraries') do
      if opts.defined? :libpath
        LIBPATH = opts[:libpath].split(',')
      else
        LIBPATH = %w(/usr/lib /lib /usr/lib/sse2)
      end
      
      # Choose static libraries for Windows XP
      case $os_name
      when 'Windows XP'
        ATLASLIBS = %w(libf77blas.a libatlas.a liblapack.a libcblas.a)
      else
        ATLASLIBS = %w(libf77blas.so libatlas.so liblapack.so libcblas.so)
      end
      
      $atlas_libs = collect_paths(ATLASLIBS, LIBPATH, config)
      unless $atlas_libs
        config.fail <<EOS
Couldn\'t locate the ATLAS libraries #{ATLASLIBS.join(', ')}. You can
try to pass the location via --libpath=...
EOS
      else
        config['LDFLAGS'] <<= $atlas_libs.map {|p| '-L' + dir(p)}.join(' ')
        config['LOADLIBES'] <<= '-llapack -lf77blas -lcblas -latlas'
      end
      nil
    end
  end
  
  ######################################################################
  def check_lapack_home(config)
    config.check_files($lapack_home, 
                       ['BLAS', 'SRC', 'dgemm.f'], 
                       ['SRC', 'dsyev.f']) do
      config['LAPACK_HOME'] = $lapack_home
      config['LDFLAGS'] <<= '-L' + dir($lapack_home)
    end
  end
  
  config.msg('locating lapack sources') do
    $lapack_home = opts.get :lapack, './lapack-lite-3.1.1'
    begin
      check_lapack_home(config)
    rescue ConfigError => e
      if opts.defined? :download_lapack
        puts "trying to download lapack (about 5M)"
        File.delete(File.join('.', 'lapack-lite-3.1.1.tgz'))
        system("wget http://www.netlib.org/lapack/lapack-lite-3.1.1.tgz")
        system("tar xzvf lapack-lite-3.1.1.tgz")
        check_lapack_home(config)
      else
        raise ConfigError, <<EOS
Couldn\'t locate sources for LAPACK and BLAS. Sources for lapack can be
found at http://www.netlib.org/lapack/lapack-lite-3.1.1.tgz or try
running configure with the --download-lapack option.
EOS
      end
    end
    $lapack_home
  end

  ######################################################################
  LAPACK_LIBS = [['libblas-fortran.a'], ['liblapack-fortran.a']]
  config.msg('checking for LAPACK and BLAS libraries') do
    begin
      config.check_files($lapack_home, *LAPACK_LIBS)
    rescue ConfigError => e
      if opts.defined? :build_lapack
        config.msg("trying to compile LAPACK and BLAS libraries") do
          out = %x(bash config/compile_lapack #{$lapack_home})
          open('configure.compile_log', 'w') {|o| o.print out}
        end
      else
        raise ConfigError, <<EOS
Couldn\'t find LAPACK libraries. Try running with the --build-lapack option.
EOS
      end
    end
    config.check_files($lapack_home, *LAPACK_LIBS) do
      config['LOADLIBES'] <<= '-llapack-fortran -lblas-fortran'
    end
  end


  ######################################################################
  # dumping results
  puts
  puts 'Configuration succesfull, writing out results to configure.out'
  open('configure.out', 'w') {|f| config.dump f}

rescue ConfigError => e
  puts 
  puts "Configuration failed!"
  puts
  puts "Reason: #{e.message}"
end
