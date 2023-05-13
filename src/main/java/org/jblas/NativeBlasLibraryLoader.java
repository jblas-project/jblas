// --- BEGIN LICENSE BLOCK ---
/*
 * Copyright (c) 2009, Mikio L. Braun
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of the Technische Universitaet Berlin nor the
 *       names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior
 *       written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// --- END LICENSE BLOCK ---

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
 * Date: Oct 24, 2012
 */
class NativeBlasLibraryLoader {
  static void loadLibraryAndCheckErrors() {
    try {
      try {
        // Try to load it first, probably it's in the path
        System.loadLibrary("jblas");
      } catch (UnsatisfiedLinkError e) {
        // Nope, ok, so let's copy it.
        Logger.getLogger().config(
            "BLAS native library not found in path. Copying native library "
                + "from the archive. Consider installing the library somewhere "
                + "in the path (for Windows: PATH, for Linux: LD_LIBRARY_PATH).");

        // potentially load dependent libraries (mostly Cygwin libs for Windows)
        loadDependentLibraries();

        // Ok, and now load it!
        new LibraryLoader().loadLibrary("jblas", true);
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
      loader.loadLibrary("libgcc_s_sjlj-1", false);
      loader.loadLibrary("libgfortran-3", false);
    } else if (name.startsWith("Windows") && arch.equals("x86")) {
      loader.loadLibrary("libgcc_s_dw2-1", false);
      loader.loadLibrary("libgfortran-3", false);
    } else if (name.equals("Linux") && arch.equals("amd64")) {
      loader.loadLibrary("quadmath-0", false);
      loader.loadLibrary("gfortran-5", false);
    }
  }
}
