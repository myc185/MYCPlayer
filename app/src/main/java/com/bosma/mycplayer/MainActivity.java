package com.bosma.mycplayer;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.bosma.bplayerlib.Demo;
import com.bosma.bplayerlib.bean.TimeInfoBean;
import com.bosma.bplayerlib.listener.OnErrorListener;
import com.bosma.bplayerlib.listener.OnLoadListener;
import com.bosma.bplayerlib.listener.OnPauseResumeListener;
import com.bosma.bplayerlib.listener.OnPreparedListener;
import com.bosma.bplayerlib.listener.OnTimeInfoListener;
import com.bosma.bplayerlib.log.MyLog;
import com.bosma.bplayerlib.player.MYCPlayer;
import com.bosma.bplayerlib.util.TimeUtil;

import java.io.File;
import java.sql.Time;

public class MainActivity extends AppCompatActivity {

    private MYCPlayer mycPlayer;
    private int MY_PERMISSIONS_REQUEST_CALL_SDCARD = 100;
    private TextView mTvTime;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mTvTime = findViewById(R.id.tv_time);

        mycPlayer = new MYCPlayer();

        mycPlayer.setOnPreparedListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                Log.d(MainActivity.class.getSimpleName(), "音频解码器已经准备好了");
                mycPlayer.start();
            }
        });

        mycPlayer.setOnLoadListener(new OnLoadListener() {
            @Override
            public void onLoad(boolean load) {
                if (load) {
                    MyLog.d("加载中...");
                } else {
                    MyLog.d("播放中...");
                }
            }
        });

        mycPlayer.setOnPauseResumeListener(new OnPauseResumeListener() {
            @Override
            public void onPuase(boolean pause) {
                if (pause) {
                    MyLog.d("暂停中...");
                } else {
                    MyLog.d("播放中...");
                }

            }
        });

        mycPlayer.setOnTimeinfoListener(new OnTimeInfoListener() {
            @Override
            public void onTImeInfo(TimeInfoBean timeInfoBean) {
//                MyLog.d(timeInfoBean.toString());
                Message message = Message.obtain();
                message.what = 1;
                message.obj = timeInfoBean;
                handler.sendMessage(message);

            }

        });

        mycPlayer.setOnErrorListener(new OnErrorListener() {
            @Override
            public void onError(int code, String msg) {
                MyLog.d("[" + code + "] : " + msg);
            }
        });

    }

    public void prepared(View view) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.CALL_PHONE},
                        MY_PERMISSIONS_REQUEST_CALL_SDCARD);
                return;
            } else {
                showToast("权限已申请");
            }
        }


//        mycPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
//        mycPlayer.setSource("http://ngcdn004.ncr.cn/live/dszs/index.m3u8");
        mycPlayer.setSource("http://ngcdn004.cnr.cn/live/dszs/index.m3u8");
//        String path = getInnerSDCardPath() + "/mydream.m4a";
//        // /storage/emulated/0/mydream.m4a
//        MyLog.d(path);
//        File file = new File(path);
//        if (file.exists()) {
//            mycPlayer.setSource(path);
//        }

        mycPlayer.prepared();

    }


    /**
     * 获取内置SD卡路径
     *
     * @return
     */
    public String getInnerSDCardPath() {
        return Environment.getExternalStorageDirectory().getPath();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        if (requestCode == MY_PERMISSIONS_REQUEST_CALL_SDCARD) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                showToast("权限已申请");
            } else {
                showToast("权限已拒绝");
            }
        }
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    private void showToast(String string) {
        Toast.makeText(MainActivity.this, string, Toast.LENGTH_LONG).show();
    }

    public void pause(View view) {
        mycPlayer.onPause();

    }

    public void resume(View view) {
        mycPlayer.onResume();
    }

    @SuppressLint("HandlerLeak")
    Handler handler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == 1) {
                TimeInfoBean timeInfoBean = (TimeInfoBean) msg.obj;

                mTvTime.setText(TimeUtil.secdsToDateFormat(timeInfoBean.getTotalTime(), timeInfoBean.getTotalTime()) + "/" + TimeUtil.secdsToDateFormat(timeInfoBean.getCurrentTime(), timeInfoBean.getTotalTime()));

            }
        }
    };

    public void stop(View view) {
        mycPlayer.onStop();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mycPlayer != null) {
            mycPlayer.onStop();
        }
    }
}
