/// --- BEGIN LICENSE BLOCK ---
// Copyright (c) 2009, Mikio L. Braun
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of the Technische Universität Berlin nor the
//       names of its contributors may be used to endorse or promote
//       products derived from this software without specific prior
//       written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/// --- END LICENSE BLOCK ---

#include "<%= header_file_name %>.h"

#define CORE_PACKAGE "org/jblas/"

/*
 * For convenience, we define some typedefs here which have the
 * same name as certain Java types, but their implementation differs (of course)
 */
typedef char *String;

typedef char ByteBuffer;
typedef short ShortBuffer;
typedef int IntBuffer;
typedef long LongBuffer;
typedef float FloatBuffer;
typedef double DoubleBuffer;

/* a function to create new objects */
static jobject createObject(JNIEnv *env, const char *className, const char *signature, ...)
{
  va_list args;
  jclass klass = (*env)->FindClass(env, className);
  jmethodID init = (*env)->GetMethodID(env, klass, "<init>", signature);
  jobject newObject;

  va_start(args, signature);
  newObject = (*env)->NewObjectV(env, klass, init, args);
  va_end(args);

  return newObject;
}

<% if $complexcc == 'f2c' %>
typedef struct { float real, imag; } ComplexFloat;
typedef struct { double real, imag; } ComplexDouble;

static jobject createComplexFloat(JNIEnv *env, ComplexFloat *fc)
{
  return createObject(env, CORE_PACKAGE "ComplexFloat", "(FF)V", fc->real, fc->imag);
}

static jobject createComplexDouble(JNIEnv *env, ComplexDouble *dc)
{
  return createObject(env, CORE_PACKAGE "ComplexDouble", "(DD)V", dc->real, dc->imag);
}

static void getComplexFloat(JNIEnv *env, jobject fc, ComplexFloat *result)
{
  jclass klass = (*env)->FindClass(env, CORE_PACKAGE "ComplexFloat");
  jfieldID reField = (*env)->GetFieldID(env, klass, "r", "F");
  jfieldID imField = (*env)->GetFieldID(env, klass, "i", "F");

  result->real = (*env)->GetFloatField(env, fc, reField);
  result->imag = (*env)->GetFloatField(env, fc, imField);
}

static void getComplexDouble(JNIEnv *env, jobject dc, ComplexDouble *result)
{
  jclass klass = (*env)->FindClass(env, CORE_PACKAGE "ComplexDouble");
  jfieldID reField = (*env)->GetFieldID(env, klass, "r", "D");
  jfieldID imField = (*env)->GetFieldID(env, klass, "i", "D");

  result->real = (*env)->GetDoubleField(env, dc, reField);
  result->imag = (*env)->GetDoubleField(env, dc, imField);
}
<% else %>
#include <complex.h>
typedef float complex ComplexFloat;
typedef double complex ComplexDouble;

static jobject createComplexFloat(JNIEnv *env, ComplexFloat fc) {
  return createObject(env, CORE_PACKAGE "ComplexFloat", "(FF)V", crealf(fc), cimagf(fc));
}

static jobject createComplexDouble(JNIEnv *env, ComplexDouble dc)
{
  return createObject(env, CORE_PACKAGE "ComplexDouble", "(DD)V", creal(dc), cimag(dc));
}

static ComplexFloat getComplexFloat(JNIEnv *env, jobject fc)
{
  jclass klass = (*env)->FindClass(env, CORE_PACKAGE "ComplexFloat");
  jfieldID reField = (*env)->GetFieldID(env, klass, "r", "F");
  jfieldID imField = (*env)->GetFieldID(env, klass, "i", "F");

  return (*env)->GetFloatField(env, fc, reField) + I*(*env)->GetFloatField(env, fc, imField);
}

static ComplexDouble getComplexDouble(JNIEnv *env, jobject dc)
{
  jclass klass = (*env)->FindClass(env, CORE_PACKAGE "ComplexDouble");
  jfieldID reField = (*env)->GetFieldID(env, klass, "r", "D");
  jfieldID imField = (*env)->GetFieldID(env, klass, "i", "D");

  return (*env)->GetDoubleField(env, dc, reField) + I*(*env)->GetDoubleField(env, dc, imField);
}
<% end %>

static void throwIllegalArgumentException(JNIEnv *env, const char *message)
{
  jclass klass = (*env)->FindClass(env, "java/lang/IllegalArgumentException");

  (*env)->ThrowNew(env, klass, message);
}

/**********************************************************************/
/*                 XERBLA function arguments                          */
/**********************************************************************/

static char *routine_names[] = {
<% for r in routines.sort {|r1, r2| r1.name <=> r2.name} %> "<%= r.name %>", <% end -%>
	0
};

static char *routine_arguments[][<%= (routines.map do |r| r.args.length end).max %>] = {
<% for r in routines.sort {|r1, r2| r1.name <=> r2.name} -%>
   { <%= (r.args.map do |s| '"' + s + '"' end).join(', ') %> }, 
<% end -%>
};

/**********************************************************************/
/*                 Our implementation of XERBLA                       */
/**********************************************************************/
static JNIEnv *savedEnv = 0;


void xerbla_(char *fct, int *info)
{
	static char name[7];
	static char buffer[256];
	int i;
	char **p;
	char **arguments = 0;
	
	for (i = 0; i < 6; i++) {
		if (fct[i] == ' ')
			break;	
		name[i] = fct[i];
	}
	name[i] = '\0';
	
	//fprintf(stderr, "Searching for function \"%s\"\n", name);
	
	for (p = routine_names, i = 0; *p; p++, i++)
		if (!strcmp(*p, name))
			arguments = routine_arguments[i];
			
	if (!arguments) {
		sprintf(buffer, "XERBLA: Error on argument %d for *unknown function* %s (how odd!)\n", *info, name);
	}
	else {
		sprintf(buffer, "XERBLA: Error on argument %d (%s) in %s", *info, arguments[*info-1], name);
	}
	throwIllegalArgumentException(savedEnv, buffer);
}

/**********************************************************************/
/*                 generated functions below                          */
/**********************************************************************/

<% for r in routines %>
<%= generate_wrapper r -%>
<% end %>
