package <%= package %>;

import org.jblas.core.ComplexFloat;
import org.jblas.core.ComplexDouble;

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
		  new org.jblas.util.LibraryLoader().loadLibrary("j<%= classname.downcase %>");
	  }
  } 

<% for r in routines -%>
<%= generate_native_declaration r %>
<% end %>
}
