/**********************************************************************/
/*                detecting sse level                                 */
/**********************************************************************/

#include <stdio.h>
#include <assert.h>
#include "org_jblas_util_ArchFlavor.h"

/*
 * Thanks to the following pages for helping me out on this.
 *
 * http://softpixel.com/~cwright/programming/simd/cpuid.php
 * http://www.gentoo.org/proj/en/hardened/pic-fix-guide.xml
 */
#define cpuid(func,ax,bx,cx,dx)	\
  __asm__ __volatile__ (" \
        movl %%ebx, %%edi; \
        cpuid;		  \
        movl %%ebx, %1;	  \
        movl %%edi, %%ebx" \
	: "=a" (ax), "=r" (bx), "=c" (cx), "=d" (dx) \
        : "a" (func) \
        : "edi");

/* in edx */
#define SSE (1L << 25)
#define SSE2 (1L << 26)

/* in ecx */
#define SSE3 (1L << 0)

int sse_level() {
  int level = -1;
#ifdef HAS_CPUID
  int a, b, c, d;

  cpuid(1, a, b, c, d);

  if (d & SSE) {
    level = 1;
  }

  if (d & SSE2) {
    level = 2;
  }

  if (c & SSE3) {
    level = 3;
  }
#endif
  return level;
}

JNIEXPORT jint JNICALL Java_org_jblas_util_ArchFlavor_SSELevel(JNIEnv *env, jclass this) {
    return sse_level();
}
