
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
    pthread_mutex_init(&codecMutex, NULL);

}

MYCVideo::~MYCVideo() {
    pthread_mutex_destroy(&codecMutex);

}

void *callPlayVideo(void *data) {

    MYCVideo *video = (MYCVideo *) (data);

    while (video->playStatus != NULL && !video->playStatus->exit) {


        if (video->playStatus->seek) {
            av_usleep(1000 * 100);//100毫秒
            continue;
        }

        if (video->playStatus->pause) {
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

        if (video->codectype == CODEC_MEDIACODEC) {
            LOGE("视频支持硬解码")
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
        } else if (video->codectype == CODEC_YUV) {

            //由于其他地方用到CodeContext,要加锁
            pthread_mutex_lock(&video->codecMutex);
            //软解码
            if (avcodec_send_packet(video->avCodecContext, avPacket) != 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                pthread_mutex_unlock(&video->codecMutex);
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
                pthread_mutex_unlock(&video->codecMutex);
                continue;
            }

            LOGE("子线程解码一个AvFrame 成功");

            //YUV转换和获取
            if (avFrame->format == AV_PIX_FMT_YUV420P) {
                LOGE("当前视频是：YUV420P 格式")


                //音视频同步
                double diff = video->getFrameDiffTime(avFrame);
                LOGE("当前视频音频时间差：%f", diff);

                av_usleep(video->getDelayTime(diff) * 1000 * 1000);

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
                    pthread_mutex_unlock(&video->codecMutex);
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
                //音视频同步
                double diff = video->getFrameDiffTime(pFrameYUV420P);
                LOGE("当前视频音频时间差：%f", diff);
                av_usleep(video->getDelayTime(diff) * 1000 * 1000);

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
            pthread_mutex_unlock(&video->codecMutex);

        }

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
        pthread_mutex_lock(&codecMutex);
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
        pthread_mutex_unlock(&codecMutex);
    }

    if (playStatus != NULL) {
        playStatus = NULL;
    }

    if (javaCallback != NULL) {
        javaCallback = NULL;
    }


}

double MYCVideo::getFrameDiffTime(AVFrame *avFrame) {
    double pts = av_frame_get_best_effort_timestamp(avFrame);

    if (pts == AV_NOPTS_VALUE) {
        pts = 0;//这个帧没有值
    }

    pts *= av_q2d(time_base);

    if (pts > 0) {
        clock = pts;
    }

    //时间差
    double diff = audio->clock - clock;
    return diff;
}

double MYCVideo::getDelayTime(double diff) {

    if (diff > 0.003) {
        delayTime = delayTime * 2 / 3;
        //不能超出默认值太多
        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 2;
        }

    } else if (diff < -0.003) {
        delayTime = delayTime * 3 / 2;

        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 2;
        }

    } else if (diff == 0.003) {

    }

    if (diff >= 0.3) {//视频延后了
        delayTime = 0;
    } else if (diff <= -0.3) {
        //延迟比较严重，以两倍速度来休眠
        delayTime = defaultDelayTime * 2;
    }
    if (fabs(diff) >= 10.0) {//这种情况可能是没有音频，就让它按实际默认值来延迟就行
        delayTime = defaultDelayTime;
    }
    return delayTime;
}
