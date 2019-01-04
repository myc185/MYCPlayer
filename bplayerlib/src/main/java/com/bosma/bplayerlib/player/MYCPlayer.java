package com.bosma.bplayerlib.player;

import android.text.TextUtils;

import com.bosma.bplayerlib.bean.MuteEnum;
import com.bosma.bplayerlib.bean.TimeInfoBean;
import com.bosma.bplayerlib.listener.OnCompleteListener;
import com.bosma.bplayerlib.listener.OnErrorListener;
import com.bosma.bplayerlib.listener.OnLoadListener;
import com.bosma.bplayerlib.listener.OnPauseResumeListener;
import com.bosma.bplayerlib.listener.OnPreparedListener;
import com.bosma.bplayerlib.listener.OnTimeInfoListener;
import com.bosma.bplayerlib.log.MyLog;
import com.bosma.bplayerlib.opengl.YCGLSurfaceView;
import com.bosma.bplayerlib.util.YCVIdeoSupporttUtil;

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

    private static String source;

    private static boolean playNext = false;

    private static int duration = -1;
    private static int volumePercent = 100;
    private static MuteEnum mute = MuteEnum.MUTE_CENTER;

    private OnPreparedListener onPreparedListener;

    private OnLoadListener onLoadListener;

    private OnPauseResumeListener onPauseResumeListener;
    private OnTimeInfoListener onTimeinfoListener;
    private OnErrorListener onErrorListener;
    private OnCompleteListener onCompleteListener;

    private static TimeInfoBean timeInfoBean;

    private YCGLSurfaceView ycglSurfaceView;


    public void setOnPreparedListener(OnPreparedListener onPreparedListener) {
        this.onPreparedListener = onPreparedListener;
    }

    public void setOnLoadListener(OnLoadListener onLoadListener) {
        this.onLoadListener = onLoadListener;
    }

    public void setOnPauseResumeListener(OnPauseResumeListener onPauseResumeListener) {
        this.onPauseResumeListener = onPauseResumeListener;
    }

    public void setOnTimeinfoListener(OnTimeInfoListener onTimeinfoListener) {
        this.onTimeinfoListener = onTimeinfoListener;
    }

    public void setOnErrorListener(OnErrorListener onErrorListener) {
        this.onErrorListener = onErrorListener;
    }

    public void setOnCompleteListener(OnCompleteListener onCompleteListener) {
        this.onCompleteListener = onCompleteListener;
    }

    public void setYcglSurfaceView(YCGLSurfaceView ycglSurfaceView) {
        this.ycglSurfaceView = ycglSurfaceView;
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
                setVolume(50);//默认音量
                setMute(mute);//默认声道
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

    public void onStop() {
        timeInfoBean = null;
        duration = -1;
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_stop();
            }
        }).start();

    }

    public void seek(int seconds) {
        n_seek(seconds);

    }

    public void playNext(String url) {
        source = url;
        playNext = true;
        onStop();
    }

    public int getDuration() {

        if (duration < 0) {
            duration = n_duration();
        }

        return duration;


    }

    public void setVolume(int percent) {
        if (percent >= 0 && percent <= 100) {
            volumePercent = percent;
            n_volume(percent);
        }
    }

    public int getVolumePercent() {
        return volumePercent;
    }

    public void setMute(MuteEnum mute) {
        this.mute = mute;
        n_mute(mute.getValue());
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

    public void onCallTimeInfo(int currentTime, int totalTime) {
        if (onTimeinfoListener != null) {
            if (timeInfoBean == null) {
                timeInfoBean = new TimeInfoBean();
            }

            timeInfoBean.setCurrentTime(currentTime);
            timeInfoBean.setTotalTime(totalTime);
            onTimeinfoListener.onTImeInfo(timeInfoBean);
        }

    }

    public void onCallError(int code, String msg) {
        n_stop();
        if (onErrorListener != null) {
            onErrorListener.onError(code, msg);
        }

    }

    public void onCallComplete() {
        onStop();
        if (onCompleteListener != null) {
            onCompleteListener.onComplete();
        }
    }

    public void onCallNext() {
        if (playNext) {
            playNext = false;
            prepared();
        }
    }

    public void onCallRenderYUV(int width, int height, byte[] y, byte[] u, byte[] v) {
        MyLog.d("获取到视频的YUV数据");
        if (ycglSurfaceView != null) {
            ycglSurfaceView.setYUVData(width, height, y, u, v);
        }
    }

    public boolean onCallIsSupportMediaCodec(String ffcodecname) {
        return YCVIdeoSupporttUtil.isSupportCodec(ffcodecname);
    }


    private native void n_prepared(String source);

    private native void n_start();

    private native void n_pause();

    private native void n_resume();

    private native void n_stop();

    private native void n_seek(int secds);

    private native int n_duration();

    private native void n_volume(int percent);

    private native void n_mute(int mute);


}
