#include <stdio.h>

/* Thanks to the following pages for helping me out on this.
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
  return level;
}

int main(int argc, char **argv) {
  switch (sse_level()) {
  case 1:
    printf("sse\n");
    break;
  case 2:
    printf("sse2\n");
    break;
  case 3:
    printf("sse3\n");
    break;
  }
}
