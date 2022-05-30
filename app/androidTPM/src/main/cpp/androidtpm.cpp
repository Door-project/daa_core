#include <jni.h>
#include <string>
#include "TPMInterface.h"


extern "C"
JNIEXPORT void JNICALL
Java_com_daaBridge_androidtpm_AndroidTPM_startTPM(JNIEnv *env, jobject thiz) {
    // TODO: implement startTPM()
    startInternalTPM();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_daaBridge_androidtpm_AndroidTPM_endTPM(JNIEnv *env, jobject thiz) {
    // TODO: implement endTPM()
}