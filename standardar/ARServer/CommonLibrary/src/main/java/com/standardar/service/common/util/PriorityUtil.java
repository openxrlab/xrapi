package com.standardar.service.common.util;

public class PriorityUtil {
    static {
        System.loadLibrary("common_lib");
    }
    public static native void setPriority(int priority);
    public static native void bindCore();
}
