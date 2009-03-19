package org.jblas.la;

public class TicToc {
    private static long savedTime;

    public static void tic(String message) {
        System.out.print(message);
        System.out.print("...");
        System.out.flush();
        savedTime = System.currentTimeMillis();
    }

    public static void tic(String message, Object... args) {
        System.out.printf(message, args);
        System.out.print("...");
        System.out.flush();
        savedTime = System.currentTimeMillis();
    }

    public static double toc() {
        long elapsedTime = System.currentTimeMillis() - savedTime;
        System.out.printf(" (%.2fs)", (double)elapsedTime / 1000d);
        System.out.println();
        return (double)elapsedTime / 1000d;
    }

    public static void main(String args[]) {
        tic("counting to 10000000");
        int counter = 0;
        for (int i = 0; i < 10000000; i++)
                counter++;
        toc();
    }
}
