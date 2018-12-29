//
// Created by Administrator on 2018/12/27.
//

#include "MYCAudio.h"


MYCAudio::MYCAudio(MYCPlayStatus *playStatus) {
    this->playStatus = playStatus;
    this->queue = new MYCQueue(playStatus);

}

MYCAudio::~MYCAudio() {

}


void *decodePlay(void *data) {
    MYCAudio *mycAudio = static_cast<MYCAudio *>(data);


    pthread_exit(&mycAudio->thread_play);

}

void MYCAudio::play() {

    pthread_create(&thread_play, NULL, decodePlay, this);


}

/**
 * 重采样
 */
int MYCAudio::resampleAudio() {

    while (playStatus != NULL && !playStatus->exit) {
        avPacket = av_packet_alloc();
        if (queue->getAvpacket(avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        avcodec_send_packet(avCodecContext, avPacket);

    }


    return 0;
}

