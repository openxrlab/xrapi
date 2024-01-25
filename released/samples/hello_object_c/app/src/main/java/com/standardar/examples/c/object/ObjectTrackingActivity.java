package com.standardar.examples.c.object;

import android.content.Intent;
import android.hardware.display.DisplayManager;
import android.net.Uri;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.bumptech.glide.Glide;
import com.bumptech.glide.RequestManager;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class ObjectTrackingActivity extends AppCompatActivity
        implements GLSurfaceView.Renderer, DisplayManager.DisplayListener {
    private static final String TAG = ObjectTrackingActivity.class.getSimpleName();

    private GLSurfaceView surfaceView;
    private ImageView fitToScanView;
    private RequestManager glideRequestManager;

    private boolean viewportChanged = false;
    private int viewportWidth;
    private int viewportHeight;

    // Opaque native pointer to the native application instance.
    private long nativeApplication;
    private Button mScan;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_object_tracking);
        surfaceView = (GLSurfaceView) findViewById(R.id.surfaceview);

        // Set up renderer.
        surfaceView.setPreserveEGLContextOnPause(true);
        surfaceView.setEGLContextClientVersion(2);
        surfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0); // Alpha used for plane blending.
        surfaceView.setRenderer(this);
        surfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

        ObjectTrackingJniInterface.assetManager = getAssets();
        nativeApplication = ObjectTrackingJniInterface.createNativeApplication(getAssets(), this, getPackageName(), getFilesDir().getAbsolutePath());

        fitToScanView = findViewById(R.id.image_view_fit_to_scan);
        glideRequestManager = Glide.with(this);
        glideRequestManager
                .load(Uri.parse("file:///android_asset/fit_to_scan.png"))
                .into(fitToScanView);

        mScan = findViewById(R.id.scan);
        mScan.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                Intent intent = new Intent();
                intent.setClass(ObjectTrackingActivity.this, HelloArActivity.class)
                        .setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP)
                        .setFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                        .setFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
                ObjectTrackingJniInterface.onStopAlgorithm(nativeApplication);
                startActivity(intent);
            }
        });
        mScan.setVisibility(View.INVISIBLE);
    }

    @Override
    protected void onResume() {
        super.onResume();
        // ARCore requires camera permissions to operate. If we did not yet obtain runtime
        // permission on Android M and above, now is a good time to ask the user for it.
        if (!PermissionHelper.hasPermission(this)) {
            PermissionHelper.requestPermission(this);
            return;
        }

        ObjectTrackingJniInterface.onResume(nativeApplication, getApplicationContext(), this);
        surfaceView.onResume();

        fitToScanView.setVisibility(View.VISIBLE);

        // Listen to display changed events to detect 180° rotation, which does not cause a config
        // change or view resize.
        getSystemService(DisplayManager.class).registerDisplayListener(this, null);
    }

    @Override
    public void onPause() {
        super.onPause();
        surfaceView.onPause();
        ObjectTrackingJniInterface.onPause(nativeApplication);
        getSystemService(DisplayManager.class).unregisterDisplayListener(this);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();

        // Synchronized to avoid racing onDrawFrame.
        synchronized (this) {
            ObjectTrackingJniInterface.destroyNativeApplication(nativeApplication);
            nativeApplication = 0;
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
        ObjectTrackingJniInterface.onGlSurfaceCreated(nativeApplication);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        viewportWidth = width;
        viewportHeight = height;
        viewportChanged = true;
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        // Synchronized to avoid racing onDestroy.
        synchronized (this) {
            if (nativeApplication == 0) {
                return;
            }
            if (viewportChanged) {
                int displayRotation = getWindowManager().getDefaultDisplay().getRotation();
                ObjectTrackingJniInterface.onDisplayGeometryChanged(
                        nativeApplication, displayRotation, viewportWidth, viewportHeight);
                viewportChanged = false;
            }
            ObjectTrackingJniInterface.onGlSurfaceDrawFrame(nativeApplication, this);

            if (mScan.getVisibility() == View.INVISIBLE) {
                if (!ObjectTrackingJniInterface.isSLAMInitializing(nativeApplication)) {
                    setScanVisible();
                }
            }

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
    public void onDisplayAdded(int displayId) {}

    @Override
    public void onDisplayRemoved(int displayId) {}

    @Override
    public void onDisplayChanged(int displayId) {
        viewportChanged = true;
    }

    public void hideFitToScanImage() {
        this.runOnUiThread(
                new Runnable() {
                    @Override
                    public void run() {
                        fitToScanView.setVisibility(View.INVISIBLE);
                    }
                });
    }

    private void setScanInvisible() {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mScan.setVisibility(View.INVISIBLE);
            }
        });
    }

    private void setScanVisible() {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mScan.setVisibility(View.VISIBLE);
            }
        });
    }

}
