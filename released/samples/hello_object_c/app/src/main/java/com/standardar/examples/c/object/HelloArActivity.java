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
package com.standardar.examples.c.object;

import android.app.Service;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.hardware.display.DisplayManager;
import android.media.MediaScannerConnection;
import android.net.Uri;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Debug;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

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

    private boolean mViewportChanged = false;
    private int mViewportWidth;
    private int mViewportHeight;

    private Button mScan, mTrack, mScanGuidanceBtn;
    private boolean stateScanning, stateScanningGuidance;

    private int scanStep = 1;

    private TextView mInitText;
    private ImageView mInitImage;

    // Opaque native pointer to the native application instance.
    private long mNativeApplication;
    private ScaleGestureDetector mScaleDetector;
    private GestureDetector mGestureDetector;
    private float mPosX, mPosY, mScaleFactor;

    private Snackbar mLoadingMessageSnackbar;
    private Handler mPlaneStatusCheckingHandler;

    private static final int FRAME_COEFF = 50; //50 -> 40 FPS; 40 -> 32 FPS
    private static final int DRAW_INTERVAL = 1000 / FRAME_COEFF;

    private int mScannerProgress;
    private int mPreEvent = 0;
    private boolean mIsDoubleClick = false;
    private boolean mIsMove = false;

    private Vibrator mVibrator;

    private final Runnable mPlaneStatusCheckingRunnable =
            new Runnable() {
                @Override
                public void run() {
                    // The runnable is executed on main UI thread.
                    try {
                        if (JniInterface.hasDetectedPlanes(mNativeApplication)) {
                            if (mLoadingMessageSnackbar != null) {
                                mLoadingMessageSnackbar.dismiss();
                            }
                            mLoadingMessageSnackbar = null;
                        } else {
                            mPlaneStatusCheckingHandler.postDelayed(
                                    mPlaneStatusCheckingRunnable, SNACKBAR_UPDATE_INTERVAL_MILLIS);
                        }
                    } catch (Exception e) {
                        Log.e(TAG, e.getMessage());
                    }
                }
            };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mSurfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);

        mScaleDetector = new ScaleGestureDetector(getApplicationContext(), new SimpleScaleListenerImpl());
        mGestureDetector = new GestureDetector(getApplicationContext(), new SimpleGestureListenerImpl());

        mSurfaceView.setOnTouchListener(
                new View.OnTouchListener() {
                    @Override
                    public boolean onTouch(View v, MotionEvent event) {
                        mGestureDetector.onTouchEvent(event);
                        mScaleDetector.onTouchEvent(event);

                        switch (event.getAction()){
                            case MotionEvent.ACTION_UP:{
                                if(mPreEvent == MotionEvent.ACTION_MOVE && mIsMove == true) {
                                    JniInterface.resetTouched(mNativeApplication);
                                    mIsDoubleClick = false;
                                    mIsMove = false;
                                }
                            }
                        }
                        mPreEvent = event.getAction();
                        return true;
                    }
                });

        // Set up renderer.
        mSurfaceView.setPreserveEGLContextOnPause(true);
        mSurfaceView.setEGLContextClientVersion(2);
        mSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        mSurfaceView.setRenderer(this);
        mSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

        JniInterface.assetManager = getAssets();
        mNativeApplication = JniInterface.createNativeApplication(getAssets(), this, getPackageName());

        mPlaneStatusCheckingHandler = new Handler();

        mScan = findViewById(R.id.scan);
        mScan.setOnClickListener(this);

        mTrack = findViewById(R.id.track);
        mTrack.setOnClickListener(this);
        mTrack.setVisibility(View.INVISIBLE);

        mScanGuidanceBtn = findViewById(R.id.scan_guidance_btn);
        mScanGuidanceBtn.setOnClickListener(this);

        mInitText = (TextView) findViewById(R.id.initText);
        mInitText.bringToFront();
        mInitText.setTextColor(0xFFFF0000);

        mInitImage = (ImageView) findViewById(R.id.initImage);
        mInitImage.bringToFront();

        mVibrator = (Vibrator) this.getSystemService(Service.VIBRATOR_SERVICE);
    }

    private class SimpleScaleListenerImpl extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            mScaleFactor = detector.getScaleFactor();
            synchronized (this) {
                if (mNativeApplication == 0) {
                    return true;
                }

                if (!mIsDoubleClick)
                    JniInterface.onTouchedZoom(mNativeApplication, mScaleFactor);
            }
            return true;
        }
    }

    private class SimpleGestureListenerImpl extends GestureDetector.SimpleOnGestureListener {
        @Override
        public boolean onSingleTapConfirmed(MotionEvent e) {
            mPosX = e.getX();
            mPosY = e.getY();

            synchronized (this) {
                if (mNativeApplication == 0) {
                    return true;
                }
                JniInterface.onTouched(mNativeApplication, mPosX, mPosY);
            }
            return true;
        }

        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            mPosX -= distanceX;
            mPosY -= distanceY;

            synchronized (this) {
                if (mNativeApplication == 0) {
                    return true;
                }
                if (e2.getPointerCount() == 1) {
                    JniInterface.onTouchedMove(mNativeApplication, mPosX, mPosY);
                }
                mIsDoubleClick = false;
                mIsMove = true;
            }
            return true;
        }

        @Override
        public boolean onDoubleTap(MotionEvent e) {
            mPosX = e.getX();
            mPosY = e.getY();
            synchronized (this) {
                if (mNativeApplication == 0) {
                    return true;
                }
                JniInterface.onDoubleTouched(mNativeApplication, mPosX, mPosY);
                mIsDoubleClick = true;
            }
            return true;
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.track:{
                Intent intent = new Intent();
                intent.setClass(HelloArActivity.this, ObjectTrackingActivity.class)
                        .setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP)
                        .setFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                        .setFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
                JniInterface.onStopAlgorithm(mNativeApplication);
                startActivity(intent);
                break;
            }
            case R.id.scan:{
                if (!stateScanning) {
                    mScanGuidanceBtn.setEnabled(true);
                    mScanGuidanceBtn.setVisibility(View.VISIBLE);
                    scanStep = 1;
                    mScanGuidanceBtn.setText("Scan");
                    JniInterface.onStartScan(mNativeApplication);
                }else {
                    mScanGuidanceBtn.setVisibility(View.GONE);
                    JniInterface.onStopScan(mNativeApplication);
                    JniInterface.onStopScanGuidance(mNativeApplication);
                    JniInterface.resetAlgorithm(mNativeApplication);
                    stateScanningGuidance = false;
                }
                stateScanning = !stateScanning;
                break;
            }
            case R.id.scan_guidance_btn:{
                switch (scanStep) {
                    case 1:
                        if(JniInterface.isScanStart(mNativeApplication)) {
                            JniInterface.onStartScanGuidance(mNativeApplication);
                            //JniInterface.getScanResult(mNativeApplication);
                            stateScanningGuidance = true;
                            mScanGuidanceBtn.setText("Complete 0/45");
                            mScanGuidanceBtn.setEnabled(false);
                            scanStep++;
                        }
                        break;
                    case 2:
                        Toast.makeText(this,"Finish",Toast.LENGTH_LONG).show();
                        mScanGuidanceBtn.setEnabled(true);
                        mScanGuidanceBtn.setText("Finish");
                        JniInterface.onStopScanGuidance(mNativeApplication);
                        stateScanningGuidance = false;
                        scanStep++;
                        break;
                    case 3:
                        JniInterface.getScanResult(mNativeApplication);
                        mScan.callOnClick();
                        break;
                }
            }
        }
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
        mLoadingMessageSnackbar =
                Snackbar.make(
                        HelloArActivity.this.findViewById(android.R.id.content),
                        "Searching for surfaces...",
                        Snackbar.LENGTH_INDEFINITE);
        // Set the snackbar background to light transparent black color.
        mLoadingMessageSnackbar.getView().setBackgroundColor(0xbf323232);
        mLoadingMessageSnackbar.show();
        mPlaneStatusCheckingHandler.postDelayed(
                mPlaneStatusCheckingRunnable, SNACKBAR_UPDATE_INTERVAL_MILLIS);

        // Listen to display changed events to detect 180° rotation, which does not cause a config
        // change or view resize.
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);

        setInitTextVisible();
    }

    @Override
    public void onPause() {
        super.onPause();
        mSurfaceView.onPause();

        setInitTextInvisible();

        JniInterface.onPause(mNativeApplication);

        mPlaneStatusCheckingHandler.removeCallbacks(mPlaneStatusCheckingRunnable);

        getSystemService(DisplayManager.class).unregisterDisplayListener(this);
        stateScanning = false;
        mScanGuidanceBtn.setVisibility(View.INVISIBLE);
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

            JniInterface.onGlSurfaceDrawFrame(mNativeApplication);

            if (mInitText.getVisibility() == View.VISIBLE) {
                if (!JniInterface.isSLAMInitializing(mNativeApplication)) {
                    setInitTextInvisible();
                }
            }

            if (mTrack.getVisibility() == View.INVISIBLE) {
                if (!JniInterface.isSLAMInitializing(mNativeApplication)) {
                    setTrackTextVisible();
                }
            }


            if (stateScanning && stateScanningGuidance) {
                mScannerProgress = JniInterface.getScanProgress(mNativeApplication);
                if (mScannerProgress == 45) {
                    stateScanningGuidance = false;
                }
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (mScannerProgress == 45) {
                            mScanGuidanceBtn.callOnClick();
                        }else {
                            mScanGuidanceBtn.setText("Complete " + mScannerProgress + "/45");
                        }
                    }
                });
            }

            if(JniInterface.isNearPlane(mNativeApplication)){
                mVibrator.vibrate(100);
            }
        }
    }

    private void setInitTextInvisible() {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mInitText.setVisibility(View.INVISIBLE);
                mInitImage.setVisibility(View.INVISIBLE);
            }
        });
    }

    private void setInitTextVisible() {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mInitText.setVisibility(View.VISIBLE);
                mInitImage.setVisibility(View.INVISIBLE);
            }
        });
    }

    private void setTrackTextInvisible() {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mTrack.setVisibility(View.INVISIBLE);
            }
        });
    }

    private void setTrackTextVisible() {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mTrack.setVisibility(View.VISIBLE);
            }
        });
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
}
