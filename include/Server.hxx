 /**********************************************
 JNI server
 Author: Pablo Baena
**********************************************/
#include <jni.h>

#ifndef SERVER_H
#define SERVER_H

extern "C" {

JNIEXPORT void JNICALL Java_org_com_uy_scanntech_surveillance_CameraServer_startServer
  (JNIEnv *, jobject, jstring, jint, jboolean, jstring, jstring, jstring, jint);

JNIEXPORT void JNICALL Java_org_com_uy_scanntech_surveillance_CameraServer_stopServer
  (JNIEnv *, jobject);

JNIEXPORT void JNICALL Java_org_com_uy_scanntech_surveillance_CameraServer_setPositionedText
  (JNIEnv *, jobject, jstring);

JNIEXPORT void JNICALL Java_org_com_uy_scanntech_surveillance_CameraServer_clearPositionedText
  (JNIEnv *env, jobject obj);

JNIEXPORT jintArray JNICALL Java_org_com_uy_scanntech_surveillance_CameraServer_getImageSize
  (JNIEnv *env, jobject obj);

}

#endif // (SERVER_H)
