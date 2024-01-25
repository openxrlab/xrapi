package com.standardar.sensor.camera;

import android.util.ArrayMap;

import java.util.Map;

public class CameraMetaData {
    public static final Key<Integer> CAMERA_RGB_WIDTH = new Key<Integer>("camera.rgb.width", Integer.class);
    public static final Key<Integer> CAMERA_RGB_HEIGHT = new Key<Integer>("camera.rgb.height", Integer.class);
    public static final Key<Integer> CAMERA_RGB_STRIDE= new Key<Integer>("camera.rgb.stride", Integer.class);
    public static final Key<Integer> CAMERA_RGB_SCANLINE= new Key<Integer>("camera.rgb.scanline", Integer.class);
    public static final Key<Integer> CAMERA_SLAM_WIDTH = new Key<Integer>("camera.slam.width", Integer.class);
    public static final Key<Integer> CAMERA_SLAM_HEIGHT = new Key<Integer>("camera.slam.height", Integer.class);
    public static final Key<Integer> CAMERA_SLAM_STRIDE= new Key<Integer>("camera.slam.stride", Integer.class);
    public static final Key<Integer> CAMERA_DEPTH_WIDTH = new Key<Integer>("camera.depth.width", Integer.class);
    public static final Key<Integer> CAMERA_DEPTH_HEIGHT = new Key<Integer>("camera.depth.height", Integer.class);
    public static final Key<Integer> CAMERA_DEPTH_STRIDE = new Key<Integer>("camera.depth.stride", Integer.class);
    public static final Key<Integer> CAMERA_TYPE = new Key<Integer>("camera.type", Integer.class);
    public static final Key<Integer> CAMERA_IMAGE_BUFFER_SIZE = new Key<Integer>("camera.image.buffer.size", Integer.class);
    public static final Key<byte[]> CAMERA_CALIBRATION_BUFFER = new Key<byte[]>("camera.calibration.buffer", byte[].class);
    public static final Key<Float> CAMERA_FOVH = new Key<Float>("camera.fovh", Float.class);
    public static final Key<Float> CAMERA_FOVV = new Key<Float>("camera.fovv", Float.class);
    public static final Key<String> CAMERA_SUPPORT_SIZE = new Key<String>("camera.support.size", String.class);
    public static final Key<Integer> SCREEN_ROTATE = new Key<Integer>("screen.rotate", Integer.class);
    public static final Key<Integer> AXIS_UP_MODE = new Key<Integer>("axisup", Integer.class);
    public static final Key<Long> CAMERA_SURFACETEXTURE_TIMESTAMP = new Key<Long>("camera.surfacetexture.timestamp", Long.class);


    public interface GetCommand {
        public <T> T getValue();
    }
    protected Map<Key<?>, GetCommand> mData = new ArrayMap<>();
    public Map<String, Key<?>> mKeyMap = new ArrayMap<>();

    public  <T> T get(Key<T> key) {
        GetCommand cmd = mData.get(key);
        if (cmd != null) {
            return (T) cmd.getValue();
        } else {
            return null;
        }
    }

    private static class Key<T> {
        private String mName;
        private Class<T> mType;
        public Key(String name, Class<T> type) {
            mName = name;
            mType = type;
        }
    }

    public Object get(String keyName) {
        Key<?> key = mKeyMap.get(keyName);
        if (key == null) {
            return null;
        }
        return get(key);
    }

    public CameraMetaData() {
        mKeyMap.put("camera.rgb.width",CAMERA_RGB_WIDTH);
        mKeyMap.put("camera.rgb.height",CAMERA_RGB_HEIGHT);
        mKeyMap.put("camera.rgb.stride",CAMERA_RGB_STRIDE);
        mKeyMap.put("camera.rgb.scanline", CAMERA_RGB_SCANLINE);
        mKeyMap.put("camera.slam.width", CAMERA_SLAM_WIDTH);
        mKeyMap.put("camera.slam.height", CAMERA_SLAM_HEIGHT);
        mKeyMap.put("camera.slam.stride", CAMERA_SLAM_STRIDE);
        mKeyMap.put("camera.depth.width", CAMERA_DEPTH_WIDTH);
        mKeyMap.put("camera.depth.height", CAMERA_DEPTH_HEIGHT);
        mKeyMap.put("camera.depth.stride", CAMERA_DEPTH_STRIDE);
        mKeyMap.put("camera.type", CAMERA_TYPE);
        mKeyMap.put("camera.image.buffer.size", CAMERA_IMAGE_BUFFER_SIZE);
        mKeyMap.put("camera.calibration.buffer", CAMERA_CALIBRATION_BUFFER);
        mKeyMap.put("camera.fovh", CAMERA_FOVH);
        mKeyMap.put("camera.fovv", CAMERA_FOVV);
        mKeyMap.put("camera.support.size", CAMERA_SUPPORT_SIZE);
        mKeyMap.put("screen.rotate", SCREEN_ROTATE);
        mKeyMap.put("axisup", AXIS_UP_MODE);
        mKeyMap.put("camera.surfacetexture.timestamp", CAMERA_SURFACETEXTURE_TIMESTAMP);
    }
}
