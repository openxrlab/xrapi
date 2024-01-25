package com.standardar.sensor.camera;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.os.Build;
import android.os.Trace;
import android.util.Size;

import com.standardar.service.common.util.DirectByteBufferNativeUtil;
import com.standardar.service.common.util.LogUtils;

import java.lang.reflect.Constructor;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.List;

public class CameraUtils {
    static {
        System.loadLibrary("camera_lib");
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public static String getSupportPreviewSize(Context context) {
        CameraManager cameraManager = (CameraManager) context.getSystemService(Context.CAMERA_SERVICE);
        if (cameraManager == null) {
            return "";
        }
        try {
            for (String id : cameraManager.getCameraIdList()) {
                CameraCharacteristics characteristics = cameraManager.getCameraCharacteristics(id);
                Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);
                if (facing == CameraCharacteristics.LENS_FACING_BACK) {
                    List<Size> sizes = null;
                    if (cameraManager != null) {
                        StreamConfigurationMap map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                        Size[] size = map.getOutputSizes(ImageFormat.YUV_420_888);
                        if(size != null)
                            sizes =  Arrays.asList(size);
                    }
                    if (sizes == null) {
                        return "";
                    }
                    StringBuilder sb = new StringBuilder();
                    for (Size s : sizes) {
                        sb.append(s.getWidth()).append("x").append(s.getHeight()).append(",");
                    }
                    return sb.deleteCharAt(sb.length() - 1).toString();
                }
            }
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
        return "";
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    public static int getYUV420Format(Image image) {
        if ((null == image) || (image.getFormat() != ImageFormat.YUV_420_888)) {
            LogUtils.LOGE("getYUV420Type, only support YUV_420_888");
            return SImageFormat.SIMAGE_FORMAT_NON;
        }

        Image.Plane[] planes = image.getPlanes();
        if(planes[1].getPixelStride() == 1){
            return SImageFormat.SIAMGE_FORMAT_YUV_I420;
        }else if(planes[1].getPixelStride() == 2){
            return SImageFormat.SIMAGE_FORMAT_YUV_NV21;
        }else{
            return SImageFormat.SIMAGE_FORMAT_NON;
        }
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    public static int getYUV420Stride(Image image) {
        if ((null == image) || (image.getFormat() != ImageFormat.YUV_420_888)) {
            LogUtils.LOGE("getYUV420Stride, only support YUV_420_888");
            return 0;
        }

        Image.Plane[] planes = image.getPlanes();
        return planes[0].getRowStride();
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public static void getYUV420Data(Image image, ByteBuffer directBuffer) {
        if ((null == image) || (image.getFormat() != ImageFormat.YUV_420_888)) {
            LogUtils.LOGE("getYUV420Data, only support YUV_420_888");
            return;
        }
        Rect crop = image.getCropRect();
        Image.Plane[] planes = image.getPlanes();
        int format = image.getFormat();
        int rowStride = planes[0].getRowStride();
        int height = crop.height();

        ByteBuffer yBuf = planes[0].getBuffer();
        ByteBuffer uBuf = planes[1].getBuffer();
        ByteBuffer vBuf = planes[2].getBuffer();
        int buflen = rowStride * height * 3 / 2;
        if (directBuffer.capacity() < buflen) {
            LogUtils.LOGE("getYUV420Data failed outdata length:" + directBuffer.capacity() + " image len:" + buflen);
            return;
        }

        if(planes[1].getPixelStride() == 1) {
            DirectByteBufferNativeUtil.copyBuffer(directBuffer, directBuffer.position(), yBuf, yBuf.position(), yBuf.remaining());
            DirectByteBufferNativeUtil.copyBuffer(directBuffer, yBuf.remaining(), uBuf, uBuf.position(), uBuf.remaining());
            DirectByteBufferNativeUtil.copyBuffer(directBuffer, yBuf.remaining() + uBuf.remaining(), vBuf, vBuf.position(), vBuf.remaining());
        }else{
            directBuffer.rewind();
            byte[] yBufArr =new byte[yBuf.remaining()];
            yBuf.get(yBufArr,yBuf.position(),yBuf.remaining());
            directBuffer.put(yBufArr);

            byte[] vBufArr =new byte[vBuf.remaining()];
            vBuf.get(vBufArr,vBuf.position(),vBuf.remaining());
            directBuffer.put(vBufArr);

        }
    }

    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public static void getYUV420DataWithoutStride(ByteBuffer srcBuffer, byte[] directBuffer, int width, int height, int stride) {
        if(width == stride){
            srcBuffer.get(directBuffer, 0, width * height * 3 / 2);
        }else{
            int idx = 0;
            int offset = 0; // the offset of output byte array

            // get data for Y plane (plane[0])
            for (idx = 0, offset = 0; idx < height; idx++, offset += width) {
                srcBuffer.get(directBuffer, offset, width);

                if (idx != (height - 1)) {
                    srcBuffer.position(srcBuffer.position() + (stride - width));
                }
            }

            // get data for V (Cr) plane (plane[2])
            for (idx = 0; idx < (height / 2); idx++, offset += width) {
                if (idx != (height / 2 - 1)) {
                    srcBuffer.get(directBuffer, offset, width);
                    srcBuffer.position(srcBuffer.position() + (stride - width));
                } else {
                    srcBuffer.get(directBuffer, offset, width - 1);
                }
            }
        }
    }

    /**
     * convert yuv420 to nv21 in an already allocated buffer
     * @param image
     * @param outData the nv21 data
     * @return
     */
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public static void YUV420toNV21(Image image, byte[] outData) {
//        Util.timeStart();
        if ((null == image) || (image.getFormat() != ImageFormat.YUV_420_888)) {
            LogUtils.LOGE("yuv420ToNv21, only support YUV_420_888");
            return;
        }
        if (outData == null) {
            return;
        }

        int imgWidth = image.getWidth();
        int imgHeight = image.getHeight();

        Rect crop = image.getCropRect();
        int format = image.getFormat();
        int width = crop.width();
        int height = crop.height();
        Image.Plane[] planes = image.getPlanes();

        int buflen = width * height * ImageFormat.getBitsPerPixel(format) / 8;
        if (outData.length != buflen) {
            LogUtils.LOGE("yuv420tonv21 failed outdata length:" + outData.length + " image len:" + buflen);
            return;
        }

        ByteBuffer yBuf = planes[0].getBuffer();
        ByteBuffer uvBuf = planes[2].getBuffer();

        if (image.getPlanes()[0].getRowStride() == image.getWidth()) {
            /**
             * for case of preview size 1280x960, the row stride is 1280
             *
             * the size of yBuf is 1228800 (1280 x 960)
             * the size of uvBuf is 614399 (1280 x 480 - 1)
             */
            yBuf.get(outData, 0, yBuf.remaining());
            uvBuf.get(outData, yBuf.position(), uvBuf.remaining());
        } else {
            /**
             * for case of preview size 1440x1080, the row stride is 1472
             *
             * the size of yBuf is 1589728 (1472 x 1079 + 1440)
             * the size of uvBuf is 794847 (1472 x 539 + 1440 - 1)
             */
            int idx = 0;
            int offset = 0; // the offset of output byte array
            int rowStride = image.getPlanes()[0].getRowStride();

            // get data for Y plane (plane[0])
            for (idx = 0, offset = 0; idx < imgHeight; idx++, offset += imgWidth) {
                yBuf.get(outData, offset, imgWidth);

                if (idx != (imgHeight - 1)) {
                    yBuf.position(yBuf.position() + (rowStride - imgWidth));
                }
            }

            // get data for V (Cr) plane (plane[2])
            for (idx = 0; idx < (imgHeight / 2); idx++, offset += imgWidth) {
                if (idx != (imgHeight / 2 - 1)) {
                    uvBuf.get(outData, offset, imgWidth);
                    uvBuf.position(uvBuf.position() + (rowStride - imgWidth));
                } else {
                    uvBuf.get(outData, offset, imgWidth - 1);
                }
            }
        }
//        Util.timeEnd("yuv420tonv21");
    }

    /**
     * 在c层拷贝Image像素
     * @param image
     * @param directBuffer
     */
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public static void YUV420toNV21(Image image, ByteBuffer directBuffer) {
        if ((null == image) || (image.getFormat() != ImageFormat.YUV_420_888)) {
            LogUtils.LOGE("yuv420ToNv21, only support YUV_420_888");
            return;
        }

        int imgWidth = image.getWidth();
        int imgHeight = image.getHeight();

        Rect crop = image.getCropRect();
        int format = image.getFormat();
        int width = crop.width();
        int height = crop.height();
        Image.Plane[] planes = image.getPlanes();

        int buflen = width * height * ImageFormat.getBitsPerPixel(format) / 8;
        if (directBuffer.capacity() < buflen) {
            LogUtils.LOGE("yuv420tonv21 failed direct buffer length:" + directBuffer.capacity() + " image len:" + buflen);
            return;
        }

        ByteBuffer yBuf = planes[0].getBuffer();
        ByteBuffer uvBuf = planes[2].getBuffer();

        if (image.getPlanes()[0].getRowStride() == image.getWidth()) {
            /**
             * for case of preview size 1280x960, the row stride is 1280
             *
             * the size of yBuf is 1228800 (1280 x 960)
             * the size of uvBuf is 614399 (1280 x 480 - 1)
             */
//            yBuf.get(outData, 0, yBuf.remaining());
//            uvBuf.get(outData, yBuf.position(), uvBuf.remaining());
            DirectByteBufferNativeUtil.copyBuffer(directBuffer, directBuffer.position(), yBuf, yBuf.position(), yBuf.remaining());
            DirectByteBufferNativeUtil.copyBuffer(directBuffer, yBuf.remaining(), uvBuf, uvBuf.position(), uvBuf.remaining());
        } else {
            /**
             * for case of preview size 1440x1080, the row stride is 1472
             *
             * the size of yBuf is 1589728 (1472 x 1079 + 1440)
             * the size of uvBuf is 794847 (1472 x 539 + 1440 - 1)
             */
            int idx = 0;
            int offset = 0; // the offset of output byte array
            int rowStride = image.getPlanes()[0].getRowStride();

            // get data for Y plane (plane[0])
            for (idx = 0, offset = 0; idx < imgHeight; idx++, offset += imgWidth) {
//                yBuf.get(outData, offset, imgWidth);
                DirectByteBufferNativeUtil.copyBuffer(directBuffer, offset, yBuf, yBuf.position(), imgWidth);
                yBuf.position(yBuf.position() + imgWidth);
                if (idx != (imgHeight - 1)) {
                    yBuf.position(yBuf.position() + (rowStride - imgWidth));
                }
            }

            // get data for V (Cr) plane (plane[2])
            for (idx = 0; idx < (imgHeight / 2); idx++, offset += imgWidth) {
                if (idx != (imgHeight / 2 - 1)) {
//                    uvBuf.get(outData, offset, imgWidth);
                    DirectByteBufferNativeUtil.copyBuffer(directBuffer, offset, uvBuf, uvBuf.position(), imgWidth);
                    uvBuf.position(uvBuf.position() + imgWidth);
                    uvBuf.position(uvBuf.position() + (rowStride - imgWidth));
                } else {
//                    uvBuf.get(outData, offset, imgWidth - 1);
                    DirectByteBufferNativeUtil.copyBuffer(directBuffer, offset, uvBuf, uvBuf.position(), imgWidth - 1);
                    uvBuf.position(uvBuf.position() + imgWidth - 1);
                }
            }
        }
    }

    /**
     * get image gray channel pixel.
     * @param image
     * @return gray channel pixel byte array
     */
    @TargetApi(Build.VERSION_CODES.LOLLIPOP)
    public static void getImageGrayByte(Image image, byte[] outData) {
        Rect crop = image.getCropRect();
        int width = crop.width();
        int height = crop.height();
        Image.Plane[] planes = image.getPlanes();

        int channelOffset = 0;

        ByteBuffer buffer = planes[0].getBuffer();
        int rowStride = planes[0].getRowStride();
        int pixelStride = planes[0].getPixelStride();
        buffer.position(rowStride * crop.top + pixelStride * crop.left);
        for (int row = 0; row < height; row++) {
            int length = width;
            buffer.get(outData, channelOffset, length);
            channelOffset += length;
            if (row < height - 1) {
                buffer.position(buffer.position() + rowStride - length);
            }
        }
    }


    public static native void cropImageNv12(ByteBuffer buf, int width, int height, int stride, int scanline, byte[] out);
    public static native void cropImageNv12Buffer(ByteBuffer buf, int width, int height, int stride, int scanline, int tofw, int tofh, ByteBuffer out_rgb, ByteBuffer out_tof);
    public static native void copyImageBuffer(ByteBuffer buf, int width, int height, int stride, int format, long timestamp);
    public static native void updateOESTexture(int textureid);
    public static native void releaseImageBuffer();
    public static native long getCurImageTimestamp();
    public static native int getCurImageWidth();
    public static native int getCurImageHeight();
    public static native int getCurImageStride();
    public static native int getCurImageFormat();
    public static native boolean getCurImageData(long y_ptr, long u_ptr, long v_ptr);
    public static native boolean isImageBufferDirty();
}
