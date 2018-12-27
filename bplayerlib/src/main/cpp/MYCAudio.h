//
// Created by Administrator on 2018/12/27.
//

#ifndef MYCPLAYER_MYCAUDIO_H
#define MYCPLAYER_MYCAUDIO_H

extern "C" {
#include <libavcodec/avcodec.h>
};

class MYCAudio {

public:

    int streamIndex = -1;

    AVCodecParameters *codecpar = NULL;
    AVCodecContext *avCodecContext = NULL;


public:
    MYCAudio();
    ~MYCAudio();

};


#endif //MYCPLAYER_MYCAUDIO_H
