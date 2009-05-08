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

package org.jblas;

import junit.framework.TestCase;
import static org.jblas.TicToc.*;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;
import java.io.File;
import java.io.IOException;
import static org.jblas.MatrixFunctions.*;
import java.util.Arrays;

public class DemoImages extends TestCase {
	private DoubleMatrix tile(DoubleMatrix x, int ix, int iy, int wx, int wy) {
		int width = (x.rows - ix) / wx;
		int height = (x.columns - iy) / wy;
		DoubleMatrix result = new DoubleMatrix(width*height, wx*wy);
		int counter = 0;
		for (int i = 0; i < x.rows - width; i += wx)
			for (int j = 0; j < x.columns - height; j += wy) {
				copyRect2(result, counter++, x, wx, wy, i, j);
			}
		return result;
	}

	private void copyRect2(DoubleMatrix result, int counter, DoubleMatrix x, int wx, int wy, int i, int j) {
		for (int jj = 0; jj < wx; jj++)
			NativeBlas.dcopy(wy, x.data, x.index(i, j + jj), 1, result.data, result.index(counter, jj * wy), result.rows);
	}
	
	private void copyRect(DoubleMatrix result, int counter, DoubleMatrix x, int wx, int wy, int i, int j) {
		int[] I = range(i, i + wx - 1);
		int[] J = range(j, j + wy - 1);
		result.putRow(counter, x.get(I, J));
	}
	
	private int[] range(int a, int b) {
		int[] result = new int[b - a + 1];
		for (int i = 0; i <= b - a; i++)
			result[i] = a + i;
		return result;
	}

	private int[] range(int a, int s, int b) {
		int[] result = new int[(b - a + (s-1)) / s];
		for (int c = 0, i = a; c < result.length; i += s, c++)
			result[c] = i;
		return result;
	}
	
	private void print(int[] a) {
		if (a.length == 0)
			System.out.println("[]");
		else {
			System.out.print("[");
			for (int i = 0; i < a.length - 1; i++)
				System.out.printf("%d; ", a[i]);
			System.out.printf("%d]\n", a[a.length - 1]);
		}
	}
	
	public void testRange() {
		System.out.println("testRange-------");
		print(range(1,3));
		for (int i = 1; i < 10; i++) {
			System.out.printf("i = %d ", i);
			print(range(1,2,i));
		}
	}
	
	private DoubleMatrix[] loadImage(String name)
		throws IOException {
		BufferedImage image = ImageIO.read(new File(name));
		
		DoubleMatrix[] result = new DoubleMatrix[3];
		for (int i = 0; i < 3; i++) {
			result[i] = new DoubleMatrix(image.getWidth(), image.getHeight());
		}
		
		for (int x = 0; x < image.getWidth(); x++) {
			for (int y = 0; y < image.getHeight(); y++) {
				int pixel = image.getRGB(x, y);
				result[0].put(x, y, ((pixel & 0xff0000) >> 16) / 255.0 );
				result[1].put(x, y, ((pixel & 0x00ff00) >> 8) / 255.0 );
				result[2].put(x, y, ((pixel & 0x0000ff)) / 255.0 );
			}
		}
		return result;
	}
	
	private int doubleToByte(double v) {
		if (v > 1.0) v = 1.0;
		else if(v < 0.0) v = 0.0;
		return (int) (v * 255);
	}

	private void saveImage(String name, String formatName, DoubleMatrix image)
		throws IOException {
		int width = image.rows;
		int height = image.columns;
		BufferedImage out = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
		
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++) {
				int pixel = doubleToByte(image.get(x,y));
				pixel |= (pixel << 16) | (pixel << 8);
				out.setRGB(x, y, pixel);
			}
		
		ImageIO.write(out, formatName, new File(name));		
	}

	private void saveImage(String name, String formatName, DoubleMatrix[] image)
		throws IOException {
		int width = image[0].rows;
		int height = image[0].columns;
		BufferedImage out = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
		
		for (int x = 0; x < width; x++)
			for (int y = 0; y < height; y++) {
				int pixel = (doubleToByte(image[0].get(x,y)) << 16) |
							(doubleToByte(image[1].get(x,y)) << 8) |
							(doubleToByte(image[2].get(x,y)));
				out.setRGB(x, y, pixel);
			}
		
		ImageIO.write(out, formatName, new File(name));
	}
	
	public void testTile() {
		//DoubleMatrix x = DoubleMatrix.randn(1000,1000);
		DoubleMatrix x = new DoubleMatrix(2000,2000);
		for (int i = 1; i < x.length; i++)
			x.put(i, i);
		//x.print();
		
		tic("Tiling matrix");
		DoubleMatrix t = tile(x, 0, 0, 100, 100);
		toc();

		System.out.printf("And the result has %d times %d entries\n", t.rows, t.columns);
		//t.print();
		
		tic("Folding");
		DoubleMatrix out = t.mmul(DoubleMatrix.ones(t.columns));
		toc();
		
		//out.reshape(5, 5);
		
		//out.print();
		
		System.out.println("Done!");
	}
	
	public void testLoadWrite() {
		try {
			tic("Loading");
			DoubleMatrix[] img = loadImage("/home/mikio/pics/me.jpg");
			toc();
			tic("Saving");
			saveImage("/home/mikio/pics/saved_me.png", "png", img);
			toc();
		} catch(IOException e) {
			System.err.println("Error reading or writing file");
		}
	}

	public long ops;
	public long mult_ops;
	public long add_ops;
	public long copy_ops;
	
	private DoubleMatrix convolute(DoubleMatrix img, DoubleMatrix weights) {
		DoubleMatrix result = new DoubleMatrix(img.rows - weights.columns, img.columns - weights.rows);
		DoubleMatrix w = new DoubleMatrix(img.rows, weights.columns);
		
		ops = 0;
		
		for (int i = 0; i < img.columns - weights.rows; i++) {
			// compute all filter for all columns
			NativeBlas.dgemm('N', 'N', img.rows, weights.rows, weights.columns,
						1.0, img.data, img.index(0, i), img.rows,
						weights.data, 0, weights.rows,
						0.0, w.data, 0, w.rows);
			ops += 2*(long)img.rows * weights.rows * weights.columns;
			
			// collect results
			// add rows of length w.rows - w.columns from j, j to first row.
			for (int j = 1; j < w.columns; j++) {
				NativeBlas.daxpy(w.rows - w.columns, 1.0, w.data, img.index(j, j), 1, w.data, 0, 1);
				ops += w.rows - w.columns;
			}
			// copy the result back to the output vector.
			NativeBlas.dcopy(w.rows - w.columns, w.data, 0, 1, result.data, result.index(0, i), 1);
		}
		return result;
	}
	
	private DoubleMatrix convoluteNaive(DoubleMatrix img, DoubleMatrix weights) {
		int width = weights.rows;
		int height = weights.columns;
		DoubleMatrix result = new DoubleMatrix(img.rows - width, img.columns - height);
		for (int i = 0; i < img.rows - width; i++)
			for (int j = 0; j < img.columns - height; j++) {
				double sum = 0.0;
				for (int l = 0; l < height; l++) {
					for (int k = 0; k < width; k++) {
						sum += img.get(i + k, j + l) * weights.get(k, l);
					}
				}
				result.put(i, j, sum);
			}
		ops += 2 * (long)(img.rows - width) * (img.columns - height) * height * width;
		return result;
	}
	
	private double ringMask(double x, double y) {
		double d = sqrt(x*x + y*y);
		if (d < 0.0 || d >= 10.0)
			return 0.0;
		else
			return 1.0;
	}
	
	private double mexicanHatMask(double x, double y) {
		double d = x*x + y*y;
		double sigma = 2;
		return 1/(sqrt(2*Math.PI)*sigma*sigma*sigma) * (1 - d/(sigma*sigma)) *
			exp(- d / (2 * sigma*sigma));
	}
	
	private DoubleMatrix makeMask(int[] x, int[] y) {
		DoubleMatrix result = new DoubleMatrix(x.length, y.length);
		
		for (int i = 0; i < x.length; i++)
			for (int j = 0; j < y.length; j++)
				result.put(i, j, mexicanHatMask(x[i], y[j]));
		return result;
	}
	
	public void testConvolute() {
		try {
			DoubleMatrix[] img = loadImage("/home/mikio/pics/img.jpg");
			
			tic("Converting to grayscale");
			DoubleMatrix scaledRed = img[0].mul(0.4);
			DoubleMatrix scaledGreen = img[1].mul(0.4);
			DoubleMatrix scaledBlue = img[2].mul(0.2);
			DoubleMatrix gray = scaledRed.addi(scaledGreen).addi(scaledBlue);
			toc();
			
			DoubleMatrix weights = makeMask(range(-20, 20), range(-20, 20));
			//DoubleMatrix weights = DoubleMatrix.randn(51,51);
			tic("Convoluting");
			DoubleMatrix cgray = convolute(gray, weights);
			double time = toc();
			cgray.addi(0.5);
			cgray.divi(abs(cgray).max());
			
			System.out.printf("[INFO] weight size = %d\n", weights.length);
			System.out.printf("[INFO] total ops = %.3fM (%.3f MFLOPS)\n", ops / 1000000d, ops / 1000000d / time);
			
			saveImage("/home/mikio/pics/saved_img.jpg", "jpg", cgray);
		} catch(IOException e) {
			System.err.println("Error reading or writing file");
		}		
	}
	
	int countHowMany(int s, int I) {
		int counter = 0;
		for (int i = 0; i < I; i += s)
			counter++;
		return counter;
	}
	
	public void testHowMany() {
		/*for (int s = 1; s < 5; s++)
			for (int i = 0; i < 20; i++)
				System.out.printf("0..%d..%d => %d (vs. %d)\n", s, i, countHowMany(s, i), (i - 0 + (s-1)) / s);
		*/	
	}
}
