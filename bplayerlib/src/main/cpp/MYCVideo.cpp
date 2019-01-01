
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
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;

        }


        //软解码
        if (avcodec_send_packet(video->avCodecContext, avPacket) != 0) {
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

        //YUV转换和获取
        if (avFrame->format == AV_PIX_FMT_YUV420P) {
            LOGE("当前视频是：YUV420P 格式")
            //直接返回给Java层进行渲染
            video->javaCallback->onCallRenderYUV(
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    avFrame->data[0],
                    avFrame->data[1],
                    avFrame->data[2]);
        } else {
            LOGE("当前视频不是YUV420P 格式")
            //转成420
            AVFrame *pFrameYUV420P = av_frame_alloc();

            //需要转换的内存大小
            int num = av_image_get_buffer_size(
                    AV_PIX_FMT_YUV420P,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    1);
            uint8_t *buffer = (uint8_t *) (av_malloc(num * sizeof(uint8_t)));

            av_image_fill_arrays(
                    pFrameYUV420P->data,
                    pFrameYUV420P->linesize,
                    buffer,
                    AV_PIX_FMT_YUV420P,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    1);

            SwsContext *sws_ctx = sws_getContext(
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    video->avCodecContext->pix_fmt,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    AV_PIX_FMT_YUV420P,
                    SWS_BICUBIC, NULL, NULL, NULL
            );

            if (!sws_ctx) {
                //创建不成功
                av_frame_free(&pFrameYUV420P);
                av_free(pFrameYUV420P);
                av_free(buffer);
                continue;
            }

            sws_scale(
                    sws_ctx,
                    avFrame->data,
                    avFrame->linesize,
                    0,
                    avFrame->height,
                    pFrameYUV420P->data,
                    pFrameYUV420P->linesize);

            //开始渲染

            //直接返回给Java层进行渲染
            video->javaCallback->onCallRenderYUV(
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    avFrame->data[0],
                    avFrame->data[1],
                    avFrame->data[2]);

            av_frame_free(&pFrameYUV420P);
            av_free(pFrameYUV420P);
            av_free(buffer);
            sws_freeContext(sws_ctx);


        }


        av_frame_free(&avFrame);
        av_free(avFrame);
        avFrame = NULL;


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

    if (playStatus != NULL) {
        playStatus = NULL;
    }

    if (javaCallback != NULL) {
        javaCallback = NULL;
    }


}
