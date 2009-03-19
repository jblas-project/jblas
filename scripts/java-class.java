package <%= package %>;

import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import java.nio.ByteBuffer;
import java.nio.ShortBuffer;
import java.nio.IntBuffer;
import java.nio.LongBuffer;
import edu.ida.core.ComplexFloat;
import edu.ida.core.ComplexDouble;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.File;
import java.io.IOException;
import java.io.FileOutputStream;
import java.lang.UnsatisfiedLinkError;

public class <%= classname %> {

  static {
	  try {
		  System.loadLibrary("j<%= classname.downcase %>");
	  }
	  catch(UnsatisfiedLinkError e) {
		  System.err.println(
				  "BLAS native library not found in path. Copying native library\n" +
				  "from the archive. Consider installing the library somewhere\n" +
				  "in the path (for Windows: PATH, for Linux: LD_LIBRARY_PATH).");
		  new edu.ida.core.LibraryLoader().loadLibrary("j<%= classname.downcase %>");
	  }
  } 

<% for r in routines -%>
<%= generate_native_declaration r %>
<% end %>
}
