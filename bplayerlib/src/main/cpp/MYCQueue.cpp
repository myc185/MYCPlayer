//
// Created by Administrator on 2018/12/28.
//

#include "MYCQueue.h"


MYCQueue::MYCQueue(MYCPlayStatus *playStatus) {
    this->playStatus = playStatus;
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);

}

MYCQueue::~MYCQueue() {

    clearAvPacket();
    //释放
    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);

}

int MYCQueue::putAvpacket(AVPacket *packet) {

    pthread_mutex_lock(&mutexPacket);

    queuePacket.push(packet);
    if (LOG_DEBUG) {
//        LOGD("put an avpacket to queue, number : %d ", queuePacket.size());
    }

    pthread_cond_signal(&condPacket);

    pthread_mutex_unlock(&mutexPacket);

    return 0;
}

int MYCQueue::getAvpacket(AVPacket *packet) {

    pthread_mutex_lock(&mutexPacket);
    while (playStatus != NULL && !playStatus->exit) {

        if (queuePacket.size() > 0) {
            AVPacket *avPacket = queuePacket.front();
            if (av_packet_ref(packet, avPacket) == 0) {
                queuePacket.pop();
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            if (LOG_DEBUG) {
//                LOGD("GET an avpacket from queue, left number  :  %d", queuePacket.size());
            }
            break;

        } else {
            pthread_cond_wait(&condPacket, &mutexPacket);
        }

    }

    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int MYCQueue::getQueueSize() {
    int size = 0;
    pthread_mutex_lock(&mutexPacket);

    size = queuePacket.size();

    pthread_mutex_unlock(&mutexPacket);
    return size;
}

void MYCQueue::clearAvPacket() {

    pthread_cond_signal(&condPacket);
    pthread_mutex_lock(&mutexPacket);

    while (!queuePacket.empty()) {
        AVPacket *packet = queuePacket.front();
        queuePacket.pop();
        av_packet_free(&packet);
        av_free(packet);
        packet = NULL;
    }


    pthread_mutex_unlock(&mutexPacket);

}

