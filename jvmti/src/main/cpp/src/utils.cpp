//
// Created by ianlong(龙毅) on 2020/7/11.
//

#include "utils.h"

JavaVM *getJavaVM(JNIEnv *env) {
    JavaVM *javaVm;
    jint result = env->GetJavaVM(&javaVm);
    if (result != JNI_OK) {
        return nullptr;
    }
    return javaVm;
}

jvmtiEnv *getJvmtiEnv(JavaVM *vm) {
    jvmtiEnv *jvmti_env;
    jint result = vm->GetEnv((void **) &jvmti_env, JVMTI_VERSION_1_2);
    if (result != JNI_OK) {
        return nullptr;
    }
    return jvmti_env;
}

jvmtiEnv *getJvmtiEnvFromJNI(JNIEnv *env) {
    JavaVM *vm = getJavaVM(env);
    return getJvmtiEnv(vm);
}

