/**
 * Copyright (C), 2015-2019, (Bosma)博冠智能技术有限公司
 * FileName: YCRender
 * Author: Administrator
 * Date: 2019/1/1 10:16
 * Description: 渲染器
 * History:
 * <author> <time> <version> <desc>
 * 作者姓名 修改时间 版本号 描述
 */
package com.bosma.bplayerlib.opengl;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import com.bosma.bplayerlib.R;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * @ClassName: YCRender
 * @Description: 作用描述
 * @Author: Administrator
 * @Date: 2019/1/1 10:16
 */
public class YCRender implements GLSurfaceView.Renderer {


    private Context mContext;

    private final float[] vertexData = {

            -1f, -1f,
            1f, -1f,
            -1f, 1f,
            1f, 1f
    };

    /***
     * 纹理坐标点(要与顶点一一对应)
     */
    private final float[] textureData = {
            0f, 1f,
            1f, 1f,
            0f, 0f,
            1f, 0f

    };


    private FloatBuffer vertexBuffer;
    private FloatBuffer textureBuffer;


    private int avPosition_yuv;
    private int afPosition_yuv;
    private int mProgramYUV;


    //创建yuv三个纹理
    private int sampler_y;
    private int sampler_u;
    private int sampler_v;

    private int[] textureid_yuv;

    private int width_yuv;
    private int height_yuv;

    private ByteBuffer y;
    private ByteBuffer u;
    private ByteBuffer v;


    public YCRender(Context context) {
        this.mContext = context;
        vertexBuffer = ByteBuffer.allocateDirect(vertexData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(vertexData);
        vertexBuffer.position(0);

        textureBuffer = ByteBuffer.allocateDirect(textureData.length * 4)
                .order(ByteOrder.nativeOrder())
                .asFloatBuffer()
                .put(textureData);
        textureBuffer.position(0);

    }


    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        initRenderYUV();
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int with, int height) {
        GLES20.glViewport(0, 0, with, height);

    }

    @Override
    public void onDrawFrame(GL10 gl10) {

        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        //用什么颜色清屏，显示的时候就是什么背景的颜色
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        renderYUV();
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);//两个三角形,六个顶点，但有两个复用
    }

    /**
     * 初始化YUV
     */
    private void initRenderYUV() {
        String vertexSource = YCShaderUtil.readRawTxt(mContext, R.raw.vertex_shader);
        String fragmentSource = YCShaderUtil.readRawTxt(mContext, R.raw.fragment_shader);
        mProgramYUV = YCShaderUtil.createProgram(vertexSource, fragmentSource);

        avPosition_yuv = GLES20.glGetAttribLocation(mProgramYUV, "av_Position");//顶点
        afPosition_yuv = GLES20.glGetAttribLocation(mProgramYUV, "af_Position");//纹理

        sampler_y = GLES20.glGetUniformLocation(mProgramYUV, "sampler_y");
        sampler_u = GLES20.glGetUniformLocation(mProgramYUV, "sampler_u");
        sampler_v = GLES20.glGetUniformLocation(mProgramYUV, "sampler_v");
        //创建纹理
        textureid_yuv = new int[3];
        GLES20.glGenTextures(3, textureid_yuv, 0);
        for (int i = 0; i < textureid_yuv.length; i++) {
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureid_yuv[i]);

            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_REPEAT);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        }


    }

    public void setYUVRenderData(int width, int height, byte[] y, byte[] u, byte[] v) {
        this.width_yuv = width;
        this.height_yuv = height;
        this.y = ByteBuffer.wrap(y);
        this.u = ByteBuffer.wrap(u);
        this.v = ByteBuffer.wrap(v);
    }

    private void renderYUV() {

        if (width_yuv > 0 && height_yuv > 0 && y != null && u != null && v != null) {
            GLES20.glUseProgram(mProgramYUV);

            GLES20.glEnableVertexAttribArray(avPosition_yuv);
            GLES20.glVertexAttribPointer(avPosition_yuv, 2, GLES20.GL_FLOAT, false, 8, vertexBuffer);

            GLES20.glEnableVertexAttribArray(afPosition_yuv);
            GLES20.glVertexAttribPointer(afPosition_yuv, 2, GLES20.GL_FLOAT, false, 8, textureBuffer);

            //激活纹理
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            //给纹理赋值
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureid_yuv[0]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, width_yuv, height_yuv, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, y);


            //激活纹理
            GLES20.glActiveTexture(GLES20.GL_TEXTURE1);
            //给纹理赋值
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureid_yuv[1]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, width_yuv / 2, height_yuv / 2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, u);


            //激活纹理
            GLES20.glActiveTexture(GLES20.GL_TEXTURE2);
            //给纹理赋值
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureid_yuv[2]);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_LUMINANCE, width_yuv / 2, height_yuv / 2, 0, GLES20.GL_LUMINANCE, GLES20.GL_UNSIGNED_BYTE, v);


            //绑定为纹理，让它转换成rgb值
            GLES20.glUniform1i(sampler_y, 0);
            GLES20.glUniform1i(sampler_u, 1);
            GLES20.glUniform1i(sampler_v, 2);

            y.clear();
            u.clear();
            v.clear();
            y = null;
            u = null;
            v = null;

        }

    }
}