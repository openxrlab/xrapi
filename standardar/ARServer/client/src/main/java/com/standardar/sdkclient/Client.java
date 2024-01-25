package com.standardar.sdkclient;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.util.Log;

import com.standardar.algorithm.AlgorithmProxy;
import com.standardar.common.CommandConstant;
import com.standardar.common.FrameworkConstant;
import com.standardar.sensor.camera.CameraSource;
import com.standardar.sensor.camera.SImageV1;
import com.standardar.sensor.imu.IMUReader;
import com.standardar.service.common.util.LogUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.Charset;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Locale;

public class Client implements CameraSource.ICameraNotifyCallback, IMUReader.ISensorNotifyCallback  {
    public static final int POSE_NUM    = 7;
    public static final int ARCLOUD_ANCHOR_VERSION  = 0;
    private ClientServiceImpl mServiceImpl;

    public Client(Context serverContext, Context sdkContext) {
        mServiceImpl = new ClientServiceImpl(serverContext, sdkContext);
    }

    @Override
    public void onCameraNotify(SImageV1 sImageV1) {
        if (sImageV1 == null) {
            return;
        }
        mServiceImpl.processFrame(sImageV1);
    }

	public void getSLAMResult() {
        mServiceImpl.sendCommand(CommandConstant.COMMAND_SLAM_GET_RESULT);
    }

    @Override
    public void onSensorChanged(float[] values, int tag, long timestamp) {
//        Util.LOGI("sx: imu delay " + (SystemClock.elapsedRealtimeNanos() - timestamp) * 1.0e-6);
        mServiceImpl.pushSensorData(values, tag, timestamp);
    }

    public void setPackageName(Context context, String packageName){
        String versionName = "";
        String pkgName = "";
        PackageManager pm = context.getPackageManager();
        try {
            PackageInfo packageInfo = pm.getPackageInfo(context.getPackageName(), 0);
            versionName = packageInfo.versionName;
            pkgName = packageInfo.packageName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        mServiceImpl.setValue(FrameworkConstant.ADAPTER_KEY_PACKAGENAME, pkgName);
        mServiceImpl.setValue(FrameworkConstant.ADAPTER_KEY_VERSIONNAME, versionName);
    }

    public void setSLAMPlaneMode(int mode){
        mServiceImpl.setValue(FrameworkConstant.ADAPTER_KEY_PLANE_MODE, mode);
    }

    public void setCloudAnchorMode(int mode){
        mServiceImpl.setValue(FrameworkConstant.ADAPTER_KEY_CLOUD_MODE, mode);
    }

    public void setSLAMLightEstimateMode(int mode){
        mServiceImpl.setValue(FrameworkConstant.ADAPTER_KEY_LIGHT_ESTIMATE, mode);
    }

    public void setSLAMWorldAligmentMode(int mode){
        mServiceImpl.setValue(FrameworkConstant.ADAPTER_KEY_WORLD_ALIGN, mode);
    }

    public void setValue(String key, Object value) {
        mServiceImpl.setValue(key, value);
    }

    public void setHandGestureMode(int mode){
        mServiceImpl.setValue(FrameworkConstant.ADAPTER_KEY_HAND_GESTURE_MODE, mode);
    }

    public void setDenseReconMode(int mode) {
        mServiceImpl.setValue(FrameworkConstant.ADAPTER_KEY_DENSE_RECON, mode);
    }

    public void setObjectTrackingMode(int mode) {
        mServiceImpl.setValue(FrameworkConstant.ADAPTER_KEY_OBJECT_TRACKING, mode);
    }

    public void setObjectScanningMode(int mode) {
        mServiceImpl.setValue(FrameworkConstant.ADAPTER_KEY_OBJECT_SCANNING, mode);
    }

    public void initAlgorithm() {
        mServiceImpl.initAlgorithm();
    }

    public void startAlgorithm(int algorithmType) {
        mServiceImpl.startAlgorithm(algorithmType);
    }

    public void stopAlgorithm(int algorithmType) {
        mServiceImpl.stopAlgortihm(algorithmType);
    }

    public void destroyAlgorithm() {
        mServiceImpl.destoryAlgorithm();
    }

    public AlgorithmProxy enableAlgorithm(int algorithmType) {
        mServiceImpl.enableAlgorithm(algorithmType);
        return mServiceImpl.getAlgorithmProxy(algorithmType);
    }

    public void disableAlgorithm(int algorithmType) {
        mServiceImpl.disableAlgorithm(algorithmType);
    }

    public void setAlgorithmStreamMode(int algorithmType, int algorithmStreamMode) {
        mServiceImpl.setAlgorithmStreamMode(algorithmType, algorithmStreamMode);
    }

    public void setDeviceType(int algorithmType, int deviceType) {
        mServiceImpl.setDeviceType(algorithmType, deviceType);
    }

    public void setResumeFlag() {
        mServiceImpl.sendCommand(CommandConstant.COMMAND_SLAM_SET_RESUME_FLAG);
    }

    public void hostAnchor(Context context, int override, float[] anchor, String mapId, String appKey, String appSecret) {
        String packageName = "";
        String signature = "";
        if (context != null) {
            packageName = context.getPackageName();
            signature = getSignature(packageName, context);
        }

        byte[] appKeyBytes = appKey.getBytes(Charset.forName("UTF-8"));
        byte[] appSecretBytes = appSecret.getBytes(Charset.forName("UTF-8"));
        byte[] packageNameBytes = packageName.getBytes(Charset.forName("UTF-8"));
        byte[] signatureBytes = signature.getBytes(Charset.forName("UTF-8"));
        int appKeyLength = appKeyBytes.length;
        int appSecretLength = appSecretBytes.length;
        int packageLength = packageNameBytes.length;
        int signatureLength = signatureBytes.length;
        int strLength;
        byte[] mapIdBytes;
        if(override == 1) {
            mapIdBytes = mapId.getBytes(Charset.forName("UTF-8"));
            strLength = Integer.BYTES + mapIdBytes.length + appKeyLength + appSecretLength + packageLength + signatureLength;
        } else {
            mapIdBytes = new byte[0];
            strLength = appKeyLength + appSecretLength + packageLength + signatureLength;
        }
        ByteBuffer buffer = ByteBuffer.allocate(strLength + Integer.BYTES * 6 + Float.BYTES * POSE_NUM);
        buffer.putInt(ARCLOUD_ANCHOR_VERSION);
        buffer.putInt(override);
        if(override == 1) {
            buffer.putInt(mapIdBytes.length);
            buffer.put(mapIdBytes);
        }
        for(int i = 0; i < POSE_NUM; i++) {
            buffer.putFloat(anchor[i]);
        }
        buffer.putInt(packageLength);
        buffer.put(packageNameBytes);
        buffer.putInt(signatureLength);
        buffer.put(signatureBytes);
        buffer.putInt(appKeyLength);
        buffer.put(appKeyBytes);
        buffer.putInt(appSecretLength);
        buffer.put(appSecretBytes);

        mServiceImpl.sendCommand(CommandConstant.COMMAND_SLAM_HOST_CLOUD_ANCHOR, buffer);

    }

    public void resolveAnchor(Context context, String anchorId, String appKey, String appSecret){
        String packageName = context.getPackageName();
        String signature = getSignature(packageName, context);
        byte[] anchorIdBytes = anchorId.getBytes(Charset.forName("UTF-8"));
        byte[] appKeyBytes = appKey.getBytes(Charset.forName("UTF-8"));
        byte[] appSecretBytes = appSecret.getBytes(Charset.forName("UTF-8"));
        byte[] packageNameBytes = packageName.getBytes(Charset.forName("UTF-8"));
        byte[] signatureBytes = signature.getBytes(Charset.forName("UTF-8"));
        int anchorIdLength = anchorIdBytes.length;
        int appKeyLength = appKeyBytes.length;
        int appSecretLength = appSecretBytes.length;
        int packageLength = packageNameBytes.length;
        int signatureLength = signatureBytes.length;
        ByteBuffer buffer = ByteBuffer.allocate(Integer.BYTES * 6 + anchorIdLength + appKeyLength + appSecretLength + packageLength + signatureLength);
        buffer.putInt(ARCLOUD_ANCHOR_VERSION);
        buffer.putInt(anchorIdLength);
        buffer.put(anchorIdBytes);
        buffer.putInt(packageLength);
        buffer.put(packageNameBytes);
        buffer.putInt(signatureLength);
        buffer.put(signatureBytes);
        buffer.putInt(appKeyLength);
        buffer.put(appKeyBytes);
        buffer.putInt(appSecretLength);
        buffer.put(appSecretBytes);
        mServiceImpl.sendCommand(CommandConstant.COMMAND_SLAM_RESOLVE_CLOUD_ANCHOR, buffer);
    }

    public void changeLightEstimationMode(int mode) {
        mServiceImpl.sendCommand(CommandConstant.COMMAND_SLAM_CHANGE_LIGHT_MODE, mode);
    }

    private String getSignature(String pkgname, Context context) {
        try {
            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(pkgname, PackageManager.GET_SIGNATURES);
            byte[] cert = packageInfo.signatures[0].toByteArray();

            MessageDigest md = MessageDigest.getInstance("SHA1");
            byte[] publicKey = md.digest(cert);
            StringBuilder hexString = new StringBuilder();
            for (int i = 0; i < publicKey.length; i++) {
                if (i != 0)
                    hexString.append(":");
                String appendString = Integer.toHexString(0xFF & publicKey[i])
                        .toUpperCase(Locale.US);
                if (appendString.length() == 1)
                    hexString.append("0");
                hexString.append(appendString);
            }
            return hexString.toString();
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }
        return "";
    }

    public void pushKeyFrame(byte[] imgBuff, long timestamp, int width, int height,
                            double[] rot, double[] trans) {
        mServiceImpl.sendCommand(CommandConstant.COMMAND_SLAM_PUSH_KEY_FRAME,
                                  imgBuff, timestamp, width, height, rot, trans);
    }

    public void runDenseRecon() {
        mServiceImpl.sendCommand(CommandConstant.COMMAND_SLAM_RUN_DENSE_RECON);
    }

    public void updateAlgorithmResult() {
        mServiceImpl.updateAlgorithmResult();
    }

    public void setAxisUpMode(int mode) {
        mServiceImpl.sendCommand(CommandConstant.COMMAND_COMMON_AXISUPMODE, mode);
    }

    public void setUserJson(String json) {

    }

    public void setUrl(String url) {

    }

    public void setBluetoothUUID(String uuid) {

    }

    public void setSLAMMode(int mode) {

    }
}
