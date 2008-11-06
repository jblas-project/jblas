package edu.ida.core;

import java.nio.ByteOrder;
import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

/**
 * This class provides some utility functions, for example for generating direct
 * double or float buffers.
 */
public class BlasUtil {
	/** Create direct DoubleBuffer holding a given number of double values. */
	public static DoubleBuffer createDoubleBuffer(int size) {
		return ByteBuffer.allocateDirect(Double.SIZE / Byte.SIZE * size).order(
				ByteOrder.nativeOrder()).asDoubleBuffer();
	}

	/** Create direct FloatBuffer holding a given number of float values. */
	public static FloatBuffer createFloatBuffer(int size) {
		return ByteBuffer.allocateDirect(Float.SIZE / Byte.SIZE * size).order(
				ByteOrder.nativeOrder()).asFloatBuffer();
	}

	/** Create direct IntBuffer holding a given number of intf values. */
	public static IntBuffer createIntBuffer(int size) {
		return ByteBuffer.allocateDirect(Integer.SIZE / Byte.SIZE * size)
				.order(ByteOrder.nativeOrder()).asIntBuffer();
	}
	
	public static int[] convertToArray(IntBuffer ibuf) {
		int[] out = new int[ibuf.capacity()];
		
		for (int i = 0; i < ibuf.capacity(); i++)
			out[i] = ibuf.get(i);
		
		return out;
	}

	public static void printBufferLn(java.io.PrintStream out, IntBuffer ibuf) {
		out.printf("IntBuffer[");
		for (int i = 0; i < ibuf.capacity(); i++) {
			if (i == 0)
				out.printf("%d", ibuf.get(i));
			else
				out.printf(" %d", ibuf.get(i));
		}
		out.printf("]\n");
	}
	
	/** Create direct DoubleBuffer from a list of initil values. */
	public static DoubleBuffer createDoubleBufferFrom(double... array) {
		DoubleBuffer b = createDoubleBuffer(array.length);
		b.put(array);
		return b;
	}

	/** Create direct FloatBuffer from a list of initil values. */
	public static FloatBuffer createFloatBufferFrom(float... array) {
		FloatBuffer b = createFloatBuffer(array.length);
		b.put(array);
		return b;
	}
	
	public static char stringToChar(String s) {
		return s.charAt(0);
	}
}
