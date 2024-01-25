/*
 * Copyright 2017 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* modification list:
 * 1. add UI components
 * 2. add state member to control whether show the UI compoents
 * 3. add onClick
 * 4. add savePicture
 * 5. add saveToSDCard
 * 6. add setInitTextInvisible
 * 7. add setDebugText
 */
package com.standardar.examples.c.handgesture;

import android.content.Context;
import android.content.res.Configuration;
import android.graphics.PixelFormat;
import android.hardware.display.DisplayManager;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;


import com.standardar.common.Vector3f;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * This is a simple example that shows how to create an augmented reality (AR) application using the
 * ARCore C API.
 */
public class HelloArActivity extends AppCompatActivity
        implements GLSurfaceView.Renderer, DisplayManager.DisplayListener, View.OnClickListener {
    private static final String TAG = HelloArActivity.class.getSimpleName();
    private static final int SNACKBAR_UPDATE_INTERVAL_MILLIS = 1000; // In milliseconds.

    private GLSurfaceView mSurfaceView;
    private TextView mHandGestureTv;

    private boolean mViewportChanged = false;
    private int mViewportWidth;
    private int mViewportHeight;



    // Opaque native pointer to the native application instance.
    private long mNativeApplication;

    private static final int FRAME_COEFF = 50; //50 -> 40 FPS; 40 -> 32 FPS
    private static final int DRAW_INTERVAL = 1000 / FRAME_COEFF;

    private SurfaceView mOverlap;
    private DrawHelper mDrawHelper;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);
        mSurfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);
        mHandGestureTv = findViewById(R.id.handgesture_tv);

        mOverlap = findViewById(R.id.overlap);
        mOverlap.setZOrderOnTop(true);
        mOverlap.getHolder().setFormat(PixelFormat.TRANSLUCENT);

        mDrawHelper = new DrawHelper();


        // Set up renderer.
        mSurfaceView.setPreserveEGLContextOnPause(true);
        mSurfaceView.setEGLContextClientVersion(2);
        mSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        mSurfaceView.setRenderer(this);
        mSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

        JniInterface.assetManager = getAssets();
        mNativeApplication = JniInterface.createNativeApplication(getAssets(), this, getPackageName());
    }

    @Override
    public void onClick(View v) {

    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!PermissionHelper.hasPermission(this)) {
            PermissionHelper.requestPermission(this);
            return;
        }
        JniInterface.onResume(mNativeApplication);
        mSurfaceView.onResume();

        // Listen to display changed events to detect 180° rotation, which does not cause a config
        // change or view resize.
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);


    }

    @Override
    public void onPause() {
        super.onPause();
        mSurfaceView.onPause();



        JniInterface.onPause(mNativeApplication);

        getSystemService(DisplayManager.class).unregisterDisplayListener(this);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        // Synchronized to avoid racing onDrawFrame.
        synchronized (this) {
            JniInterface.destroyNativeApplication(mNativeApplication);
            mNativeApplication = 0;
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            // Standard Android full-screen functionality.
            getWindow()
                    .getDecorView()
                    .setSystemUiVisibility(
                            View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                                    | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        GLES20.glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        JniInterface.onGlSurfaceCreated(mNativeApplication);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mViewportWidth = width;
        mViewportHeight = height;
        mViewportChanged = true;
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        // Synchronized to avoid racing onDestroy.
        synchronized (this) {
            if (mNativeApplication == 0) {
                return;
            }

            if (mViewportChanged) {
                int displayRotation = getWindowManager().getDefaultDisplay().getRotation();
                JniInterface.onDisplayGeometryChanged(mNativeApplication, displayRotation, mViewportWidth, mViewportHeight);
                mViewportChanged = false;
            }
            long start = System.currentTimeMillis();

            JniInterface.onGlSurfaceDrawFrame(mNativeApplication);
            Log.i("debugtime", "[client][glthread] onGlSurfaceDrawFrame=" + (System.currentTimeMillis()-start));
        }
    }


    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] results) {
        if (!PermissionHelper.hasPermission(this)) {
            Toast.makeText(this, "Camera permission is needed to run this application", Toast.LENGTH_LONG)
                    .show();
            if (!PermissionHelper.shouldShowRequestPermissionRationale(this)) {
                // Permission denied with checking "Do not ask again".
                PermissionHelper.launchPermissionSettings(this);
            }
            finish();
        }
    }

    // DisplayListener methods
    @Override
    public void onDisplayAdded(int displayId) {
    }

    @Override
    public void onDisplayRemoved(int displayId) {
    }

    @Override
    public void onDisplayChanged(int displayId) {
        mViewportChanged = true;
    }


    public  void  updateHandGestureInfo(final String info,float[] floatsPoint,int pointCount,int previewWidth,int previewHeight,int viewportWidth,int viewportHeight,int screenHeight){

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if(info !=null){
                    mHandGestureTv.setText(info);
                }else {
                    mHandGestureTv.setText("");
                }

            }
        });

        if(pointCount>0) {

            Vector3f[] points2d = new Vector3f[pointCount];

            for (int i = 0; i < pointCount; i++) {
                points2d[i] = new Vector3f();
                points2d[i].x = floatsPoint[2 * i];
                points2d[i].y = floatsPoint[2 * i + 1];
            }

            int angle = ((WindowManager)getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay().getRotation();
           int cameraRoatation = HandRotationUtil.getCameraOri(angle);

           Log.i("zwtest","====angle=="+angle+"=====cameraRoatation="+cameraRoatation);


           if(getResources().getConfiguration().orientation ==  Configuration.ORIENTATION_LANDSCAPE){
               mDrawHelper.drawHands(points2d, mOverlap, viewportWidth, viewportHeight, screenHeight,previewHeight , previewWidth,cameraRoatation);

           }else {
               mDrawHelper.drawHands(points2d, mOverlap, viewportWidth, viewportHeight, screenHeight, previewWidth, previewHeight,cameraRoatation);

           }



        }else {
            mDrawHelper.drawHands(null, mOverlap, viewportWidth, viewportHeight, screenHeight, previewWidth, previewHeight,0);

        }

    }

}
