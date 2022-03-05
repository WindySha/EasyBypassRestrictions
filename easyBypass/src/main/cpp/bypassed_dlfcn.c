//
// Created by windysha
//

#include <jni.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <stdio.h>
#include <pthread.h>
#include "bypassed_dlfcn.h"
#include "include/fake_dlfcn.h"
#include "common.h"

extern JavaVM *global_jvm;
extern jclass NativeLib_class;

static jmethodID dlopen_method_id = NULL;
static void *dlopen_func_addr_N = NULL;
static void *g_dl_mutex_addr_N = NULL;

typedef void *(*loader_dlopen_func)(const char *filename, int flag, void *address);

typedef void *(*loader_dlopen_func_N)(const char *filename, int flags, const void *extinfo,
                                      void *caller);

static void *getLibArtBaseAddress() {
    static void *art_base_address = NULL;
    if (art_base_address != NULL) {
        return art_base_address;
    }
    const char *art_name = "libart.so";
    const char *so_prefix;
    int level = GetDeviceApiLevel();
    if (level >= ANDROID_R) {
        so_prefix = kApexArtNsLibDir;
    } else if (level == ANDROID_Q) {
        so_prefix = kApexLibDir;
    } else {
        so_prefix = kSystemLibDir;
    }
    char target_so_path[256];
    sprintf(target_so_path, "%s%s", so_prefix, art_name);
    art_base_address = get_base_address(target_so_path);

    return art_base_address;
}

EXPORTFUNC void *bypass_loader_dlopen(const char *filename, int flag) {
    int api_level = GetDeviceApiLevel();
    if (api_level < ANDROID_N) {
        return dlopen(filename, flag);
    }

    if (api_level == ANDROID_N || api_level == ANDROID_N_1) {
        if (dlopen_func_addr_N == NULL) {
            void *linker_handle = fake_dlopen(kLinkerFilePath);
            dlopen_func_addr_N = fake_dlsym(linker_handle,
                                            "__dl__ZL10dlopen_extPKciPK17android_dlextinfoPv"); // this symbol not exist in arm64
            if (dlopen_func_addr_N == NULL) {
                dlopen_func_addr_N = fake_dlsym(linker_handle,
                                                "__dl__Z9do_dlopenPKciPK17android_dlextinfoPv");
                g_dl_mutex_addr_N = fake_dlsym(linker_handle,
                                               "__dl__ZL10g_dl_mutex");
            }
            fake_dlclose(linker_handle);
        }

        if (dlopen_func_addr_N == NULL) {
            LOGE(" bypass_loader_dlopen failed, dlopen_ext or do_dlopen function address cannot found!!");
            return NULL;
        }

        loader_dlopen_func_N do_dlopen_N = (loader_dlopen_func_N) (dlopen_func_addr_N);

        void *called_adddres = getLibArtBaseAddress();
        if (called_adddres == NULL) {
            called_adddres = (void *) dlerror;
        }
        if (g_dl_mutex_addr_N != NULL) pthread_mutex_lock(g_dl_mutex_addr_N);
        void *result = do_dlopen_N(filename, flag, NULL, called_adddres);
        if (g_dl_mutex_addr_N != NULL) pthread_mutex_unlock(g_dl_mutex_addr_N);
        return result;

    } else {
        void *handle = dlopen("libdl.so", RTLD_NOW);
        loader_dlopen_func loader_dlopen = (loader_dlopen_func) (dlsym(handle, "__loader_dlopen"));
        if (loader_dlopen == NULL) {
            return NULL;
        }
        void *result_handle = loader_dlopen(filename, flag, (void *) dlerror);
        if (result_handle != NULL) return result_handle;

        void *called_adddres = getLibArtBaseAddress();
        if (called_adddres == NULL) {
            return NULL;
        }
        return loader_dlopen(filename, flag, called_adddres);
    }
}

EXPORTFUNC void *bypass_dlopen(const char *filename, int flag) {
    if (GetDeviceApiLevel() < ANDROID_N) {
        return dlopen(filename, flag);
    }
    JNIEnv *env = NULL;
    bool is_attched_env = false;
    if (global_jvm == NULL) {
        goto loader_dlopen;
    }
    (*global_jvm)->GetEnv(global_jvm, (void **) (&env), JNI_VERSION_1_6);

    if (env == NULL) {
        (*global_jvm)->AttachCurrentThread(global_jvm, &env, NULL);
        is_attched_env = true;
    }
    if (env == NULL) {
        goto loader_dlopen;
    }
    if (NativeLib_class == NULL) {
        LOGE(" NativeLib_class is null");
        jclass localClass = (*env)->FindClass(env, "com/wind/storm/easybypass/EasyBypass");
        if (JNIExceptionCheck(env) || localClass == NULL) {
            LOGW("Find class com/wind/storm/easybypass/EasyBypass failed, you may do dlopen in the native worker thread.");
            goto loader_dlopen;
        }
        NativeLib_class = (jclass) ((*env)->NewGlobalRef(env, localClass));
        (*env)->DeleteLocalRef(env, localClass);
    }

    if (dlopen_method_id == NULL) {
        dlopen_method_id = (*env)->GetStaticMethodID(env, NativeLib_class, "nativeDlopen",
                                                     "(JJI)J");
        if (dlopen_method_id == NULL) {
            LOGE(" dlopen_method_id is null");
            goto loader_dlopen;
        }
    }

    jlong handle = (*env)->CallStaticLongMethod(env, NativeLib_class, dlopen_method_id,
                                                (jlong) dlopen,
                                                (jlong) filename, flag);

    if (JNIExceptionCheck(env)) {
        goto loader_dlopen;
    }

    if (handle == 0) goto loader_dlopen;

    if (is_attched_env && env != NULL) {
        (*global_jvm)->DetachCurrentThread(global_jvm);
        env = NULL;
    }
    return (void *) handle;

    loader_dlopen:
    if (is_attched_env && env != NULL) {
        (*global_jvm)->DetachCurrentThread(global_jvm);
        env = NULL;
    }
    return bypass_loader_dlopen(filename, flag);
}