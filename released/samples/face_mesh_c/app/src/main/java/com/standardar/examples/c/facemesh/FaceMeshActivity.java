package com.standardar.examples.c.facemesh;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class FaceMeshActivity extends Activity implements GLSurfaceView.Renderer {

    private long mNativeApplication;

    private GLSurfaceView mSurfaceView;


    private boolean mViewportChanged = false;
    private int mViewportWidth;
    private int mViewportHeight;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.facemesh_activity);

        mSurfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);


        // Set up renderer.
        mSurfaceView.setPreserveEGLContextOnPause(false);
        mSurfaceView.setEGLContextClientVersion(2);
        mSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        mSurfaceView.setRenderer(this);
        mSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

        JniInterface.assetManager = getAssets();
        mNativeApplication = JniInterface.createNativeApplication(getAssets(), this, getPackageName());


        findViewById(R.id.camera_switch).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                JniInterface.switchCamera(mNativeApplication);
            }
        });



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
                mSurfaceView.queueEvent(new Runnable() {
                    @Override
                    public void run() {
                        Bitmap bitmap = BitmapFactory.decodeResource(getResources(),
                                R.drawable.face);
                        JniInterface.setTexture(mNativeApplication,bitmap);
                        bitmap.recycle();
                    }
                });
            }

            JniInterface.onGlSurfaceDrawFrame(mNativeApplication);

        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if(keyCode == KeyEvent.KEYCODE_BACK){
            android.os.Process.killProcess(android.os.Process.myPid());
        }
        return true;
    }
}
