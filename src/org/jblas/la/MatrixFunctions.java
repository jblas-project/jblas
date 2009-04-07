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

package org.jblas.la;
 
import java.lang.Math;

/**
 * This class provides the functions from java.lang.Math for matrices. The
 * functions are applied to each element of the matrix.
 * 
 * @author Mikio Braun
 */
public class MatrixFunctions {

	/*#
	def mapfct(f); <<-EOS
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) #{f}(x.get(i)));
	   return x;
	   EOS
  	end
  	
  	def cmapfct(f); <<-EOS
	   for (int i = 0; i < x.length; i++)
	      x.put(i, x.get(i).#{f}());
	   return x;
	   EOS
  	end
	#*/

	/**
	 * Sets all elements in this matrix to their absolute values. Note
	 * that this operation is in-place.
	 * @see MatrixFunctions#abs(DoubleMatrix)
	 * @return this matrix
	 */
	public static DoubleMatrix absi(DoubleMatrix x) { 
		/*# mapfct('Math.abs') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.abs(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	public static ComplexDoubleMatrix absi(ComplexDoubleMatrix x) {
		/*# cmapfct('abs') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, x.get(i).abs());
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the trigonometric <i>arccosine</i> function element wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#acos(DoubleMatrix)
	 * @return this matrix
	 */
	public static DoubleMatrix acosi(DoubleMatrix x) { 
		/*# mapfct('Math.acos') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.acos(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the trigonometric <i>arcsine</i> function element wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#asin(DoubleMatrix)
	 * @return this matrix
	 */	
	public static DoubleMatrix asini(DoubleMatrix x) { 
		/*# mapfct('Math.asin') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.asin(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the trigonometric <i>arctangend</i> function element wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#atan(DoubleMatrix)
	 * @return this matrix
	 */		
	public static DoubleMatrix atani(DoubleMatrix x) { 
		/*# mapfct('Math.atan') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.atan(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>cube root</i> function element wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#cbrt(DoubleMatrix)
	 * @return this matrix
	 */		
	public static DoubleMatrix cbrti(DoubleMatrix x) { 
		/*# mapfct('Math.cbrt') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.cbrt(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Element-wise round up by applying the <i>ceil</i> function on each 
	 * element. Note that this is an in-place operation.
	 * @see MatrixFunctions#ceil(DoubleMatrix)
	 * @return this matrix
	 */		
	public static DoubleMatrix ceili(DoubleMatrix x) { 
		/*# mapfct('Math.ceil') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.ceil(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>cosine</i> function element-wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#cos(DoubleMatrix)
	 * @return this matrix
	 */
	public static DoubleMatrix cosi(DoubleMatrix x) { 
		/*# mapfct('Math.cos') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.cos(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>hyperbolic cosine</i> function element-wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#cosh(DoubleMatrix)
	 * @return this matrix
	 */	
	public static DoubleMatrix coshi(DoubleMatrix x) { 
		/*# mapfct('Math.cosh') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.cosh(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>exponential</i> function element-wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#exp(DoubleMatrix)
	 * @return this matrix
	 */		
	public static DoubleMatrix expi(DoubleMatrix x) { 
		/*# mapfct('Math.exp') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.exp(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Element-wise round down by applying the <i>floor</i> function on each 
	 * element. Note that this is an in-place operation.
	 * @see MatrixFunctions#floor(DoubleMatrix)
	 * @return this matrix
	 */		
	public static DoubleMatrix floori(DoubleMatrix x) { 
		/*# mapfct('Math.floor') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.floor(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>natural logarithm</i> function element-wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#log(DoubleMatrix)
	 * @return this matrix
	 */		
	public static DoubleMatrix logi(DoubleMatrix x) {
		/*# mapfct('Math.log') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.log(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>logarithm with basis to 10</i> element-wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#log10(DoubleMatrix)
	 * @return this matrix
	 */
	public static DoubleMatrix log10i(DoubleMatrix x) {
		/*# mapfct('Math.log10') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.log10(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Element-wise power function. Replaces each element with its
	 * power of <tt>d</tt>.Note that this is an in-place operation.
	 * @param d the exponent
	 * @see MatrixFunctions#pow(DoubleMatrix,double)
	 * @return this matrix
	 */	
	public static DoubleMatrix powi(DoubleMatrix x, double d) {
		if (d == 2.0)
			return x.muli(x);
		else {
			for (int i = 0; i < x.length; i++)
				x.put(i, (double) Math.pow(x.get(i), d));
			return x;
		}
	}

    public static DoubleMatrix powi(double base, DoubleMatrix x) {
        for (int i = 0; i < x.length; i++)
            x.put(i, (double) Math.pow(base, x.get(i)));
        return x;
    }

    public static DoubleMatrix powi(DoubleMatrix x, DoubleMatrix e) {
        x.checkLength(e.length);
        for (int i = 0; i < x.length; i++)
            x.put(i, (double) Math.pow(x.get(i), e.get(i)));
        return x;
    }

    public static DoubleMatrix signumi(DoubleMatrix x) {
		/*# mapfct('Math.signum') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.signum(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	public static DoubleMatrix sini(DoubleMatrix x) { 
		/*# mapfct('Math.sin') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.sin(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}

	public static DoubleMatrix sinhi(DoubleMatrix x) { 
		/*# mapfct('Math.sinh') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.sinh(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	public static DoubleMatrix sqrti(DoubleMatrix x) { 
		/*# mapfct('Math.sqrt') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.sqrt(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	public static DoubleMatrix tani(DoubleMatrix x) {
		/*# mapfct('Math.tan') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.tan(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	public static DoubleMatrix tanhi(DoubleMatrix x) {
		/*# mapfct('Math.tanh') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (double) Math.tanh(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}

	/**
	 * Returns a copy of this matrix where all elements are set to their
	 * absolute values. 
	 * @see MatrixFunctions#absi(DoubleMatrix)
	 * @return copy of this matrix
	 */
	public static DoubleMatrix abs(DoubleMatrix x) { return absi(x.dup()); }
	
	/**
	 * Returns a copy of this matrix where the trigonometric <i>acos</i> function is applied
	 * element wise.
	 * @see MatrixFunctions#acosi(DoubleMatrix)
	 * @return copy of this matrix
	 */
	public static DoubleMatrix acos(DoubleMatrix x)   { return acosi(x.dup()); }
	public static DoubleMatrix asin(DoubleMatrix x)   { return asini(x.dup()); }
	public static DoubleMatrix atan(DoubleMatrix x)   { return atani(x.dup()); }
	public static DoubleMatrix cbrt(DoubleMatrix x)   { return cbrti(x.dup()); }
    public static DoubleMatrix ceil(DoubleMatrix x)   { return ceili(x.dup()); }
    public static DoubleMatrix cos(DoubleMatrix x)    { return cosi(x.dup()); }
    public static DoubleMatrix cosh(DoubleMatrix x)   { return coshi(x.dup()); }
    public static DoubleMatrix exp(DoubleMatrix x)    { return expi(x.dup()); }
    public static DoubleMatrix floor(DoubleMatrix x)  { return floori(x.dup()); }
    public static DoubleMatrix log(DoubleMatrix x)    { return logi(x.dup()); }
    public static DoubleMatrix log10(DoubleMatrix x)  { return log10i(x.dup()); }
    public static double pow(double x, double y) { return (double)Math.pow(x, y); }
    public static DoubleMatrix pow(DoubleMatrix x, double e) { return powi(x.dup(), e); }
    public static DoubleMatrix pow(double b, DoubleMatrix x) { return powi(b, x.dup()); }
    public static DoubleMatrix pow(DoubleMatrix x, DoubleMatrix e) { return powi(x.dup(), e); }
    public static DoubleMatrix signum(DoubleMatrix x) { return signumi(x.dup()); }
    public static DoubleMatrix sin(DoubleMatrix x)    { return sini(x.dup()); }
    public static DoubleMatrix sinh(DoubleMatrix x)   { return sinhi(x.dup()); }
    public static DoubleMatrix sqrt(DoubleMatrix x)   { return sqrti(x.dup()); }
    public static DoubleMatrix tan(DoubleMatrix x)    { return tani(x.dup()); }
    public static DoubleMatrix tanh(DoubleMatrix x)   { return tanhi(x.dup()); }

    /*# %w{abs acos asin atan cbrt ceil cos cosh exp floor log log10 signum sin sinh sqrt tan tanh}.map do |fct| <<-EOS
    public static double #{fct}(double x) { return (double)Math.#{fct}(x); }
    EOS
        end   
     #*/
//RJPP-BEGIN------------------------------------------------------------
    public static double abs(double x) { return (double)Math.abs(x); }
    public static double acos(double x) { return (double)Math.acos(x); }
    public static double asin(double x) { return (double)Math.asin(x); }
    public static double atan(double x) { return (double)Math.atan(x); }
    public static double cbrt(double x) { return (double)Math.cbrt(x); }
    public static double ceil(double x) { return (double)Math.ceil(x); }
    public static double cos(double x) { return (double)Math.cos(x); }
    public static double cosh(double x) { return (double)Math.cosh(x); }
    public static double exp(double x) { return (double)Math.exp(x); }
    public static double floor(double x) { return (double)Math.floor(x); }
    public static double log(double x) { return (double)Math.log(x); }
    public static double log10(double x) { return (double)Math.log10(x); }
    public static double signum(double x) { return (double)Math.signum(x); }
    public static double sin(double x) { return (double)Math.sin(x); }
    public static double sinh(double x) { return (double)Math.sinh(x); }
    public static double sqrt(double x) { return (double)Math.sqrt(x); }
    public static double tan(double x) { return (double)Math.tan(x); }
    public static double tanh(double x) { return (double)Math.tanh(x); }
//RJPP-END--------------------------------------------------------------

//STOP
    public static DoubleMatrix floatToDouble(FloatMatrix fm) {
    	DoubleMatrix dm = new DoubleMatrix(fm.rows, fm.columns);

        for (int i = 0; i < fm.length; i++)
            dm.put(i, (double) fm.get(i));

        return dm;
    }

    public static FloatMatrix doubleToFloat(DoubleMatrix dm) {
        FloatMatrix fm = new FloatMatrix(dm.rows, dm.columns);

        for (int i = 0; i < dm.length; i++)
            fm.put(i, (float) dm.get(i));

        return fm;
    }
//START
    
//BEGIN
  // The code below has been automatically generated.
  // DO NOT EDIT!

	/*#
	def mapfct(f); <<-EOS
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) #{f}(x.get(i)));
	   return x;
	   EOS
  	end
  	
  	def cmapfct(f); <<-EOS
	   for (int i = 0; i < x.length; i++)
	      x.put(i, x.get(i).#{f}());
	   return x;
	   EOS
  	end
	#*/

	/**
	 * Sets all elements in this matrix to their absolute values. Note
	 * that this operation is in-place.
	 * @see MatrixFunctions#abs(FloatMatrix)
	 * @return this matrix
	 */
	public static FloatMatrix absi(FloatMatrix x) { 
		/*# mapfct('Math.abs') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.abs(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	public static ComplexFloatMatrix absi(ComplexFloatMatrix x) {
		/*# cmapfct('abs') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, x.get(i).abs());
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the trigonometric <i>arccosine</i> function element wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#acos(FloatMatrix)
	 * @return this matrix
	 */
	public static FloatMatrix acosi(FloatMatrix x) { 
		/*# mapfct('Math.acos') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.acos(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the trigonometric <i>arcsine</i> function element wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#asin(FloatMatrix)
	 * @return this matrix
	 */	
	public static FloatMatrix asini(FloatMatrix x) { 
		/*# mapfct('Math.asin') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.asin(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the trigonometric <i>arctangend</i> function element wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#atan(FloatMatrix)
	 * @return this matrix
	 */		
	public static FloatMatrix atani(FloatMatrix x) { 
		/*# mapfct('Math.atan') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.atan(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>cube root</i> function element wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#cbrt(FloatMatrix)
	 * @return this matrix
	 */		
	public static FloatMatrix cbrti(FloatMatrix x) { 
		/*# mapfct('Math.cbrt') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.cbrt(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Element-wise round up by applying the <i>ceil</i> function on each 
	 * element. Note that this is an in-place operation.
	 * @see MatrixFunctions#ceil(FloatMatrix)
	 * @return this matrix
	 */		
	public static FloatMatrix ceili(FloatMatrix x) { 
		/*# mapfct('Math.ceil') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.ceil(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>cosine</i> function element-wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#cos(FloatMatrix)
	 * @return this matrix
	 */
	public static FloatMatrix cosi(FloatMatrix x) { 
		/*# mapfct('Math.cos') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.cos(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>hyperbolic cosine</i> function element-wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#cosh(FloatMatrix)
	 * @return this matrix
	 */	
	public static FloatMatrix coshi(FloatMatrix x) { 
		/*# mapfct('Math.cosh') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.cosh(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>exponential</i> function element-wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#exp(FloatMatrix)
	 * @return this matrix
	 */		
	public static FloatMatrix expi(FloatMatrix x) { 
		/*# mapfct('Math.exp') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.exp(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Element-wise round down by applying the <i>floor</i> function on each 
	 * element. Note that this is an in-place operation.
	 * @see MatrixFunctions#floor(FloatMatrix)
	 * @return this matrix
	 */		
	public static FloatMatrix floori(FloatMatrix x) { 
		/*# mapfct('Math.floor') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.floor(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>natural logarithm</i> function element-wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#log(FloatMatrix)
	 * @return this matrix
	 */		
	public static FloatMatrix logi(FloatMatrix x) {
		/*# mapfct('Math.log') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.log(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Applies the <i>logarithm with basis to 10</i> element-wise on this
	 * matrix. Note that this is an in-place operation.
	 * @see MatrixFunctions#log10(FloatMatrix)
	 * @return this matrix
	 */
	public static FloatMatrix log10i(FloatMatrix x) {
		/*# mapfct('Math.log10') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.log10(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	/**
	 * Element-wise power function. Replaces each element with its
	 * power of <tt>d</tt>.Note that this is an in-place operation.
	 * @param d the exponent
	 * @see MatrixFunctions#pow(FloatMatrix,float)
	 * @return this matrix
	 */	
	public static FloatMatrix powi(FloatMatrix x, float d) {
		if (d == 2.0f)
			return x.muli(x);
		else {
			for (int i = 0; i < x.length; i++)
				x.put(i, (float) Math.pow(x.get(i), d));
			return x;
		}
	}

    public static FloatMatrix powi(float base, FloatMatrix x) {
        for (int i = 0; i < x.length; i++)
            x.put(i, (float) Math.pow(base, x.get(i)));
        return x;
    }

    public static FloatMatrix powi(FloatMatrix x, FloatMatrix e) {
        x.checkLength(e.length);
        for (int i = 0; i < x.length; i++)
            x.put(i, (float) Math.pow(x.get(i), e.get(i)));
        return x;
    }

    public static FloatMatrix signumi(FloatMatrix x) {
		/*# mapfct('Math.signum') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.signum(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	
	public static FloatMatrix sini(FloatMatrix x) { 
		/*# mapfct('Math.sin') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.sin(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}

	public static FloatMatrix sinhi(FloatMatrix x) { 
		/*# mapfct('Math.sinh') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.sinh(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	public static FloatMatrix sqrti(FloatMatrix x) { 
		/*# mapfct('Math.sqrt') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.sqrt(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	public static FloatMatrix tani(FloatMatrix x) {
		/*# mapfct('Math.tan') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.tan(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}
	public static FloatMatrix tanhi(FloatMatrix x) {
		/*# mapfct('Math.tanh') #*/
//RJPP-BEGIN------------------------------------------------------------
	   for (int i = 0; i < x.length; i++)
	      x.put(i, (float) Math.tanh(x.get(i)));
	   return x;
//RJPP-END--------------------------------------------------------------
	}

	/**
	 * Returns a copy of this matrix where all elements are set to their
	 * absolute values. 
	 * @see MatrixFunctions#absi(FloatMatrix)
	 * @return copy of this matrix
	 */
	public static FloatMatrix abs(FloatMatrix x) { return absi(x.dup()); }
	
	/**
	 * Returns a copy of this matrix where the trigonometric <i>acos</i> function is applied
	 * element wise.
	 * @see MatrixFunctions#acosi(FloatMatrix)
	 * @return copy of this matrix
	 */
	public static FloatMatrix acos(FloatMatrix x)   { return acosi(x.dup()); }
	public static FloatMatrix asin(FloatMatrix x)   { return asini(x.dup()); }
	public static FloatMatrix atan(FloatMatrix x)   { return atani(x.dup()); }
	public static FloatMatrix cbrt(FloatMatrix x)   { return cbrti(x.dup()); }
    public static FloatMatrix ceil(FloatMatrix x)   { return ceili(x.dup()); }
    public static FloatMatrix cos(FloatMatrix x)    { return cosi(x.dup()); }
    public static FloatMatrix cosh(FloatMatrix x)   { return coshi(x.dup()); }
    public static FloatMatrix exp(FloatMatrix x)    { return expi(x.dup()); }
    public static FloatMatrix floor(FloatMatrix x)  { return floori(x.dup()); }
    public static FloatMatrix log(FloatMatrix x)    { return logi(x.dup()); }
    public static FloatMatrix log10(FloatMatrix x)  { return log10i(x.dup()); }
    public static float pow(float x, float y) { return (float)Math.pow(x, y); }
    public static FloatMatrix pow(FloatMatrix x, float e) { return powi(x.dup(), e); }
    public static FloatMatrix pow(float b, FloatMatrix x) { return powi(b, x.dup()); }
    public static FloatMatrix pow(FloatMatrix x, FloatMatrix e) { return powi(x.dup(), e); }
    public static FloatMatrix signum(FloatMatrix x) { return signumi(x.dup()); }
    public static FloatMatrix sin(FloatMatrix x)    { return sini(x.dup()); }
    public static FloatMatrix sinh(FloatMatrix x)   { return sinhi(x.dup()); }
    public static FloatMatrix sqrt(FloatMatrix x)   { return sqrti(x.dup()); }
    public static FloatMatrix tan(FloatMatrix x)    { return tani(x.dup()); }
    public static FloatMatrix tanh(FloatMatrix x)   { return tanhi(x.dup()); }

    /*# %w{abs acos asin atan cbrt ceil cos cosh exp floor log log10 signum sin sinh sqrt tan tanh}.map do |fct| <<-EOS
    public static float #{fct}(float x) { return (float)Math.#{fct}(x); }
    EOS
        end   
     #*/
//RJPP-BEGIN------------------------------------------------------------
    public static float abs(float x) { return (float)Math.abs(x); }
    public static float acos(float x) { return (float)Math.acos(x); }
    public static float asin(float x) { return (float)Math.asin(x); }
    public static float atan(float x) { return (float)Math.atan(x); }
    public static float cbrt(float x) { return (float)Math.cbrt(x); }
    public static float ceil(float x) { return (float)Math.ceil(x); }
    public static float cos(float x) { return (float)Math.cos(x); }
    public static float cosh(float x) { return (float)Math.cosh(x); }
    public static float exp(float x) { return (float)Math.exp(x); }
    public static float floor(float x) { return (float)Math.floor(x); }
    public static float log(float x) { return (float)Math.log(x); }
    public static float log10(float x) { return (float)Math.log10(x); }
    public static float signum(float x) { return (float)Math.signum(x); }
    public static float sin(float x) { return (float)Math.sin(x); }
    public static float sinh(float x) { return (float)Math.sinh(x); }
    public static float sqrt(float x) { return (float)Math.sqrt(x); }
    public static float tan(float x) { return (float)Math.tan(x); }
    public static float tanh(float x) { return (float)Math.tanh(x); }
//RJPP-END--------------------------------------------------------------

    
//END
}
