package org.jblas;

import org.jblas.exceptions.UnsupportedArchitectureException;
import org.jblas.util.LibraryLoader;
import org.jblas.util.Logger;

/**
 * Help class for loading libraries needed for NativeBlas
 *
 * The only use of this class is to have NativeBlas inherit from this class.
 *
 * User: Mikio L. Braun
 * Date: 10/24/12
 * Time: 3:15 PM
 */
class NativeBlasLibraryLoader {
  static void loadLibraryAndCheckErrors() {
    try {
      try {
        loadDependentLibraries();
        System.loadLibrary("jblas");
      } catch (UnsatisfiedLinkError e) {
        Logger.getLogger().config(
            "BLAS native library not found in path. Copying native library "
                + "from the archive. Consider installing the library somewhere "
                + "in the path (for Windows: PATH, for Linux: LD_LIBRARY_PATH).");
        new LibraryLoader().loadLibrary("jblas", true, false);
      }
      // Let's do some quick tests to see whether we trigger some errors
      // when dependent libraries cannot be found
      double[] a = new double[1];
      NativeBlas.dgemm('N', 'N', 1, 1, 1, 1.0, a, 0, 1, a, 0, 1, 1.0, a, 0, 1);
    } catch (UnsatisfiedLinkError e) {
      String arch = System.getProperty("os.arch");
      String name = System.getProperty("os.name");

      if (name.startsWith("Windows") && e.getMessage().contains("Can't find dependent libraries")) {
        System.err.println("On Windows, you need some additional support libraries.\n" +
          "For example, you can install the two packages in cygwin:\n" +
          "\n" +
          "   mingw64-x86_64-gcc-core   mingw64-x86_64-gfortran\n" +
          "\n" +
          "and add the directory <cygwin-home>\\usr\\x86_64-w64-mingw32\\sys-root\\mingw\\bin to your path.\n\n" +
          "For more information, see http://github.com/mikiobraun/jblas/wiki/Missing-Libraries");
      } else if (name.equals("Linux") && arch.equals("amd64")) {
        System.err.println("On Linux 64bit, you need additional support libraries.\n" +
          "You need to install libgfortran3.\n\n" +
          "For example for debian or Ubuntu, type \"sudo apt-get install libgfortran3\"\n\n" +
          "For more information, see https://github.com/mikiobraun/jblas/wiki/Missing-Libraries");
      }
    } catch (UnsupportedArchitectureException e) {
      System.err.println(e.getMessage());
    }
  }

  public static void loadDependentLibraries() {
    String arch = System.getProperty("os.arch");
    String name = System.getProperty("os.name");

    LibraryLoader loader = new LibraryLoader();

    if (name.startsWith("Windows") && arch.equals("amd64")) {
      loader.loadLibrary("libgcc_s_sjlj-1", false, true);
      loader.loadLibrary("libgfortran-3", false, true);
    } else if (name.startsWith("Windows") && arch.equals("x86")) {
      loader.loadLibrary("libgcc_s_dw2-1", false, true);
      loader.loadLibrary("libgfortran-3", false, true);
    }
  }
}
