//
// Created by Administrator on 2018/12/27.
//

#ifndef MYCPLAYER_MYCAUDIO_H
#define MYCPLAYER_MYCAUDIO_H

#include "MYCQueue.h"
#include "MYCPlayStatus.h"


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

    pthread_t thread_play;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    int ret = -1;

    uint8_t  *buffer = NULL;

    int data_size = 0;


public:
    MYCAudio(MYCPlayStatus *playStatus);

    ~MYCAudio();

    void play();

    int resampleAudio();

};


#endif //MYCPLAYER_MYCAUDIO_H
