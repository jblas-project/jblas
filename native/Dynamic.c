#include "edu_ida_core_Dynamic.h"
#include <stdio.h>

JNIEXPORT void JNICALL Java_edu_ida_core_Dynamic_hello
  (JNIEnv *env, jclass this)
{
	printf("--hello!\n");
}
