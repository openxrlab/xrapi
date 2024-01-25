package com.standardar.service.common.util;

import android.os.SystemClock;
import android.util.Log;

public class LogUtils {
    private static final String TAG = "arserver";
    private static boolean DEBUG = false;

    private static long time = 0;

    public static void LOGD(String log) {
        if (DEBUG) {
            Log.d(TAG, log);
        }
    }

    public static void LOGI(String log) {
        Log.i(TAG, log);
    }

    public static void LOGW(String log) {
        Log.w(TAG, log);
    }

    public static void LOGE(String log) {
        Log.e(TAG, log);
    }

    public static void timeStart() {
        time = SystemClock.elapsedRealtimeNanos();
    }

    public static void timeEnd(String msg) {
        LOGI(msg + " cost:" + (SystemClock.elapsedRealtimeNanos() - time) / 1000000.0f + " ms");
    }
}
