
/**
* @ClassName: MYCVideo
* @Description: 视频解码
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


        if (video->playStatus->seek) {
            av_usleep(1000 * 100);//100毫秒
            continue;
        }

        if (video->queue->getQueueSize() == 0) {
            if (!video->playStatus->load) {
                video->playStatus->load = true;
                video->javaCallback->onCallLoad(THREAD_CHILD, true);
            }
            av_usleep(1000 * 100);//100毫秒
            continue;
        } else {
            if (!video->playStatus->load) {
                video->playStatus->load = false;
                video->javaCallback->onCallLoad(THREAD_CHILD, false);
            }
        }


        AVPacket *avPacket = av_packet_alloc();
        if (video->queue->getAvpacket(avPacket) != 0) {
            //解码渲染
            LOGE("线程中获取视频 AVPacket")
            av_usleep(1000 * 100);//100毫秒
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;

        }


        //软解码
        if (avcodec_send_packet(video->avCodecContext, avPacket) != 0) {
            av_usleep(1000 * 100);//100毫秒
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        AVFrame *avFrame = av_frame_alloc();
        if (avcodec_receive_frame(video->avCodecContext, avFrame) != 0) {

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            continue;
        }

        LOGE("子线程解码一个AvFrame 成功");


        av_frame_free(&avFrame);
        av_free(avPacket);
        avPacket = NULL;


        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;

    }

    pthread_exit(&video->thread_play);
}


void MYCVideo::play() {
    pthread_create(&thread_play, NULL, callPlayVideo, this);
}

void MYCVideo::release() {

    if (queue != NULL) {
        delete (queue);
        queue = NULL;
    }

    if (avCodecContext != NULL) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }

    if(playStatus != NULL) {
        playStatus = NULL;
    }

    if(javaCallback != NULL) {
        javaCallback = NULL;
    }


}
