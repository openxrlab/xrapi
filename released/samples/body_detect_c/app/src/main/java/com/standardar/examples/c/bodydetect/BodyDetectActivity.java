package com.standardar.examples.c.bodydetect;

import android.app.Activity;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class BodyDetectActivity extends Activity implements GLSurfaceView.Renderer {

    private static final String TAG ="BodyDetectActivity";
    private long mNativeApplication;

    private GLSurfaceView mSurfaceView;


    private boolean mViewportChanged = false;
    private int mViewportWidth;
    private int mViewportHeight;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.bodydetect_activity);

        mSurfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);


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
    protected void onResume() {
        super.onResume();
        if (!PermissionHelper.hasPermission(this)) {
            PermissionHelper.requestPermission(this);
            return;
        }
        JniInterface.onResume(mNativeApplication);
        mSurfaceView.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();

        mSurfaceView.onPause();

        JniInterface.onPause(mNativeApplication);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Synchronized to avoid racing onDrawFrame.
        synchronized (this) {
            JniInterface.destroyNativeApplication(mNativeApplication);
            mNativeApplication = 0;
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
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
        synchronized (this) {
            if (mViewportChanged) {
                int displayRotation = getWindowManager().getDefaultDisplay().getRotation();
                JniInterface.onDisplayGeometryChanged(mNativeApplication, displayRotation, mViewportWidth, mViewportHeight);
                mViewportChanged = false;
            }

            JniInterface.onGlSurfaceDrawFrame(mNativeApplication);


        }
    }

}
