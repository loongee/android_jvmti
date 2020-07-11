//
// Created by ianlong(龙毅) on 2020/7/11.
//

#ifndef JVMTI_EVENT_CALLBACKS_H
#define JVMTI_EVENT_CALLBACKS_H

#include <jni.h>
#include "jvmti.h"

class EventCallbacks {
public:
    static void JNICALL ThreadStart(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread);

    static void JNICALL ThreadEnd(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread);
};


#endif //JVMTI_EVENT_CALLBACKS_H
