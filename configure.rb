require 'config/path'
require 'config/config'
require 'config/opts'

#Todo:
#  - find/download/compile LAPACK
#  - find ATLAS
#  - write out stuff to .config
# EOS

opts = Opts.new(ARGV)

config = Config.new

begin
  ######################################################################
  config.msg('checking for java, javac') do 
    config.check_cmd('java', 'javac')
    %x(javac config/PrintProperty.java)
    nil
  end

  ######################################################################
  config.msg('locating the Java Development Kit') do
    $java_home = File.dirname(%x(java -cp config PrintProperty java.home))
    config.check_files $java_home, ['include', 'jni.h'] do
      config['JAVA_HOME'] = $java_home
    end
  end

  ######################################################################
  config.msg('Determining operating system') do
    $os_name = %x(java -cp config PrintProperty os.name).chomp
    $os_name
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
  config.msg('locating lapack sources') do
    $lapack_home = opts.get :lapack, './lapack-lite-3.1.1'
    begin
      config.check_files($lapack_home, 
                         ['BLAS', 'SRC', 'dgemm.f'], 
                         ['SRC', 'dsyev.f']) do
        config['LAPACK_HOME'] = $lapack_home
      end
    rescue ConfigError => e
      raise ConfigError, <<EOS
Couldn\'t locate sources for LAPACK and BLAS. Sources for lapack can be
found at http://www.netlib.org/lapack/lapack-lite-3.1.1.tgz
EOS
    end
    $lapack_home
  end
  
  ######################################################################
  config.msg('locating atlas libraries') do
    LIBPATH = opts.get :libpath, %w(/usr/lib /lib /usr/lib/sse2)
    ATLASLIBS = %w(libf77blas.so libatlas.so liblapack.so libblas.so liblapack.so)
    $atlas_libs = collect_paths(ATLASLIBS, LIBPATH)
    unless $atlas_libs
      config.fail <<EOS
Couldn\'t locate the ATLAS libraries #{ATLASLIBS.join(', ')}
EOS
    else
      p $atlas_libs
      config['LDFLAGS'] <<= $atlas_libs.map {|p| '-L' + p}.join(' ')
    end
  end
  
  ######################################################################
  config.msg('Setting up gcc and flags') do
    config.check_cmd('gcc', 'make', 'ld')
    case $os_name
    when 'Linux'
      config['CC'] = 'gcc'
      config['CFLAGS'] = '-fPIC -ggdb'
      config['INCDIRS'] <<= '-Iinclude'
      config['SO'] = 'so'
      config['LIB'] = 'lib'
      config['RUBY'] = 'ruby'
    when 'WinXP'
      config.check_cmd('cygpath')
      config['CC'] = gcc
      config['CFLAGS'] = "-ggdb -D__int64='long long'"
      javadir = %x(cygpath -u #{$java_home})
      atlasdir = %x(cygpath -u #{$atlas_home})
      lapackir = %x(cygpath -u #{$lapack_home})
      config['INCDIRS'] <<= "-I\"#{javadir}/include\" -I\"#{javadir}/include/win32\" -Iinclude"
      config['LDFLAGS'] <<= "-mno-cygwin -shared -Wl,--add-stdcall-alias -L#{atlasdir} -L#{lapackdir})"
      config['SO'] = 'dll'
      config['LIB'] = ''
      config['RUBY'] = 'ruby'
    else
      puts "Sorry, the OS #{$os_name} is currently not supported"
    end
  end

  ######################################################################
  # dumping results
  open('.config', 'w') {|f| config.dump f}

rescue ConfigError => e
  puts 
  puts "Configuration failed!"
  puts
  puts "Reason: #{e.message}"
end
