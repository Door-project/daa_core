#include <jni.h>
#include <string>

extern "C" {
#include "TPMInterface.h"
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_androidtpm_AndroidTPM_startTPM(JNIEnv *env, jobject thiz);


void Java_com_example_androidtpm_AndroidTPM_startTPM(JNIEnv *env, jobject thiz) {
    startInternalTPM();

}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_androidtpm_AndroidTPM_endTPM(JNIEnv *env, jobject thiz);

void Java_com_example_androidtpm_AndroidTPM_endTPM(JNIEnv *env, jobject thiz) {
    // TODO: implement endTPM()
}