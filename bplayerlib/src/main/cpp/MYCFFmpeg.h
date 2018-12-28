//
// Created by Administrator on 2018/12/27.
//

#ifndef MYCPLAYER_MYCFFMPEG_H
#define MYCPLAYER_MYCFFMPEG_H

#include <pthread.h>
#include "MYCJavaCallback.h"
#include "MYCAudio.h"

extern "C" {
#include <libavformat/avformat.h>
};


class MYCFFmpeg {

public:

    MYCJavaCallback *callbackJava = NULL;
    const char *url = NULL;
    pthread_t decodeThread;
    AVFormatContext *avFormatContext = NULL;
    MYCAudio *mycAudio = NULL;
    MYCPlayStatus *playStatus = NULL;


public:

    MYCFFmpeg(MYCPlayStatus *playStatus,MYCJavaCallback *callback, const char *url);

    ~MYCFFmpeg();

    void prepard();

    void decodeFFmpegThread();
    void start();

};


#endif //MYCPLAYER_MYCFFMPEG_H
