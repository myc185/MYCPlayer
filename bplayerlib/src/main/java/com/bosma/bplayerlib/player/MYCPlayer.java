package com.bosma.bplayerlib.player;

import android.text.TextUtils;

import com.bosma.bplayerlib.listener.OnLoadListener;
import com.bosma.bplayerlib.listener.OnPauseResumeListener;
import com.bosma.bplayerlib.listener.OnPreparedListener;
import com.bosma.bplayerlib.log.MyLog;

/**
 * moyc
 */
public class MYCPlayer {

    // Used to load the 'native-lib' library on application startup.
    static {

        System.loadLibrary("native-lib");

        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
        System.loadLibrary("postproc-54");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avdevice-57");

    }

    private String source;

    private OnPreparedListener onPreparedListener;

    private OnLoadListener onLoadListener;

    private OnPauseResumeListener onPauseResumeListener;


    public void setOnPreparedListener(OnPreparedListener onPreparedListener) {
        this.onPreparedListener = onPreparedListener;
    }

    public void setOnLoadListener(OnLoadListener onLoadListener) {
        this.onLoadListener = onLoadListener;
    }

    public void setOnPauseResumeListener(OnPauseResumeListener onPauseResumeListener) {
        this.onPauseResumeListener = onPauseResumeListener;
    }

    public MYCPlayer() {

    }

    public void setSource(String source) {
        this.source = source;
    }

    public void prepared() {
        if (TextUtils.isEmpty(source)) {
            MyLog.d("source is empty");
            return;
        }

        onCallLoad(true);

        new Thread(new Runnable() {
            @Override
            public void run() {
                n_prepared(source);
            }
        }).start();

    }

    public void start() {
        if (TextUtils.isEmpty(source)) {
            MyLog.d("source is empty");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_start();
            }
        }).start();

    }

    public void onPause() {
        n_pause();
        if (onPauseResumeListener != null) {
            onPauseResumeListener.onPuase(true);
        }
    }

    public void onResume() {
        n_resume();
        if (onPauseResumeListener != null) {
            onPauseResumeListener.onPuase(false);
        }
    }

    public void onCallLoad(boolean load) {
        if (onLoadListener != null) {
            onLoadListener.onLoad(load);
        }
    }


    public void onCallPrepared() {
        if (onPreparedListener != null) {
            onPreparedListener.onPrepared();
        }
    }

    private native void n_prepared(String source);

    private native void n_start();

    private native void n_pause();

    private native void n_resume();


}
