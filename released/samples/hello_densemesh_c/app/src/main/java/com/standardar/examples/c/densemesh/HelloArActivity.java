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
package com.standardar.examples.c.densemesh;

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
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
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
    private static final int COMMAND_SHOW_FEATURE = 0;
    private static final int COMMAND_SHOW_LANDMARK = 1;
    private static final int COMMAND_SHOW_AXIS = 2;
    private static final int COMMAND_SHOW_PLANE = 3;
    private GLSurfaceView mSurfaceView;

    private boolean mViewportChanged = false;
    private int mViewportWidth;
    private int mViewportHeight;

    private Button mPlane, mCloud, mFeature, mCoordinate, mStart, mStop;
    private boolean statePlane, stateCloud, stateFeature, stateCoordinate;
    private boolean stateFinishSavePic = true;

    private byte[] pixels;

    private TextView mDebugTextView, mInitText;
    private ImageView mInitImage;

    // Opaque native pointer to the native application instance.
    private long mNativeApplication;
    private GestureDetector mGestureDetector;

    private Snackbar mLoadingMessageSnackbar;
    private Handler mPlaneStatusCheckingHandler;

    private static final int FRAME_COEFF = 50; //50 -> 40 FPS; 40 -> 32 FPS
    private static final int DRAW_INTERVAL = 1000 / FRAME_COEFF;

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

        // Set up tap listener.
        mGestureDetector =
                new GestureDetector(
                        this,
                        new GestureDetector.SimpleOnGestureListener() {
                            @Override
                            public boolean onSingleTapUp(final MotionEvent e) {
                                mSurfaceView.queueEvent(
                                        new Runnable() {
                                            @Override
                                            public void run() {
                                                JniInterface.onTouched(mNativeApplication, e.getX
                                                        (), e.getY());
                                            }
                                        });
                                return true;
                            }

                            @Override
                            public boolean onDown(MotionEvent e) {
                                return true;
                            }

                            @Override
                            public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
                                return true;
                            }
                        });

        mSurfaceView.setOnTouchListener(
                new View.OnTouchListener() {
                    @Override
                    public boolean onTouch(View v, MotionEvent event) {
                        return mGestureDetector.onTouchEvent(event);
                    }
                });

        // Set up renderer.
        mSurfaceView.setPreserveEGLContextOnPause(true);
        mSurfaceView.setEGLContextClientVersion(2);
        mSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        mSurfaceView.setRenderer(this);
        mSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

        mPlaneStatusCheckingHandler = new Handler();

        mPlane = (Button) findViewById(R.id.plane);
        mPlane.bringToFront();
        mPlane.setOnClickListener(this);

        mCloud = (Button) findViewById(R.id.cloud);
        mCloud.bringToFront();
        mCloud.setOnClickListener(this);

        mFeature = (Button) findViewById(R.id.feature);
        mFeature.bringToFront();
        mFeature.setOnClickListener(this);

        mCoordinate = (Button) findViewById(R.id.coordinate);
        mCoordinate.bringToFront();
        mCoordinate.setOnClickListener(this);

        mStart = (Button) findViewById(R.id.start);
        mStart.bringToFront();
        mStart.setOnClickListener(this);

        mStop = (Button) findViewById(R.id.stop);
        mStop.bringToFront();
        mStop.setOnClickListener(this);

        stateCloud = true;
        statePlane = true;
        stateFeature = true;
        stateCoordinate = true;

        mDebugTextView = (TextView) findViewById(R.id.textView);
        mDebugTextView.bringToFront();
        mDebugTextView.setLines(30);
        mDebugTextView.setWidth(800);
        mDebugTextView.setTextColor(0xFFFF0000);

        mInitText = (TextView) findViewById(R.id.initText);
        mInitText.bringToFront();
        mInitText.setTextColor(0xFFFF0000);

        mInitImage = (ImageView) findViewById(R.id.initImage);
        mInitImage.bringToFront();

        JniInterface.assetManager = getAssets();

        mNativeApplication = JniInterface.createNativeApplication(getAssets(), this, getPackageName());
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.plane: {
                if (statePlane == false) {
                    statePlane = true;
                } else {
                    statePlane = false;
                }
                JniInterface.onDebugCommandInt(mNativeApplication, COMMAND_SHOW_PLANE, statePlane
                        ? 1 : 0);
                break;
            }
            case R.id.cloud: {
                if (stateCloud == false) {
                    stateCloud = true;
                } else {
                    stateCloud = false;
                }
                JniInterface.onDebugCommandInt(mNativeApplication, COMMAND_SHOW_LANDMARK,
                        stateCloud ? 1 : 0);
                break;
            }
            case R.id.feature: {
                if (stateFeature == false) {
                    stateFeature = true;
                } else {
                    stateFeature = false;
                }
                JniInterface.onDebugCommandInt(mNativeApplication, COMMAND_SHOW_FEATURE,
                        stateFeature ? 1 : 0);
                break;
            }
            case R.id.coordinate: {
                if (stateCoordinate == false) {
                    stateCoordinate = true;
                } else {
                    stateCoordinate = false;
                }
                JniInterface.onDebugCommandInt(mNativeApplication, COMMAND_SHOW_AXIS,
                        stateCoordinate ? 1 : 0);
                break;
            }
            case R.id.start:{
                JniInterface.onStartAlgorithm(mNativeApplication);
                break;
            }
            case R.id.stop:{
                JniInterface.onStopAlgorithm(mNativeApplication);
                break;
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
            long startTime = System.currentTimeMillis();

            if (mViewportChanged) {
                int displayRotation = getWindowManager().getDefaultDisplay().getRotation();
                JniInterface.onDisplayGeometryChanged(mNativeApplication, displayRotation, mViewportWidth, mViewportHeight);
                mViewportChanged = false;
            }

            JniInterface.onGlSurfaceDrawFrame(mNativeApplication);

            final String texinfo = JniInterface.getSLAMInfo(mNativeApplication);
            this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    SetDebugText(texinfo);
                }
            });
            if (mInitText.getVisibility() == View.VISIBLE) {
                if (!JniInterface.isSLAMInitializing(mNativeApplication)) {
                    setInitTextInvisible();
                }
            }

            long endTime = System.currentTimeMillis();
            long costTime = endTime - startTime;
            if (costTime < DRAW_INTERVAL) {
                long sleepTime = DRAW_INTERVAL - costTime;
                try {
                    Thread.sleep(sleepTime);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
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

    public void SetDebugText(String debugtext) {
        mDebugTextView.setText(debugtext);
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
