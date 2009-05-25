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

import java.io.*;

/**
 * Class which allows to load a dynamic file as resource (for example, from a 
 * jar-file)
 */
public class LibraryLoader {

    /** Find the library <tt>libname</tt> as a resource, copy it to a tempfile
     * and load it using System.load(). The name of the library has to be the
     * base name, it is mapped to the corresponding system name using
     * System.mapLibraryName(). For example, the library "foo" is called "libfoo.so"
     * under Linux and "foo.dll" under Windows, but you just have to pass "foo"
     * the loadLibrary().
     *
     * I'm not quite sure if this doesn't open all kinds of security holes. Any ideas?
     *
     * @param libname basename of the library
     * @throws UnsatisfiedLinkError if library cannot be found
     */
    public void loadLibrary(String libname) {
        libname = System.mapLibraryName(libname);

        // We're in a static initializer and need a class. What shall we do?
        Class cl = getClass();

        // Trying to copy from here.
        System.err.println("Trying to copy from /" + libname + ".");
        InputStream is = cl.getResourceAsStream("/" + libname);

        // Trying to copy from "bin"
        if (is == null) {
            System.err.println("Trying to copy from /bin/" + libname + ".");
            is = cl.getResourceAsStream("/bin/" + libname);
        }

        // Trying to extract static version from the jar file. Why the static version?
        // Because it is more likely to run.
        if (is == null) {
            System.err.println("Trying to copy from " + fatJarLibraryPath(libname, "static") + ".");
            is = cl.getResourceAsStream(fatJarLibraryPath(libname, "static"));
        }

        // Finally, let's see if we can get the dynamic version.
        if (is == null) {
            is = cl.getResourceAsStream(fatJarLibraryPath(libname, "dynamic"));
            System.err.println("Trying to copy from " + fatJarLibraryPath(libname, "dynamic") + ".");
        }

        // Oh man, have to get out of here!
        if (is == null) {
            throw new UnsatisfiedLinkError("Couldn't find the resource " + libname + ".");
        }

        try {
            File tempfile = File.createTempFile("jblas", libname);
            tempfile.deleteOnExit();
            OutputStream os = new FileOutputStream(tempfile);

            System.out.println("tempfile.getPath() = " + tempfile.getPath());

            long savedTime = System.currentTimeMillis();

            byte buf[] = new byte[1024];
            int len;
            while ((len = is.read(buf)) > 0) {
                os.write(buf, 0, len);
            }

            double seconds = (double) (System.currentTimeMillis() - savedTime) / 1e3;
            System.err.println("Copying took " + seconds + " seconds.");

            os.close();

            System.load(tempfile.getPath());
        } catch (IOException io) {
            System.err.println("Could not create the temp file: " + io.toString() + ".\n");
        } catch (UnsatisfiedLinkError ule) {
            System.err.println("Couldn't load copied link file: " + ule.toString() + ".\n");
        }
    }

    static public String unifyOSName(String osname) {
        if (osname.startsWith("Windows")) {
            return "Windows";
        }
        return osname;
    }

    /** Compute the path to the library. The path is basically
    "/" + os.name + "/" + os.arch + "/" + libname. */
    static public String fatJarLibraryPath(String libname, String linkage) {
        String sep = "/"; //System.getProperty("file.separator");
        String os_name = unifyOSName(System.getProperty("os.name"));
        String os_arch = System.getProperty("os.arch");
        return sep + "lib" + sep + linkage + sep + os_name + sep + os_arch + sep + libname;
    }
}
