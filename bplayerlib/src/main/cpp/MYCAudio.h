//
// Created by Administrator on 2018/12/27.
//

#ifndef MYCPLAYER_MYCAUDIO_H
#define MYCPLAYER_MYCAUDIO_H

#include "MYCQueue.h"
#include "MYCPlayStatus.h"

extern "C" {
#include <libavcodec/avcodec.h>
};

class MYCAudio {

public:

    int streamIndex = -1;

    AVCodecParameters *codecpar = NULL;
    AVCodecContext *avCodecContext = NULL;
    MYCQueue *queue = NULL;
    MYCPlayStatus *playStatus = NULL;


public:
    MYCAudio(MYCPlayStatus *playStatus);
    ~MYCAudio();

};


#endif //MYCPLAYER_MYCAUDIO_H
