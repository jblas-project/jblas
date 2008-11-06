#include "com_fhg_la_MyNativeClass.h"

JNIEXPORT jint JNICALL Java_test_MyNativeClass_dot(JNIEnv * env, jclass p1, jobject buf) {
	jlong lo = (*env)->GetDirectBufferCapacity(env, buf);
	jint* p  = (*env)->GetDirectBufferAddress(env, buf);

	if(p == NULL) return -1;
	
	int i;
	for(i=0;i < lo; i++)
		p[i] = i;

	return (int)lo;
}

