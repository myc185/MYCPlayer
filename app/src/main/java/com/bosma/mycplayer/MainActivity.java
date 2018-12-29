package com.bosma.mycplayer;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.bosma.bplayerlib.Demo;
import com.bosma.bplayerlib.listener.OnPreparedListener;
import com.bosma.bplayerlib.log.MyLog;
import com.bosma.bplayerlib.player.MYCPlayer;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    private MYCPlayer mycPlayer;
    private int MY_PERMISSIONS_REQUEST_CALL_SDCARD = 100;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mycPlayer = new MYCPlayer();

        mycPlayer.setOnPreparedListener(new OnPreparedListener() {
            @Override
            public void onPrepared() {
                Log.d(MainActivity.class.getSimpleName(), "音频解码器已经准备好了");
                mycPlayer.start();
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


        mycPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        String path = getInnerSDCardPath() + "/mydream.m4a";
        // /storage/emulated/0/mydream.m4a
        MyLog.d(path);
        File file = new File(path);
        if (file.exists()) {
            mycPlayer.setSource(path);
        }
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

}
