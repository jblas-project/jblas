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
 *     * Neither the name of the Technische Universität Berlin nor the
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
            new org.jblas.util.LibraryLoader().loadLibrary("jblas_arch_flavor", false, false);
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
        String name = System.getProperty("os.name");


        if (name.startsWith("Windows") && arch.equals("amd64")) {
            return null;
        }
        else if(arch.equals("i386") || arch.equals("x86") || arch.equals("x86_64") || arch.equals("amd64")) {
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
