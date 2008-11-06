package edu.ida.core;

public class Dynamic {
	public static native void hello();
	
	public static void main(String[] args) {
		System.out.println("trying to load library");
		System.loadLibrary("Dynamic");
		System.out.println("done");
		hello();
		System.out.println("done calling hello");
	}
}
