//
// Created by Administrator on 2018/12/28.
//

#ifndef MYCPLAYER_MYCQUEUE_H
#define MYCPLAYER_MYCQUEUE_H

#include "queue"
#include "pthread.h"
#include "AndroidLog.h"
#include "MYCPlayStatus.h"
extern "C" {
#include "libavcodec/avcodec.h"
};


class MYCQueue {

public:

    //队列
    std::queue<AVPacket *> queuePacket;

    //条件锁
    pthread_mutex_t mutexPacket;

    //条件变量
    pthread_cond_t condPacket;

    MYCPlayStatus *playStatus;

public:
    MYCQueue(MYCPlayStatus *playstatus);
    ~MYCQueue();

    int putAvpacket(AVPacket *packet);
    int getAvpacket(AVPacket *packet);
    int getQueueSize();
    void clearAvPacket();

};


#endif //MYCPLAYER_MYCQUEUE_H
