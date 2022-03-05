#ifndef EASYBYPASSRESTRICTIONS_COMMON_H
#define EASYBYPASSRESTRICTIONS_COMMON_H

#include <android/api-level.h>
#include <android/log.h>

#define ANDROID_N                       24
#define ANDROID_N_1                     25
#define ANDROID_P                       28
#define ANDROID_Q                       29
#define ANDROID_R                       30

#define EXPORTFUNC  __attribute__ ((visibility ("default")))

#define  LOG_TAG "BypassNativeLib"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__)

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Shdr Elf64_Shdr
#define Elf_Sym  Elf64_Sym

static const char *const kSystemLibDir = "/system/lib64/";
static const char *const kOdmLibDir = "/odm/lib64/";
static const char *const kVendorLibDir = "/vendor/lib64/";
static const char *const kApexLibDir = "/apex/com.android.runtime/lib64/";
static const char *const kApexArtNsLibDir = "/apex/com.android.art/lib64/";
static const char *const kLinkerFilePath = "/system/bin/linker64";
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Shdr Elf32_Shdr
#define Elf_Sym  Elf32_Sym

static const char *const kSystemLibDir = "/system/lib/";
static const char *const kOdmLibDir = "/odm/lib/";
static const char *const kVendorLibDir = "/vendor/lib/";
static const char *const kApexLibDir = "/apex/com.android.runtime/lib/";
static const char *const kApexArtNsLibDir = "/apex/com.android.art/lib/";
static const char *const kLinkerFilePath = "/system/bin/linker";
#endif

static inline int JNIExceptionCheck(JNIEnv *env) {
    if ((*env)->ExceptionCheck(env)) {
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
        return 1;
    }
    return 0;
}

static inline int GetDeviceApiLevel() {
    static int level = -1;
    if (level < 0) {
        level = android_get_device_api_level();
    }
    return level;
}

#endif //EASYBYPASSRESTRICTIONS_COMMON_H
