//
// Created by ianlong(龙毅) on 2020/7/11.
//

#include "log.h"
#include "EventCallbacks.h"


void JNICALL EventCallbacks::ThreadStart(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread) {
    jvmtiThreadInfo threadInfo;
    jvmti_env->GetThreadInfo(thread, &threadInfo);
    ALOGI("thread %s started", threadInfo.name);
}

void JNICALL EventCallbacks::ThreadEnd(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread) {
    jvmtiThreadInfo threadInfo;
    jvmti_env->GetThreadInfo(thread, &threadInfo);
    ALOGI("thread %s end", threadInfo.name);
}
