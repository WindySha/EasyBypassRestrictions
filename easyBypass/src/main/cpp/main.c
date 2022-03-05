//
// Created by windysha on 2022.2
//
#include <jni.h>
#include <dlfcn.h>
#include <pthread.h>
#include "bypassed_dlfcn.h"
#include "fake_dlfcn.h"
#include "common.h"

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

JavaVM *global_jvm = NULL;
jclass NativeLib_class = NULL;

typedef void *(*dlopen_function)(const char *, int);

static jlong nativeDlopen(JNIEnv *env, jclass clazz, jlong address, jlong filename,
                          jint flags);

#if defined(__aarch64__)
static jlong nativeDlopen(JNIEnv *env, jclass clazz, jlong address, jlong filename,
                             jint flags) {
    return (jlong)((dlopen_function)address)((char *)filename, (int)flags);
}
#elif defined(__arm__)
__asm__(
"nativeDlopen:\n"
".arm\n"
"ldr r0, [sp]\n"
"ldr r1, [sp, #8]\n"
"bx r2\n"
);
#else
#error "Arch Not Supported Now !!!"
#endif

static void nativeSetHiddenApiExemptions(JNIEnv *env, jclass clazz,
                                         jobjectArray signature_prefixes) {
    if (GetDeviceApiLevel() < ANDROID_P) {
        return;
    }
    jclass vmRumtime_class = (*env)->FindClass(env, "dalvik/system/VMRuntime");
    if (vmRumtime_class == NULL) {
        return;
    }
    jmethodID getRuntime_art_method = (*env)->GetStaticMethodID(env, vmRumtime_class,
                                                                "getRuntime",
                                                                "()Ldalvik/system/VMRuntime;");
    jobject vmRuntime_instance = (*env)->CallStaticObjectMethod(env, vmRumtime_class,
                                                                (jmethodID) getRuntime_art_method);
    jmethodID setHiddenApiExemptions_art_method = (*env)->GetMethodID(env, vmRumtime_class,
                                                                      "setHiddenApiExemptions",
                                                                      "([Ljava/lang/String;)V");
    if (JNIExceptionCheck(env)) {
        return;
    }
    (*env)->CallVoidMethod(env, vmRuntime_instance, (jmethodID) setHiddenApiExemptions_art_method,
                           signature_prefixes);
    if (!JNIExceptionCheck(env)) {
        LOGD("Bypass hidden api restriction success.");
    }
}

static const JNINativeMethod gMethods[] = {
        {"nativeDlopen",                 "(JJI)J",                 (void *) nativeDlopen},
        {"nativeSetHiddenApiExemptions", "([Ljava/lang/String;)V", (void *) nativeSetHiddenApiExemptions},
};

jint JNI_OnLoad(JavaVM *jvm, void *p) {
    JNIEnv *env = NULL;
    if ((*jvm)->GetEnv(jvm, (void **) (&env), JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;

    global_jvm = jvm;

    jclass localClass = (*env)->FindClass(env, "com/wind/storm/easybypass/EasyBypass");
    if (JNIExceptionCheck(env)) {
        LOGE("Cannot find class com/wind/storm/easybypass/EasyBypass");
        return JNI_ERR;
    }
    NativeLib_class = (jclass) ((*env)->NewGlobalRef(env, localClass));
    (*env)->DeleteLocalRef(env, localClass);

    int rc = (*env)->RegisterNatives(env, NativeLib_class, gMethods, NELEM(gMethods));
    if (rc != JNI_OK) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return rc;
    }
    return JNI_VERSION_1_6;
}