package org.jblas;

import org.jblas.util.Logger;

/**
 * <one line description>
 * <p/>
 * <longer description>
 * <p/>
 * User: mikio
 * Date: 10/24/12
 * Time: 3:15 PM
 */
public class NativeBlasLibraryLoader {
  static void loadLibraryAndCheckErrors() {
    try {
      try {
        System.loadLibrary("jblas");
      } catch (UnsatisfiedLinkError e) {
        Logger.getLogger().config(
            "BLAS native library not found in path. Copying native library "
                + "from the archive. Consider installing the library somewhere "
                + "in the path (for Windows: PATH, for Linux: LD_LIBRARY_PATH).");
        new org.jblas.util.LibraryLoader().loadLibrary("jblas", true);
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
          "You need to install libgfortran.\n\n" +
          "For example for debian or Ubuntu, type \"aptitude install libgfortran3\"\n\n" +
          "For more information, see https://github.com/mikiobraun/jblas/wiki/Missing-Libraries");
      }
    }
  }
}
