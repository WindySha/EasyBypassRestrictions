#include <jni.h>
#include <dlfcn.h>
#include <pthread.h>
#include <android/log.h>
#include "bypassed_dlfcn.h"
#include "fake_dlfcn.h"

#define LOG_TAG  "Test_Bypass_Dlfcn_Restriction"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,  LOG_TAG, __VA_ARGS__)

static void test_bypass_dlopen() {
    void *handle = bypass_dlopen("libart.so", RTLD_NOLOAD);
    LOGD(" bypass_dlopen libart.so result -> %p", handle);
    if (handle != nullptr) {
        void *runtime_instance = dlsym(handle, "_ZN3art7Runtime9instance_E");
        LOGD(" dlsym runtime_instance result -> %p", runtime_instance);
    }
    dlclose(handle);
}

static void test_bypass_loader_dlopen() {
    void *handle2 = bypass_loader_dlopen("libart.so", RTLD_NOLOAD);
    LOGD(" bypass_loader_dlopen libart.so result -> %p", handle2);
    if (handle2 != nullptr) {
        void *runtime_instance = dlsym(handle2, "_ZN3art7Runtime9instance_E");
        LOGD(" bypass_loader_dlopen dlsym runtime_instance result -> %p", runtime_instance);
    }
    dlclose(handle2);
}

static void test_fake_dlopen() {
    void *handle = fake_dlopen("libart.so");
    LOGD("fake_dlopen libart.so result -> %p", handle);

    if (handle != nullptr) {
        void *runtime_instance = fake_dlsym(handle, "_ZN3art7Runtime9instance_E");
        LOGD(" fake_dlsym runtime_instance result -> %p", runtime_instance);

        void *callback = fake_dlsym_symtab(handle, "MterpCommonFallback");
        LOGD(" fake_dlsym callback result -> %p", callback);
    }

    fake_dlclose(handle);
}

static void *thread_fun(void *arg) {
    LOGD("start test bypass_dlopen called in worker thread  ######### ");
    test_bypass_dlopen();
    pthread_detach(pthread_self());
    return nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wind_storm_bypass_MainActivity_testDlFunction(JNIEnv *env, jclass clazz) {

    LOGD("start test bypass_dlopen called in main thread ------------- ");
    test_bypass_dlopen();
    test_bypass_loader_dlopen();

    pthread_t pt;
    pthread_create(&pt, NULL, &thread_fun, NULL);

    test_fake_dlopen();
}