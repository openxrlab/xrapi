package com.standardar.examples.c.handgesture;

import android.graphics.Rect;
import android.util.Log;
import android.view.Surface;

import com.standardar.common.Vector3f;

/**
 * 旋转FaceInfo的工具类, 包括旋转Face本身以及Face矩形框等
 **/
public class HandRotationUtil {

    /**
     * rotate face rect
     *
     * @param rect          used to rotate
     * @param width         The width of image
     * @param height        The height of image
     * @param isFrontCamera The isFrontCamera whether the image come from front camera
     * @param degrees       The degrees of camera display
     */
    public static Rect rotateFaceRect(Rect rect, int width, int height, boolean isFrontCamera, int degrees) {
        if (rect == null) {
            return null;
        }
        int tmp;
        switch (degrees) {
            case 0: // landscape
                // 前置摄像头时左右翻转
                if (isFrontCamera) {
                    rect.left = width - rect.left;
                    rect.right = width - rect.right;
                }
                break;
            case 90: // portrait
                Log.d("ori", "run 90");
                // 顺时针旋转90°
                tmp = rect.left;
                rect.left = height - rect.bottom;
                rect.bottom = rect.right;
                rect.right = height - rect.top;
                rect.top = tmp;
                // 前置摄像头时上下翻转
                if (isFrontCamera) {
                    tmp = rect.top;
                    rect.top = width - rect.bottom;
                    rect.bottom = width - tmp;
                }
                break;
            case 180:
                // 上下翻转
                rect.top = height - rect.top;
                rect.bottom = height - rect.bottom;
                // 后置摄像头时再左右翻转
                if (!isFrontCamera) {
                    rect.left = width - rect.left;
                    rect.right = width - rect.right;
                }
                break;
            case 270:
                // 顺时针旋转90°
                tmp = rect.left;
                rect.left = height - rect.bottom;
                rect.bottom = rect.right;
                rect.right = height - rect.top;
                rect.top = tmp;
                // 左右翻转
                tmp = rect.left;
                rect.left = height - rect.right;
                rect.right = height - tmp;
                // 后置摄像头时再上下翻转
                if (!isFrontCamera) {
                    tmp = rect.top;
                    rect.top = width - rect.bottom;
                    rect.bottom = width - tmp;
                }
                break;
            default:
                break;
        }
        return rect;
    }

    /**
     * rotate face key points
     *
     * @param point         used to rotate
     * @param width         The width of image
     * @param height        The height of image
     * @param isFrontCamera The isFrontCamera whether the image come from front camera
     * @param degrees       The degrees of camera display
     */
    public static Vector3f rotatePoints(Vector3f point, int width, int height, boolean isFrontCamera, int degrees) {
        float tmp;
        switch (degrees) {
            case 0: // landscape
                if (isFrontCamera) {
                    point.x = width - point.x;
                }
                break;
            case 90: // portrait
                tmp = point.x;
                point.x = height - point.y;
                point.y = tmp;
                if (isFrontCamera) {
                    point.y = width - point.y;
                }
                break;
            case 180:
                point.y = height - point.y;
                if (!isFrontCamera) {
                    point.x = width - point.x;
                }
                break;
            case 270:
                tmp = point.y;
                point.y = point.x;
                point.x = tmp;
                if (!isFrontCamera) {
                    point.y = width - point.y;
                }
                break;
            default:
                break;
        }
        return point;
    }




    public static int getCameraOri(int rotation) {
        int degrees = rotation * 90;
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 90;
                break;
            case Surface.ROTATION_90:
                degrees = 0;
                break;
            case Surface.ROTATION_180:
                degrees = 270;
                break;
            case Surface.ROTATION_270:
                degrees = 180;
                break;
            default:
                break;
        }

        return degrees;
    }




}
