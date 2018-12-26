package com.bosma.bplayerlib;

import android.util.Log;

/**
 * @ProjectName: MYCPlayer
 * @Package: com.bosma.bplayerlib
 * @ClassName: Demo
 * @Description: java类作用描述
 * @Author: 莫运川
 * @CreateDate: 2018/12/26 15:58
 * @UpdateUser: 更新者
 * @UpdateDate: 2018/12/26 15:58
 * @UpdateRemark: 更新说明
 * @Version: 1.0.0
 */
public class Demo {


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


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native void testFfmpeg();
}
