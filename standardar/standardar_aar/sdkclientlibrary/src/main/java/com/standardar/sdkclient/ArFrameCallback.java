package com.standardar.sdkclient;

import android.app.Activity;
import android.view.Choreographer;

public class ArFrameCallback implements android.view.Choreographer.FrameCallback
{

    public ArFrameCallback()
    {
        //choreographerInstance = Choreographer.getInstance();
    }

    public static ArFrameCallback handler = new ArFrameCallback();
    public static Choreographer choreographerInstance;

    public static void startVsync( Activity act )
    {
        act.runOnUiThread( new Thread()
        {
            @Override
            public void run()
            {
                choreographerInstance = Choreographer.getInstance();
                choreographerInstance.removeFrameCallback(handler);
                choreographerInstance.postFrameCallback(handler);
            }
        });
    }

    public static void stopVsync( Activity act )
    {
        act.runOnUiThread( new Thread()
        {
            @Override
            public void run()
            {
                if (choreographerInstance != null) {
                    choreographerInstance.removeFrameCallback(handler);
                }
            }
        });
    }



    @Override
    public void doFrame(long frameTimeNanos) {
        nativeVsync(frameTimeNanos);
        Choreographer.getInstance().postFrameCallback(this);
    }

    public static native void nativeVsync(long lastVsyncNano);
}
