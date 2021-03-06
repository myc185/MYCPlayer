//
// Created by Administrator on 2019/1/1.
//

#ifndef MYCPLAYER_MYCVIDEO_H
#define MYCPLAYER_MYCVIDEO_H

#include "MYCQueue.h"
#include "MYCJavaCallback.h"
#include "MYCAudio.h"

//软解码
#define CODEC_YUV 0

//硬解码
#define CODEC_MEDIACODEC 1

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
};


class MYCVideo {

public:

    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *codecpar = NULL;
    MYCQueue *queue = NULL;
    MYCPlayStatus *playStatus = NULL;
    MYCJavaCallback *javaCallback = NULL;

    AVRational time_base;

    pthread_t thread_play;

    MYCAudio *audio = NULL;
    double clock = 0;
    double delayTime = 0;

    //默认延迟时间，也就是一帧视频播放事件 如果一秒25帧，就是0.04秒
    double defaultDelayTime = 0.04;

    pthread_mutex_t codecMutex;

    //解码类型
    int codectype = CODEC_YUV;


public:
    MYCVideo(MYCPlayStatus *playStatus, MYCJavaCallback *javaCallback);

    ~MYCVideo();

    void play();

    void release();

    double getFrameDiffTime(AVFrame *avFrame);

    double getDelayTime(double diff);

};


#endif //MYCPLAYER_MYCVIDEO_H
