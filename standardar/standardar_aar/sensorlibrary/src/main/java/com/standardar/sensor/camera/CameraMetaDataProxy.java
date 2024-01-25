package com.standardar.sensor.camera;

import android.util.ArrayMap;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Map;

import dalvik.system.DexClassLoader;

public class CameraMetaDataProxy {
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

	private Method mGet;
    private Object mCameraDetaData;
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
        public String getName() {
            return mName;
        }
    }

    public Object get(String keyName) {
        Key<?> key = mKeyMap.get(keyName);
        if (key == null) {
            return null;
        }
        return get(key);
    }

    public CameraMetaDataProxy(Object metaData, DexClassLoader classLoader) {
        mCameraDetaData = metaData;
        try {
            Class<?> clz = classLoader.loadClass("com.standardar.sensor.camera.CameraMetaData");
            mGet = clz.getMethod("get", String.class);
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
        mData.put(CAMERA_RGB_WIDTH, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_RGB_WIDTH.mName) ;
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_RGB_HEIGHT, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_RGB_HEIGHT.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_RGB_STRIDE, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_RGB_STRIDE.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_RGB_SCANLINE, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_RGB_SCANLINE.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_SLAM_WIDTH, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_SLAM_WIDTH.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_SLAM_HEIGHT, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_SLAM_HEIGHT.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_SLAM_STRIDE, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_SLAM_STRIDE.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_DEPTH_WIDTH, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_DEPTH_WIDTH.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_DEPTH_HEIGHT, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_DEPTH_HEIGHT.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_DEPTH_STRIDE, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_DEPTH_STRIDE.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });


        mData.put(CAMERA_TYPE, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_TYPE.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_IMAGE_BUFFER_SIZE, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_IMAGE_BUFFER_SIZE.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_FOVH, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_FOVH.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_FOVV, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_FOVV.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_SUPPORT_SIZE, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_SUPPORT_SIZE.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_CALIBRATION_BUFFER, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_CALIBRATION_BUFFER.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });
        mData.put(SCREEN_ROTATE, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, SCREEN_ROTATE.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(AXIS_UP_MODE, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, AXIS_UP_MODE.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });

        mData.put(CAMERA_SURFACETEXTURE_TIMESTAMP, new GetCommand() {
            @Override
            public <T> T getValue() {
                try {
                    return (T) mGet.invoke(mCameraDetaData, CAMERA_SURFACETEXTURE_TIMESTAMP.mName);
                } catch (IllegalAccessException e) {
                    e.printStackTrace();
                } catch (InvocationTargetException e) {
                    e.printStackTrace();
                }
                return null;
            }
        });
    }

}
