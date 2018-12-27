package com.bosma.mycplayer;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.bosma.bplayerlib.Demo;
import com.bosma.bplayerlib.listener.OnPreparedListener;
import com.bosma.bplayerlib.player.MYCPlayer;

public class MainActivity extends AppCompatActivity {

    private MYCPlayer mycPlayer;


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
        mycPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        mycPlayer.prepared();

    }
}
