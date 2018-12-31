//
// Created by Administrator on 2018/12/27.
//

#ifndef MYCPLAYER_MYCAUDIO_H
#define MYCPLAYER_MYCAUDIO_H

#include "MYCQueue.h"
#include "MYCPlayStatus.h"
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES.h>
#include "MYCJavaCallback.h"


extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
};

class MYCAudio {

public:

    int streamIndex = -1;

    AVCodecParameters *codecpar = NULL;
    AVCodecContext *avCodecContext = NULL;
    MYCQueue *queue = NULL;
    MYCPlayStatus *playStatus = NULL;
    MYCJavaCallback *javaCallback = NULL;

    pthread_t thread_play;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int ret = -1;

    uint8_t *buffer = NULL;
    int data_size = 0;
    int sample_rate = 0;
    //总长度
    int duration = 0;

    AVRational time_base;

    double now_time = 0;
    double clock = 0;
    double last_time = 0;

    //默认声音
    int volumePercent = 100;


    //引擎接口
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    //混音器
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentReverb = NULL;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;

    //PCM
    SLObjectItf pcmPlayerObject = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    SLVolumeItf pcmVolumePlay = NULL;

    //缓冲队列接口
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;


public:
    MYCAudio(MYCPlayStatus *playStatus, int sample_rate, MYCJavaCallback *callback);

    ~MYCAudio();

    void play();

    int resampleAudio();

    void initOpenSLES();

    int getCurrentSampleRateForOpensles(int sample_rate);

    void puase();

    void resume();

    void stop();

    void release();

    void setVolume(int percent);
};


#endif //MYCPLAYER_MYCAUDIO_H
