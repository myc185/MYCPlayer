package com.bosma.bplayerlib.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class YCGLSurfaceView extends GLSurfaceView {


    private YCRender mRender;

    public YCGLSurfaceView(Context context) {
        super(context);
    }

    public YCGLSurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setEGLContextClientVersion(2);
        mRender = new YCRender(context);
        setRenderer(mRender);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

    }

    public void setYUVData(int width, int height, byte[] y, byte[] u, byte[] v) {
        if (mRender != null) {
            mRender.setYUVRenderData(width, height, y, u, v);
            requestRender();
        }

    }
}
