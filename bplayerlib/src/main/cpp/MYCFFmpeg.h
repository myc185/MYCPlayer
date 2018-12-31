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
#include <libavutil/time.h>
};


class MYCFFmpeg {

public:

    MYCJavaCallback *callbackJava = NULL;
    const char *url = NULL;
    pthread_t decodeThread;
    AVFormatContext *avFormatContext = NULL;
    MYCAudio *mycAudio = NULL;
    MYCPlayStatus *playStatus = NULL;

    pthread_mutex_t init_mutex;
    bool exit = false;

    int duration = 0;
    pthread_mutex_t seek_mutex;

public:

    MYCFFmpeg(MYCPlayStatus *playStatus,MYCJavaCallback *callback, const char *url);

    ~MYCFFmpeg();

    void prepard();

    void decodeFFmpegThread();

    void start();

    void pause();

    void resume();

    void release();

    void seek(int64_t secds) ;

    void setVolume(int percent);

    void setMute(int mute);

};


#endif //MYCPLAYER_MYCFFMPEG_H
