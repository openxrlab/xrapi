package com.standardar.service.facedetectlibrary;

import android.graphics.Rect;

public class FaceInfo {

    public FaceInfo() {
    }

    public int id;

    public float  score;
    public int    detect_width;
    public int    detect_height;
    public int    correct_width;
    public int    correct_height;
    public int    landmarks_points_count;
    public long   landmarks_points_array;
    public int    extra_landmarks_points_count;
    public long   extra_landmarks_points_array;
    public float  yaw;
    public float  pitch;
    public float  roll;
    public Rect   face_rect;
    public int rotation;
    public boolean isFrontCamera;



    @Override
    public String toString() {
        return "faceInfo[id ="+id+" landmarks_points_count="+landmarks_points_count+"landmarks_points_array="+landmarks_points_array+" extra_landmarks_points_count:"+
                extra_landmarks_points_count+"correct_width="+correct_width+" correct_height = "+correct_height+"]";
    }
}
