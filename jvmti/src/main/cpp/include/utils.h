//
// Created by ianlong(龙毅) on 2020/7/11.
//

#ifndef JVMTI_UTILS_H
#define JVMTI_UTILS_H

#include <jni.h>
#include "jvmti.h"

JavaVM *getJavaVM(JNIEnv *env);
jvmtiEnv *getJvmtiEnv(JavaVM *vm);
jvmtiEnv *getJvmtiEnvFromJNI(JNIEnv *env);

#endif //JVMTI_UTILS_H
