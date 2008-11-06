/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.ida.la;

/**
 *
 * @author mikio
 */
public class MemoryTracker {

    public static long allocatedBytes = 0;
    public static long allocatedCount = 0;
    public static long finalizedBytes = 0;
    public static long finalizedCount = 0;
    public static long maxUsedBytes = 0;
    public static long allocatedBytesIntervalCounter = 0;
    public static boolean showStatistics = false;
    public static long garbageCollectionCounter = 0;

    public static void printStatistics() {
        long usedBytes = allocatedBytes - finalizedBytes;
        System.err.printf("[INFO/MemoryTracker] Still Used Bytes = %.2fMB (max: %.2fMB), Allocated Bytes = %.2fMB, Allocated = %d, Finalized = %d, called GC = %d\n",
                usedBytes / 1048576.0, maxUsedBytes / 1048576.0, allocatedBytes / 1048576.0, allocatedCount, finalizedCount, garbageCollectionCounter);
    }

    public static void showStatistics(boolean flag) {
        if (flag == showStatistics) {
            return;
        }
        if (flag) {
            showStatistics = true;
            new Thread(new Runnable() {

                public void run() {
                    System.err.println("[INFO/MemoryTracker] Started thread!");
                    while (showStatistics) {
                        System.gc();
                        //System.runFinalization();
                        if (allocatedBytes - finalizedBytes >= 1048576) {
                            printStatistics();
                        }
                        try {
                            Thread.sleep(1000);
                        } catch (Exception e) {
                            // whatever!
                            }
                    }
                    System.err.println("[INFO/MemoryTracker] Exited thread!");
                }
            }).start();
        } else {
            showStatistics = false;
        }
    }

    public static void registerFinalize(int bytes) {
        finalizedBytes += bytes;
        finalizedCount++;
    }

    public static void registerCreate(int bytes) {
        allocatedBytes += bytes;
        allocatedCount++;
        long usedBytes = allocatedBytes - finalizedBytes;
        if (usedBytes > maxUsedBytes) {
            maxUsedBytes = usedBytes;
        }
        allocatedBytesIntervalCounter += bytes;
        if (allocatedBytesIntervalCounter > 500 * 1048576) {
            System.gc();
            allocatedBytesIntervalCounter = 0;
            garbageCollectionCounter++;
        }
    }

    public static long getGarbageCollectionCounter() {
        return garbageCollectionCounter;
    }
}
