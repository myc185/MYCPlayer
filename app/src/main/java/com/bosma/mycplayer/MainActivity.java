package com.bosma.mycplayer;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

import com.bosma.bplayerlib.Demo;

public class MainActivity extends AppCompatActivity {

    private Demo mDemo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mDemo = new Demo();
        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(mDemo.stringFromJNI());
        mDemo.testFfmpeg();
    }


}
