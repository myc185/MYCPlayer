//
// Created by Administrator on 2019/1/1.
//

#ifndef MYCPLAYER_MYCVIDEO_H
#define MYCPLAYER_MYCVIDEO_H

#include "MYCQueue.h"
#include "MYCJavaCallback.h"

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


public:
    MYCVideo(MYCPlayStatus *playStatus, MYCJavaCallback *javaCallback);

    ~MYCVideo();

    void play();

    void release();

};


#endif //MYCPLAYER_MYCVIDEO_H
