/*

 */

#include <glad/glad.h>

#include <vector>
#include <algorithm>
#include <time.h>

//#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Point.h"
#include "Vec3.h"
#include "Matrix3x3.h"
#include "ResizingArray.h"

//#include <EGL/egl.h> // : the main EGL API definitions.
//#include <EGL/eglext.h>


//jint JNI_OnLoad(JavaVM *vm, void *reserved) {
//    LOGI ("JNI_OnLoad");
//    return JNI_VERSION_1_6;
//}

// good place to create textures, or pass in from java side (resource -> byte array -> void* ?)
void initialize(const char* strVertexSrc, const char* strFragmentSrc,
                const char* strTexVpSrc, const char* strTexFpSrc)
{
	masterScene.printGLString("Version", GL_VERSION);
    masterScene.printGLString("Vendor", GL_VENDOR);
    masterScene.printGLString("Renderer", GL_RENDERER);
    masterScene.printGLString("Extensions", GL_EXTENSIONS);

//	LOGI("setupGraphics(%d, %d)", w, h);
    masterScene.createPrograms(strVertexSrc, strFragmentSrc,
                               strTexVpSrc, strTexFpSrc);
    //masterScene.initTextures();
}

void drawframe(int invalue) {
    masterScene.drawFrame();
}

// device orientation matrix is passed into native side from device
// device -> java side -> JNI -> native side
void receiveMatrix(float* matIn) {
    masterScene.storeOrientation(matIn);
}


//extern "C" {
//JNIEXPORT void JNICALL Java_com_neusoft_particle_ObjectJNI_jni_surfaceChanged
//        (JNIEnv *, jclass, jint inwidth, jint inheight) {
//
//}
//
//JNIEXPORT void JNICALL Java_com_neusoft_particle_ObjectJNI_jni_1surfaceChanged
//        (JNIEnv *, jclass, jint inwidth, jint inheight) {
//    masterScene.surfaceChanged(inwidth, inheight);
//}
//
//JNIEXPORT void JNICALL Java_com_neusoft_particle_ObjectJNI_jni_1touchdownHandler
//        (JNIEnv *, jclass, jint invalue) {
//    masterScene.toggleTouchdown();
//}


/*
 * Class:     com_neusoft_particle_ObjectJNI
 * Method:    jni_initialize
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
// public static native void jni_initialize(String strVertexSrc, String strFragmentSrc);

//public static native void jni_initialize(String strVertexSrc, String strFragmentSrc);
//JNIEXPORT void JNICALL Java_com_neusoft_particle_ObjectJNI_jni_1initialize
//        (JNIEnv *env, jclass, jstring strVertexSrc, jstring strFragmentSrc,
//        jstring strTextureVp, jstring strTextureFp)
//{
//    unsigned char whatisthis = 0;
//    const char *strVertex = env->GetStringUTFChars(strVertexSrc, &whatisthis);
//    const char *strFragment = env->GetStringUTFChars(strFragmentSrc, &whatisthis);
//
//    const char *strTextureVertex = env->GetStringUTFChars(strTextureVp, &whatisthis);
//    const char *strTextureFragment = env->GetStringUTFChars(strTextureFp, &whatisthis);
//
//    initialize(strVertex, strFragment, strTextureVertex, strTextureFragment);
//}

/*
 * Class:     com_neusoft_particle_ObjectJNI
 * Method:    jni_drawframe
 * Signature: (I)V
 */
//JNIEXPORT void JNICALL Java_com_neusoft_particle_ObjectJNI_jni_1drawframe
//        (JNIEnv *, jclass, jint invalue) {
//    drawframe(invalue);
//}
//
//JNIEXPORT void JNICALL Java_com_neusoft_particle_ObjectJNI_jni_1receiveMatrix
//        (JNIEnv *env, jclass, jfloatArray jfa) {
//    jfloatArray result;
//    result = env->NewFloatArray(16);
//    if (result == NULL) {
//    // todo: handle error, can't allocate new float array
//    }
//
//    jfloat matrix[16];
//    jfloat *pmatrix = env->GetFloatArrayElements(jfa, 0);
//
//    receiveMatrix(pmatrix);
//
//    env->ReleaseFloatArrayElements(jfa, pmatrix, 0);
//}

//JNIEXPORT void JNICALL Java_com_neusoft_particle_ObjectJNI_jni_1pushTexture(JNIEnv* env,
//                                                           jclass,
//                                                           jintArray arr, jint w, jint h) {
//    int len = w * h * 4;
//
//    // todo: free resource?
//    int* body = env->GetIntArrayElements(arr, NULL);
//
//    GLubyte* cImgData;
//    cImgData = new GLubyte[len];
//
//    unsigned int pos = 0;
//    // Converts ARGB data from android Bitmap into RGBA (OpenGL)
//    for(int i=0; i<len; i++) {
//        cImgData[pos++] = (body[i] & 0xff0000) >> 16; // red
//        cImgData[pos++] = (body[i] & 0xff00) >> 8; // grn
//        cImgData[pos++] = body[i] & 0xff; // blu
//        cImgData[pos++] = (body[i] & 0xff000000) >> 24; // alpha
//    }
//
//    masterScene.storeTexture(cImgData, w, h);
//    delete[] cImgData;
//}

//} // end extern "C"


