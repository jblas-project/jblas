/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas.util;

/**
 *
 */
public class ArchFlavor {

    static {
        try {
            System.loadLibrary("jblas_arch_flavor");
        } catch (UnsatisfiedLinkError e) {
            Logger.getLogger().config("ArchFlavor native library not found in path. Copying native library "
                    + "libjblas_arch_flavor from the archive. Consider installing the library somewhere "
                    + "in the path (for Windows: PATH, for Linux: LD_LIBRARY_PATH).");
            new org.jblas.util.LibraryLoader().loadLibrary("jblas_arch_flavor", false);
        }
    }
    private static String fixedFlavor = null;

    public static final int SSE = 1;
    public static final int SSE2 = 2;
    public static final int SSE3 = 3;
    public static final int NO_SSE = -1; // for platforms where it isn't applicable.

    public static native int SSELevel();

    public static String archFlavor() {
        if (fixedFlavor != null)
            return fixedFlavor;

        String arch = System.getProperty("os.arch");

        if (arch.equals("i386") || arch.equals("x86") || arch.equals("x86_64") || arch.equals("amd64")) {
            switch (SSELevel()) {
                case SSE:
                    return "sse";
                case SSE2:
                    return "sse2";
                case SSE3:
                    return "sse3";
                default:
                    return null;
            }
        } else {
            return null;
        }
    }

    public static void overrideArchFlavor(String flavor) {
        fixedFlavor = flavor;
    }
}
