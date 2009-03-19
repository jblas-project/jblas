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
	 */ 
	public void loadLibrary(String libname) {
		libname = System.mapLibraryName(libname);

		// We're in a static initializer and need a class. What shall we do?
		Class cl = getClass();

		InputStream is = cl.getResourceAsStream("/" + libname);

        // Hm, let's see if we can find it in the build directory 
		if (is == null)
			is = cl.getResourceAsStream("/bin/" + libname);

		if (is == null) {
			System.err.println("Couldn't find the resource " + libname + ".");
			System.exit(0);
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
			System.out.println("Copying took " + seconds + " seconds.");

			os.close();

			System.load(tempfile.getPath());
		} catch (IOException io) {
			System.err.println("Could not create the temp file: " + io.toString() + ".\n");
		} catch (UnsatisfiedLinkError ule) {
			System.err.println("Couldn't load copied link file: " + ule.toString() + ".\n");
		}
	}

    /** Compute the path to the library. The path is basically
        "/" + os.name + "/" + os.arch + "/" + libname. */
    static public String libraryPath(String libname) {
        return "/" + System.getProperty("os.name") + "/" + System.getProperty("os.arch") + "/" + libname;
    }

    static public void main(String[] args) {
        System.out.println(libraryPath(""));
    }
}
