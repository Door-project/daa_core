#include <jni.h>
#include <string>
#include "daa_bridge.h"
#include "Test_issuer/issuer_interface.h"

TPM2B_PUBLIC ek;
TPM2B_NONCE currentSession;



extern "C"
JNIEXPORT void JNICALL
Java_com_example_daabridgecpp_MainActivity_registerWalletPK(JNIEnv *env, jobject thiz,
                                                            jbyteArray pem_file) {
    // TODO: Write to Wallet_keys
}



extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_daabridgecpp_MainActivity_daaIssuerRegistration(JNIEnv *env, jobject thiz,
                                                                 jbyteArray signedNonce, jint len) {
    // Setup Endorsement Key
    ek = setup();

    // Todo: correct cast
    onNewSessionFromVCIssuer(&ek, reinterpret_cast<uint8_t *>(signedNonce), len);

    //Todo: Correct casting
    return reinterpret_cast<jbyteArray>(credentialDigest);


}
extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_example_daabridgecpp_MainActivity_startDAASession(JNIEnv *env, jobject thiz) {
    requestNonce(&currentSession);
    //Todo: make work correctly
    return reinterpret_cast<jbyteArray>(currentSession.b.buffer);
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_daabridgecpp_MainActivity_daaSign(JNIEnv *env, jobject thiz, jbyteArray data,
                                                   jint len, jbyteArray signed_nonce,
                                                   jint signed_nonce_len) {
    // TODO: Fix cast
    execute_daa_sign(reinterpret_cast<uint8_t *>(data), len,
                     reinterpret_cast<uint8_t *>(signed_nonce), signed_nonce_len);


}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_daabridgecpp_MainActivity_daaSignWithEvidence(JNIEnv *env, jobject thiz,
                                                               jbyteArray data, jint len,
                                                               jbyteArray signed_nonce,
                                                               jint signed_nonce_len,
                                                               jbyteArray evidence, jint len_1) {

    //TODO: Verify evidence
    execute_daa_sign(reinterpret_cast<uint8_t *>(data), len,
                     reinterpret_cast<uint8_t *>(signed_nonce), signed_nonce_len);
}