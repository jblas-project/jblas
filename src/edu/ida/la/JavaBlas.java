/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.ida.la;

/**
 *
 * @author mikio
 */
public class JavaBlas {

    public static void rswap(int n, double[] dx, int dxIdx, int incx, double[] dy, int dyIdx, int incy) {
        if (incx == 1 && incy == 1 && dxIdx == 0 && dyIdx == 0) {
            double z;
            for (int i = 0; i < n; i++) {
                z = dx[i];
                dx[i] = dy[i];
                dy[i] = z;
            }
        } else {
            double z;
            for (int c = 0, xi = dxIdx, yi = dyIdx; c < n; xi += incx, yi += incy) {
                z = dx[xi];
                dx[xi] = dy[yi];
                dy[yi] = z;
            }
        }
    }

    public static void rcopy(int n, double[] dx, int dxIdx, int incx, double[] dy, int dyIdx, int incy) {
        if (incx == 1 && incy == 1) {
            System.arraycopy(dx, dxIdx, dy, dyIdx, n);
        } else {
            for (int c = 0, xi = dxIdx, yi = dyIdx; c < n; xi += incx, yi += incy) {
                dy[yi] = dx[xi];
            }
        }
    }

    /** Compute y <- alpha * x + y. */
    public static void raxpy(int n, double da, double[] dx, int dxIdx, int incx, double[] dy, int dyIdx, int incy) {
        if (dxIdx < 0 || dxIdx + (n - 1) * incx >= dx.length) {
            throw new LapackException("Java.raxpy", "Parameters for x aren't valid! (n = " + n + ", dx.length = " + dx.length + ", dxIdx = " + dxIdx + ", incx = " + incx + ")");
        }
        if (dyIdx < 0 || dyIdx + (n - 1) * incy >= dy.length) {
            throw new LapackException("Java.raxpy", "Parameters for y aren't valid! (n = " + n + ", dy.length = " + dy.length + ", dyIdx = " + dyIdx + ", incy = " + incy + ")");
        }
        
        if (incx == 1 && incy == 1 && dxIdx == 0 && dyIdx == 0) {
            if (da == 1.0) {
                for (int i = 0; i < n; i++) {
                    dy[i] += dx[i];
                }
            } else {
                for (int i = 0; i < n; i++) {
                    dy[i] += da * dx[i];
                }
            }
        } else {
            if (da == 1.0) {
                for (int c = 0, xi = dxIdx, yi = dyIdx; c < n; c++, xi += incx, yi += incy) {
                    dy[yi] += dx[xi];
                }

            } else {
                for (int c = 0, xi = dxIdx, yi = dyIdx; c < n; c++, xi += incx, yi += incy) {
                    dy[yi] += da * dx[xi];
                }
            }
        }
    }
    
    public static double rdot(int n, double[] dx, int dxIdx, int incx, double[] dy, int dyIdx, int incy) {
        double s = 0.0;
        if (incx == 1 && incy == 1 && dxIdx == 0 && dyIdx == 0) {
            for (int i = 0; i < n; i++)
                s += dx[i] * dy[i];
        }
        else {
            for (int c = 0, xi = dxIdx, yi = dyIdx; c < n; c++, xi += incx, yi += incy) {
                s += dx[xi] * dy[yi];
            }
        }
        return s;
    }
//BEGIN
  // The code below has been automatically generated.
  // DO NOT EDIT!

    public static void rswap(int n, float[] dx, int dxIdx, int incx, float[] dy, int dyIdx, int incy) {
        if (incx == 1 && incy == 1 && dxIdx == 0 && dyIdx == 0) {
            float z;
            for (int i = 0; i < n; i++) {
                z = dx[i];
                dx[i] = dy[i];
                dy[i] = z;
            }
        } else {
            float z;
            for (int c = 0, xi = dxIdx, yi = dyIdx; c < n; xi += incx, yi += incy) {
                z = dx[xi];
                dx[xi] = dy[yi];
                dy[yi] = z;
            }
        }
    }

    public static void rcopy(int n, float[] dx, int dxIdx, int incx, float[] dy, int dyIdx, int incy) {
        if (incx == 1 && incy == 1) {
            System.arraycopy(dx, dxIdx, dy, dyIdx, n);
        } else {
            for (int c = 0, xi = dxIdx, yi = dyIdx; c < n; xi += incx, yi += incy) {
                dy[yi] = dx[xi];
            }
        }
    }

    /** Compute y <- alpha * x + y. */
    public static void raxpy(int n, float da, float[] dx, int dxIdx, int incx, float[] dy, int dyIdx, int incy) {
        if (dxIdx < 0 || dxIdx + (n - 1) * incx >= dx.length) {
            throw new LapackException("Java.raxpy", "Parameters for x aren't valid! (n = " + n + ", dx.length = " + dx.length + ", dxIdx = " + dxIdx + ", incx = " + incx + ")");
        }
        if (dyIdx < 0 || dyIdx + (n - 1) * incy >= dy.length) {
            throw new LapackException("Java.raxpy", "Parameters for y aren't valid! (n = " + n + ", dy.length = " + dy.length + ", dyIdx = " + dyIdx + ", incy = " + incy + ")");
        }
        
        if (incx == 1 && incy == 1 && dxIdx == 0 && dyIdx == 0) {
            if (da == 1.0f) {
                for (int i = 0; i < n; i++) {
                    dy[i] += dx[i];
                }
            } else {
                for (int i = 0; i < n; i++) {
                    dy[i] += da * dx[i];
                }
            }
        } else {
            if (da == 1.0f) {
                for (int c = 0, xi = dxIdx, yi = dyIdx; c < n; c++, xi += incx, yi += incy) {
                    dy[yi] += dx[xi];
                }

            } else {
                for (int c = 0, xi = dxIdx, yi = dyIdx; c < n; c++, xi += incx, yi += incy) {
                    dy[yi] += da * dx[xi];
                }
            }
        }
    }
    
    public static float rdot(int n, float[] dx, int dxIdx, int incx, float[] dy, int dyIdx, int incy) {
        float s = 0.0f;
        if (incx == 1 && incy == 1 && dxIdx == 0 && dyIdx == 0) {
            for (int i = 0; i < n; i++)
                s += dx[i] * dy[i];
        }
        else {
            for (int c = 0, xi = dxIdx, yi = dyIdx; c < n; c++, xi += incx, yi += incy) {
                s += dx[xi] * dy[yi];
            }
        }
        return s;
    }
//END
}
