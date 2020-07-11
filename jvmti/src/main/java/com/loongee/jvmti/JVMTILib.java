package com.loongee.jvmti;

import android.content.Context;
import android.os.Build;
import android.os.Debug;
import android.util.Log;

import androidx.annotation.RequiresApi;

import java.io.File;
import java.lang.reflect.Method;
import java.nio.file.Files;
import java.nio.file.Paths;

/**
 * created by ianlong on 2020/7/11
 */
class JVMTILib {

    private static String TAG = "JVMTILib";
    private static String LIB_NAME = "jvmti-agent";

    public static void init(Context context) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return;
        }

        String path = createDuplicateLib(context);
        System.load(path);
        attachJvmtiAgent(path, context.getClassLoader());

        // start events
        start();
    }

    private static void attachJvmtiAgent(String agentPath, ClassLoader classLoader) {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                Debug.attachJvmtiAgent(agentPath, null, classLoader);
            } else {
                Class vmDebugClazz = Class.forName("dalvik.system.VMDebug");
                Method attachAgentMethod = vmDebugClazz.getMethod("attachAgent", String.class);
                attachAgentMethod.setAccessible(true);
                attachAgentMethod.invoke(null, agentPath);
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    private static String createDuplicateLib(Context context) {
        // Build.VERSION.SDK_INT >= Build.VERSION_CODES.O
        try {
            String packageCodePath = context.getPackageCodePath();
            ClassLoader classLoader = context.getClassLoader();
            Method findLibrary = ClassLoader.class.getDeclaredMethod("findLibrary", String.class);
            String jvmtiAgentLibPath = (String) findLibrary.invoke(classLoader, LIB_NAME);

            //copy lib to /data/user/0/com.adi.demo/files/jvmti_agent/agent.so
            Log.d(TAG, "jvmti_agent_path: " + jvmtiAgentLibPath);
            File filesDir = context.getFilesDir();
            File jvmtiLibDir = new File(filesDir, "jvmti_agent");
            if (!jvmtiLibDir.exists()) {
                jvmtiLibDir.mkdirs();
            }
            File agentLibSo = new File(jvmtiLibDir, "agent.so");
            if (agentLibSo.exists()) {
                agentLibSo.delete();
            }
            Files.copy(Paths.get(new File(jvmtiAgentLibPath).getAbsolutePath()), Paths.get((agentLibSo).getAbsolutePath()));

            Log.d(TAG, agentLibSo.getAbsolutePath() + ", " + packageCodePath);
            return agentLibSo.getAbsolutePath();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    public static native void start();
}
