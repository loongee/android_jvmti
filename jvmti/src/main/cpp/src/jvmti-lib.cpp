#include <jni.h>
#include <unistd.h>
#include <csignal>
#include <memory>
#include "log.h"
#include "jvmti.h"
#include "utils.h"
#include "EventCallbacks.h"

static jvmtiEnv *jvmti_env;

void SetEventNotification(jvmtiEnv *jvmti, jvmtiEventMode mode,
                          jvmtiEvent event_type) {
    jvmtiError err = jvmti->SetEventNotificationMode(mode, event_type, nullptr);
    if (err != JVMTI_ERROR_NONE) {
        ALOGI("Error on SetEventNotification: %d", err);
    }
}

void JNICALL enableEvents(JNIEnv *env, const int* events, int events_len) {
    if (events_len == 0 || events == nullptr) {
        return;
    }

    for (int i = 0; i < events_len; ++i) {
        SetEventNotification(jvmti_env, JVMTI_ENABLE, jvmtiEvent(events[i]));
    }
}

void SetAllCapabilities(jvmtiEnv *jvmti) {
    jvmtiCapabilities caps;
    jvmtiError error;
    error = jvmti->GetPotentialCapabilities(&caps);
    if (error != JVMTI_ERROR_NONE) {
        ALOGI("GetPotentialCapabilities error: %d", error);
    }
    error = jvmti->AddCapabilities(&caps);
    if (error != JVMTI_ERROR_NONE) {
        ALOGI("AddCapabilities error: %d", error);
    }
}

void dumpAllCapabilities(jvmtiEnv *jvmti) {
    jvmtiCapabilities caps;
    jvmtiError error;
    error = jvmti->GetCapabilities(&caps);
    if (error != JVMTI_ERROR_NONE) {
        ALOGI("dumpAllCapabilities error: %d", error);
        return;
    }

#define DUMP_CAPABILITY(cap) \
    ALOGI(#cap ": %d", caps.cap);

    ALOGI("=========Capabilities=========");
    DUMP_CAPABILITY(can_tag_objects)
    DUMP_CAPABILITY(can_generate_field_modification_events)
    DUMP_CAPABILITY(can_generate_field_access_events)
    DUMP_CAPABILITY(can_get_bytecodes)
    DUMP_CAPABILITY(can_get_synthetic_attribute)
    DUMP_CAPABILITY(can_get_owned_monitor_info)
    DUMP_CAPABILITY(can_get_current_contended_monitor)
    DUMP_CAPABILITY(can_get_monitor_info)
    DUMP_CAPABILITY(can_pop_frame)
    DUMP_CAPABILITY(can_redefine_classes)
    DUMP_CAPABILITY(can_signal_thread)
    DUMP_CAPABILITY(can_get_source_file_name)
    DUMP_CAPABILITY(can_get_line_numbers)
    DUMP_CAPABILITY(can_get_source_debug_extension)
    DUMP_CAPABILITY(can_access_local_variables)
    DUMP_CAPABILITY(can_maintain_original_method_order)
    DUMP_CAPABILITY(can_generate_single_step_events)
    DUMP_CAPABILITY(can_generate_exception_events)
    DUMP_CAPABILITY(can_generate_frame_pop_events)
    DUMP_CAPABILITY(can_generate_breakpoint_events)
    DUMP_CAPABILITY(can_suspend)
    DUMP_CAPABILITY(can_redefine_any_class)
    DUMP_CAPABILITY(can_get_current_thread_cpu_time)
    DUMP_CAPABILITY(can_get_thread_cpu_time)
    DUMP_CAPABILITY(can_generate_method_entry_events)
    DUMP_CAPABILITY(can_generate_method_exit_events)
    DUMP_CAPABILITY(can_generate_all_class_hook_events)
    DUMP_CAPABILITY(can_generate_compiled_method_load_events)
    DUMP_CAPABILITY(can_generate_monitor_events)
    DUMP_CAPABILITY(can_generate_vm_object_alloc_events)
    DUMP_CAPABILITY(can_generate_native_method_bind_events)
    DUMP_CAPABILITY(can_generate_garbage_collection_events)
    DUMP_CAPABILITY(can_generate_object_free_events)
    DUMP_CAPABILITY(can_force_early_return)
    DUMP_CAPABILITY(can_get_owned_monitor_stack_depth_info)
    DUMP_CAPABILITY(can_get_constant_pool)
    DUMP_CAPABILITY(can_set_native_method_prefix)
    DUMP_CAPABILITY(can_retransform_classes)
    DUMP_CAPABILITY(can_retransform_any_class)
    DUMP_CAPABILITY(can_generate_resource_exhaustion_heap_events)
    DUMP_CAPABILITY(can_generate_resource_exhaustion_threads_events)
}

void GCStartCallback(jvmtiEnv *jvmti) {
    ALOGI("startGC");
}

JNIEXPORT void JNICALL start(JNIEnv *env, jclass jclazz) {
    int events[] = { JVMTI_EVENT_GARBAGE_COLLECTION_START,
                     JVMTI_EVENT_THREAD_START,
                     JVMTI_EVENT_THREAD_END,
                     };
    enableEvents(env, events, sizeof(events) / sizeof(events[0]));
}

static JNINativeMethod methods[] = {
        {"start", "()V", (void*)start},
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    ALOGI("==============library load pid: %d====================", getpid());
    jclass clazz = env->FindClass("com/loongee/jvmti/JVMTILib");
    env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM *vm, char *options, void *reserved) {
    // signal(SIGTRAP, ignoreHandler);

    jvmti_env = getJvmtiEnv(vm);

    if (jvmti_env == nullptr) {
        ALOGI("env is null");
        return JNI_ERR;
    }
    SetAllCapabilities(jvmti_env);
    dumpAllCapabilities(jvmti_env);

    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    /*callbacks.ClassPrepare = &ClassPrepare;
    callbacks.VMObjectAlloc = &ObjectAllocCallback;
    callbacks.GarbageCollectionFinish = &GCFinishCallback;
    callbacks.ObjectFree = &ObjectFree;
    callbacks.MonitorContendedEnter = &MonitorContendedEnter;
    callbacks.MonitorContendedEntered = &MonitorContendedEntered;*/
    callbacks.ThreadStart = &EventCallbacks::ThreadStart;
    callbacks.ThreadEnd = &EventCallbacks::ThreadEnd;
    callbacks.GarbageCollectionStart = &GCStartCallback;
    int error = jvmti_env->SetEventCallbacks(&callbacks, sizeof(callbacks));
    if (error != JVMTI_ERROR_NONE) {
        ALOGI("Error on Agent_OnAttach: %d", error);
    }

    ALOGI("==========Agent_OnAttach=======");
    return JNI_OK;

}