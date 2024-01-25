package com.standardar.common;

import android.Manifest;
import android.annotation.TargetApi;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;

import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

import android.content.pm.ResolveInfo;

import android.content.res.AssetManager;
import android.net.Uri;
import android.os.Build;
import android.os.SystemClock;

import android.text.TextPaint;
import android.util.Log;

import android.view.Display;
import android.view.Gravity;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import java.util.List;
import java.util.Stack;

import static android.content.Context.CONTEXT_IGNORE_SECURITY;

public class Util {
    private static final String TAG = "StandardAR";
    private static boolean DEBUG = false;
    private static long time = 0;
    private static Stack<Long> sTimeStack = new Stack<>();
    private static int mARServerState = 0;

    private static final float DIP_TO_PIX_OFFSET = 0.5f;

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

    /**
     * invoke timeStart firstly and then timeEnd. timeEnd will print the time
     * between timeStart invoke-time and timeEnd invoke-time in ms
     */
    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR1)
    public static void timeStart() {
        sTimeStack.push(SystemClock.elapsedRealtimeNanos());
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR1)
    public static void timeEnd(String msg) {
        if(!sTimeStack.empty()) {
            LOGI(msg + " cost " + (SystemClock.elapsedRealtimeNanos() - sTimeStack.pop()) / 1000000.0f + " ms");
        }
    }

    public static boolean checkCameraPermission(Context context) {
        return context.checkPermission(Manifest.permission.CAMERA, android.os.Process.myPid(), android.os.Process.myUid()) == PackageManager.PERMISSION_GRANTED;
    }

    /**
     * check if pkg is install in this android phone
     * @param context
     * @param action action name to query
     * @return
     */
    public static final int ARSERVER_NOT_INSTALLED = -201;
    public static final int ARSERVER_TOO_OLD = -202;
    public static final int ARSERVER_INSTALLED = 1;
    public static int checkPackageInstalled(Context context, String action) {
        if (context == null || action == null) {
            mARServerState = ARSERVER_NOT_INSTALLED;
            return mARServerState;
        }
        PackageManager pm = context.getPackageManager();
        try {
            PackageInfo pkgInfo = pm.getPackageInfo(FrameworkConstant.ARSERVICE_PACKAGE_NAME, 0);
            if (pkgInfo == null) {
                return ARSERVER_NOT_INSTALLED;
            } else {
                mARServerState = pkgInfo.versionCode >= FrameworkConstant.ARSERVICE_CURRENT_VERSION ?ARSERVER_INSTALLED:ARSERVER_TOO_OLD;
                return mARServerState;
            }
        }catch (PackageManager.NameNotFoundException e){
            mARServerState = ARSERVER_NOT_INSTALLED;
            return mARServerState;
        }
    }

    public static int getVersionCode(Context context) {
        if (context == null) {
            return 0;
        }
        PackageManager pm = context.getPackageManager();
        try {
            PackageInfo info = pm.getPackageInfo(FrameworkConstant.ARSERVICE_PACKAGE_NAME, 0);
            return info.versionCode;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return 0;

    }

    /**
     * check if this android operation system version is larger than version
     * @param version
     * @return
     */
    public static boolean checkAndroidExceed(int version) {
        return Build.VERSION.SDK_INT > version;
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR1)
    public static int getLandscapeDisplayWidth(Context context) {
        WindowManager wm = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        Display d = wm.getDefaultDisplay();
        android.graphics.Point point = new android.graphics.Point();
        d.getRealSize(point);
        return point.x > point.y ? point.x : point.y;
    }

    @TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR1)
    public static int getLandscapeDisplayHeight(Context context) {
        WindowManager wm = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        Display d = wm.getDefaultDisplay();
        android.graphics.Point point = new android.graphics.Point();
        d.getRealSize(point);
        return point.x > point.y ? point.y : point.x;
    }

    public static int dip2px(Context context, float dpValue) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dpValue * scale + DIP_TO_PIX_OFFSET);
    }

    public static boolean isArServiceSingleAlgorithm(Context context) {
        if (context == null) {
            return false;
        }
        PackageManager pm = context.getPackageManager();
        try {
            ApplicationInfo info = pm.getApplicationInfo(FrameworkConstant.ARSERVICE_PACKAGE_NAME,
                    PackageManager.GET_META_DATA);
            if (info == null) {
                return true;
            }
            if (info.metaData == null) {
                return true;
            }
            if ("true".equals(info.metaData.getString("MultiAlgorithm"))) {
                return false;
            } else {
                return true;
            }
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return false;
    }

    public static boolean isArServiceSupportRGBD(Context context) {
        if (context == null) {
            return false;
        }
        PackageManager pm = context.getPackageManager();
        try {
            ApplicationInfo info = pm.getApplicationInfo(FrameworkConstant.ARSERVICE_PACKAGE_NAME,
                    PackageManager.GET_META_DATA);
            if (info == null) {
                return false;
            }
            if (info.metaData == null) {
                return false;
            }
            if ("true".equals(info.metaData.getString("RGBDSLAM"))) {
                return true;
            } else {
                return false;
            }
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return false;
    }

    public static boolean isArServiceSupportHandGesture(Context context) {
        if (context == null) {
            return false;
        }
        PackageManager pm = context.getPackageManager();
        try {
            ApplicationInfo info = pm.getApplicationInfo(FrameworkConstant.ARSERVICE_PACKAGE_NAME,
                    PackageManager.GET_META_DATA);
            if (info == null) {
                return false;
            }
            if (info.metaData == null) {
                return false;
            }
            if ("true".equals(info.metaData.getString("HAND_GESTURE"))) {
                return true;
            } else {
                return false;
            }
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return false;
    }

    public static String getXRServerSupport(Context context) {
        String pkg = FrameworkConstant.ARSERVICE_PACKAGE_NAME;
        try {
            Context xrserverContext = context.createPackageContext(pkg, CONTEXT_IGNORE_SECURITY);
            AssetManager assetManager = xrserverContext.getAssets();
            InputStream inputStream = assetManager.open("support.txt");
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
            String line = "";
            StringBuilder sb = new StringBuilder();
            while ((line = bufferedReader.readLine()) != null) {
                sb.append(line);
            }
            return sb.toString();
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return "";
    }

    public static boolean isArServiceSupportCloudAnchor(Context context) {
        if (context == null) {
            return false;
        }
        PackageManager pm = context.getPackageManager();
        try {
            ApplicationInfo info = pm.getApplicationInfo(FrameworkConstant.ARSERVICE_PACKAGE_NAME,
                    PackageManager.GET_META_DATA);
            if (info == null) {
                return false;
            }
            if (info.metaData == null) {
                return false;
            }
            if ("true".equals(info.metaData.getString("CLOUD_ANCHOR"))) {
                return true;
            } else {
                return false;
            }
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return false;
    }

    public static boolean isArServiceDoubleStream(Context context) {
        if (context == null) {
            return false;
        }
        PackageManager pm = context.getPackageManager();
        try {
            ApplicationInfo info = pm.getApplicationInfo(FrameworkConstant.ARSERVICE_PACKAGE_NAME,
                    PackageManager.GET_META_DATA);
            if (info == null) {
                return false;
            }
            if (info.metaData == null) {
                return false;
            }
            if ("true".equals(info.metaData.getString("DoubleStream"))) {
                return true;
            } else {
                return false;
            }
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return false;
    }

    public static boolean isVersionNewerThan1_9(Context context) {
        PackageManager pckMan = context.getPackageManager();
        List<PackageInfo> packageInfo = pckMan.getInstalledPackages(0);
        for (PackageInfo pInfo : packageInfo) {
            if(FrameworkConstant.ARSERVICE_PACKAGE_NAME.equals(pInfo.packageName)){
                String[] versionName = pInfo.versionName.split("\\.");
                if(Integer.parseInt(versionName[0]) > 1)
                    return true;
                else if(Integer.parseInt(versionName[0]) == 1 && Integer.parseInt(versionName[1]) >= 9)
                    return true;
                else
                    return false;
            }
        }
        return false;
    }

    public static String getARServerMetaData(Context context, String key) {
        if (context == null || key == null) {
            return null;
        }
        PackageManager pm = context.getPackageManager();
        try {
            ApplicationInfo appInfo = pm.getApplicationInfo(FrameworkConstant.ARSERVICE_PACKAGE_NAME, PackageManager.GET_META_DATA);
            if (appInfo == null) {
                return null;
            }
            if (appInfo.metaData == null) {
                return null;
            }
            return appInfo.metaData.getString(key);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static boolean checkARServerSupport(Context context, String slamFeature) {
        String value = getARServerMetaData(context, slamFeature);
        if (FrameworkConstant.ARSERVER_META_TRUE.equals(value)) {
            return true;
        } else {
            return false;
        }
    }

    public static int getIMUHz(Context context, String Path) {
        PackageManager pm = context.getPackageManager();
        String action = FrameworkConstant.MULTI_ALGORITHM_SERVICE_NAME;
        Intent intent = new Intent(action);
        List<ResolveInfo> resolveInfos = pm.queryIntentServices(intent, 0);
        if (resolveInfos == null || resolveInfos.size() != 1) {
            Util.LOGW("arserver number is " + (resolveInfos == null ? 0 : resolveInfos.size()) );
            return 400;
        }
        ResolveInfo serviceInfo = resolveInfos.get(0);
        String pkg = serviceInfo.serviceInfo.packageName;
        try {
            Context xrserverContext = context.createPackageContext(pkg, CONTEXT_IGNORE_SECURITY);
            AssetManager assetManager = xrserverContext.getAssets();
            InputStream inputStream = assetManager.open(Path);
            BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
            String line = "";
            while ((line = bufferedReader.readLine()) != null) {
                if(line.contains("imu_rate_hz")){
                    String[] result = line.split(" ");
                    return Integer.parseInt(result[1]);
                }
            }
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return 400;
    }

    public static native void copyImage(ByteBuffer dst, int order, int pos,  ByteBuffer src, int rgbSize, int depthSize);
    public static native void copyBuffer(ByteBuffer dst, int dstPos, ByteBuffer src, int srcPos, int length);
    public static native String getXRServerPkg();
    public static native void bindCore();


}