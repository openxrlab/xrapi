package com.standardar.sensor.imu;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.res.AssetManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;

import com.standardar.common.IMUConstant;
import com.standardar.service.common.util.LogUtils;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;

@TargetApi(Build.VERSION_CODES.KITKAT)
public class IMUReader implements SensorEventListener {

    static {
        System.loadLibrary("camera_lib");
    }


    private final int Us = 1000000;
    private Context mContext;
    private Sensor mAccelerationSensor;
    private Sensor mGyroscopeSensor;
    private Sensor mRotationVectorSensor;
    private Sensor mGravitySensor;
    private SensorManager mSensorManager;

    private HandlerThread mSensorThread;
    private Handler mSensorHandler;

    private static IMUReader mInstance;
    private static Object mInstanceLock = new Object();

    private Map<Sensor, Integer> mSensorTag = new HashMap<>();

    public interface ISensorNotifyCallback {
        void onSensorChanged(float[] values, int tag, long timestamp);
    }
    
    private ISensorNotifyCallback mSensorNotify = null;

    public void setSensorNotify(ISensorNotifyCallback callback) {
        mSensorNotify = callback;
    }

    public IMUReader(Context context) {
        mContext = context;
    }

    private void initSensor() {
        mSensorManager = (SensorManager) getContext().getSystemService(Context.SENSOR_SERVICE);
        if (mSensorManager == null) {
            LogUtils.LOGE("can not get sensormanager");
            return;
        }
        mAccelerationSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER_UNCALIBRATED);
        if (null == mAccelerationSensor) {
            LogUtils.LOGE("Do not support acceleration uncalibrated sensor");
            mAccelerationSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            if (mAccelerationSensor == null) {
                LogUtils.LOGE("Do not support acceleration sensor");
            }
        }

        mGyroscopeSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE_UNCALIBRATED);
        if (null == mGyroscopeSensor) {
            LogUtils.LOGE("Do not support gyroscope uncalibrated sensor");
            mGyroscopeSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
            if (mGyroscopeSensor == null) {
                LogUtils.LOGE("Do not support gyroscope sensor");
            }
        }

        mRotationVectorSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
        if (null == mRotationVectorSensor) {
            LogUtils.LOGE("Do not support rotation vector sensor");
        }

        mGravitySensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY);
        if (null == mGravitySensor) {
            LogUtils.LOGE("Do not support gravity sensor");
        }

        mSensorTag.put(mAccelerationSensor, IMUConstant.IMU_ACC_TAG);
        mSensorTag.put(mGyroscopeSensor, IMUConstant.IMU_GYRO_TAG);
        if (mRotationVectorSensor != null)
            mSensorTag.put(mRotationVectorSensor, IMUConstant.IMU_RV_TAG);
        if (mGravitySensor != null)
            mSensorTag.put(mGravitySensor, IMUConstant.IMU_GRAVITY_TAG);
        LogUtils.LOGI("open imu sensor");
    }

    public void openSensor() {
        initSensor();
        startSensorThread();
    }

    private void startSensorThread() {
        if (mSensorThread != null) {
            return;
        }
        mSensorThread = new HandlerThread("sensor callback thread", Thread.MAX_PRIORITY);
        mSensorThread.start();
        mSensorHandler = new Handler(mSensorThread.getLooper());
    }

    private void stopSensorThread() {
        if (mSensorThread == null) {
            return;
        }
        mSensorThread.quitSafely();
        try {
            mSensorThread.join();
            mSensorThread = null;
            mSensorHandler = null;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private Context getContext() {
        return mContext;
    }

    public void setContext(Context context) {
        mContext = context;
    }

    private String getSupport(Context serverContext) {
        BufferedReader bufferedReader = null;
        try {
            AssetManager assetManager = serverContext.getAssets();
            InputStream inputStream = assetManager.open("support.txt");
            bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
            String line = "";
            StringBuilder sb = new StringBuilder();
            while ((line = bufferedReader.readLine()) != null) {
                sb.append(line);
            }
            return sb.toString();
        } catch (IOException e) {
            LogUtils.LOGE("support.txt read error");
        } finally {
            try {
                if (bufferedReader != null)
                    bufferedReader.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return "";
    }

    private int getIMUHz(Context serverContext, String path) {
        if (serverContext == null) {
            return 400;
        }
        BufferedReader bufferedReader = null;
        try {
            AssetManager assetManager = serverContext.getAssets();
            InputStream inputStream = assetManager.open(path);
            bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
            String line = "";
            while ((line = bufferedReader.readLine()) != null) {
                if(line.contains("imu_rate_hz")){
                    String[] result = line.split(" ");
                    return Integer.parseInt(result[1]);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                bufferedReader.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return 400;
    }

    public void start(Context serverContext) {
        if (serverContext == null) {
            LogUtils.LOGE("imu servercontext is null");
            return;
        }
        String supportTXT = getSupport(serverContext);
        String path = parserDeviceParmaPath(supportTXT);
        RegisterListener(getIMUHz(serverContext, path));
        LogUtils.LOGI("start imu sensor callback");
    }

    public void stop() {
        UnregisterListener();
        LogUtils.LOGI("stop imu sensor callback");
    }

    public void closeSensor() {
        stopSensorThread();
        mContext = null;
        LogUtils.LOGI("close imu sensor");
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        float[] values = event.values;
        long timestamp = event.timestamp;
        Sensor sensor = event.sensor;
        if (mSensorNotify != null) {
            // LogUtils.LOGI("sx: imu " + sensor.getType() + " delay " + (SystemClock.elapsedRealtimeNanos() - timestamp) / 1.0e6);
            mSensorNotify.onSensorChanged(values, mSensorTag.get(sensor), timestamp);
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    @TargetApi(Build.VERSION_CODES.O)
    private void RegisterListener(int hz) {
        if (mSensorManager == null) {
            LogUtils.LOGE("register sensor listener failed");
            return;
        }

        mSensorManager.registerListener(this, mAccelerationSensor, Us / hz, mSensorHandler);
        mSensorManager.registerListener(this, mGyroscopeSensor, Us / hz, mSensorHandler);
        if (mRotationVectorSensor != null)
            mSensorManager.registerListener(this, mRotationVectorSensor, Us / hz, mSensorHandler);
        if (mGravitySensor != null)
            mSensorManager.registerListener(this, mGravitySensor, Us / hz, mSensorHandler);
    }

    private void UnregisterListener() {
        if (mSensorManager == null) {
            return;
        }
        mSensorManager.unregisterListener(this);
    }

    private native String parserDeviceParmaPath(String supportTxt);
}

