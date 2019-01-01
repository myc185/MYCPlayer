
/**
* @ClassName: MYCVideo
* @Description: java类作用描述 
* @Author: Administrator
* @Date: 2019/1/1 7:38
*/
//
// Created by Administrator on 2019/1/1.
//

#include "MYCVideo.h"

MYCVideo::MYCVideo(MYCPlayStatus *playStatus, MYCJavaCallback *javaCallback) {

    this->playStatus = playStatus;
    this->javaCallback = javaCallback;
    queue = new MYCQueue(playStatus);


}

MYCVideo::~MYCVideo() {


}

void *callPlayVideo(void *data) {

    MYCVideo *video = (MYCVideo *) (data);

    while (video->playStatus != NULL && !video->playStatus->exit) {
        AVPacket *avPacket = av_packet_alloc();
        if (video->queue->getAvpacket(avPacket) == 0) {
            //解码渲染
            LOGE("线程中获取视频 AVPacket")

        }
        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;

    }

    pthread_exit(&video->thread_play);
}


void MYCVideo::play() {
    pthread_create(&thread_play, NULL, callPlayVideo, this);
}
