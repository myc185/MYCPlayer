//
// Created by Administrator on 2018/12/27.
//



#include "MYCFFmpeg.h"

MYCFFmpeg::~MYCFFmpeg() {
    pthread_mutex_destroy(&init_mutex);
}

MYCFFmpeg::MYCFFmpeg(MYCPlayStatus *playStatus, MYCJavaCallback *callback, const char *url) {
    this->callbackJava = callback;
    this->url = url;
    this->playStatus = playStatus;
    pthread_mutex_init(&init_mutex, NULL);


}

void *decodeFFmpeg(void *data) {
    MYCFFmpeg *mycfFmpeg = (MYCFFmpeg *) (data);
    mycfFmpeg->decodeFFmpegThread();
    pthread_exit(&mycfFmpeg->decodeThread);


}

void MYCFFmpeg::prepard() {
    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);

}

int avformat_callback(void *ctx) {


    MYCFFmpeg *fmpeg = (MYCFFmpeg *) (ctx);
    if (fmpeg->playStatus->exit) {
        if (LOG_DEBUG) {
            LOGE("avformat_open_input 回调中断 AVERROR_EOF");
        }
        return AVERROR_EOF;
    }
    return 0;
}

void MYCFFmpeg::decodeFFmpegThread() {


    pthread_mutex_lock(&init_mutex);

    if (LOG_DEBUG) {
        LOGE("decodeFFmpegThread 被调用 ");
    }
    //1、注册解码器
    av_register_all();
    avformat_network_init();
    avFormatContext = avformat_alloc_context();
    AVDictionary *opts = NULL;
    av_dict_set(&opts, "timeout", "5000000", 0);//设置超时3秒

    //阻塞回调
    avFormatContext->interrupt_callback.callback = avformat_callback;
    avFormatContext->interrupt_callback.opaque = this;
    //2、打开文件或网络流
    if (avformat_open_input(&avFormatContext, url, NULL, &opts) != 0) {
        if (LOG_DEBUG) {
            LOGE("cannot open url : %s ", url);
        }
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        av_dict_free(&opts);
        opts = NULL;
        return;
    }

    av_dict_free(&opts);
    opts = NULL;

    //3、获取流信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        if (LOG_DEBUG) {
            LOGE("cannot find stream from url : %s ", url);
        }
        exit = true;
        pthread_mutex_unlock(&init_mutex);
        return;
    }

    //4、找到音频解码器类型
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (mycAudio == NULL) {
                mycAudio = new MYCAudio(playStatus,
                                        avFormatContext->streams[i]->codecpar->sample_rate,
                                        callbackJava);
            }
            mycAudio->streamIndex = i;
            mycAudio->codecpar = avFormatContext->streams[i]->codecpar;
            mycAudio->duration = avFormatContext->duration / AV_TIME_BASE;
            mycAudio->time_base = avFormatContext->streams[i]->time_base;

        }
    }

    if (!mycAudio) {
        if (LOG_DEBUG) {
            LOGE("cannot find decoder");
        }
        exit = true;
        pthread_mutex_unlock(&init_mutex);

        return;
    }
    //5、找到音频解码器
    AVCodec *dec = avcodec_find_decoder(mycAudio->codecpar->codec_id);
    if (!dec) {
        if (LOG_DEBUG) {
            LOGE("cannot find decoder");
        }
        exit = true;
        pthread_mutex_unlock(&init_mutex);

        return;
    }

    //6、利用音频解码器创建解码器上下文
    mycAudio->avCodecContext = avcodec_alloc_context3(dec);
    if (!mycAudio->avCodecContext) {
        if (LOG_DEBUG) {
            LOGE("cannot alloc new decoder ctx");
        }
        exit = true;
        pthread_mutex_unlock(&init_mutex);

        return;
    }

    //填充初始化好的解码器参数
    if (avcodec_parameters_to_context(mycAudio->avCodecContext, mycAudio->codecpar) < 0) {
        if (LOG_DEBUG) {
            LOGE("cannot fill decoder ctx");
        }
        exit = true;
        pthread_mutex_unlock(&init_mutex);

        return;
    }

    //7、打开音频解码器
    if (avcodec_open2(mycAudio->avCodecContext, dec, 0) != 0) {
        if (LOG_DEBUG) {
            LOGE("cannot open audio stream");
        }
        exit = true;
        pthread_mutex_unlock(&init_mutex);

        return;
    }

    callbackJava->onCallPrepared(THREAD_CHILD);
    pthread_mutex_unlock(&init_mutex);

}

void MYCFFmpeg::start() {

    if (mycAudio == NULL) {
        if (LOG_DEBUG) {
            LOGE("audio is null");
        }
        return;
    }

    //刚开始解码是没数据的，但是获取数据的时候会阻塞，因此可以在这里提前调用
    mycAudio->play();


    int count = 0;
    while (playStatus != NULL && !playStatus->exit) {
        AVPacket *avPacket = av_packet_alloc();
        if (av_read_frame(avFormatContext, avPacket) == 0) {

            if (avPacket->stream_index == mycAudio->streamIndex) {
                count++;
                if (LOG_DEBUG) {
//                    LOGE("decode %d frame", count);
                }
                mycAudio->queue->putAvpacket(avPacket);

            } else {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
            }


        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            //释放缓存
            while (playStatus != NULL && !playStatus->exit) {
                if (mycAudio->queue->getQueueSize() > 0) {
                    continue;
                } else {
                    playStatus->exit = true;
                    break;
                }

            }
            break;
        }
    }

    exit = true;
    if (LOG_DEBUG) {
        LOGD("解码完成！")
    }


}

void MYCFFmpeg::pause() {

    if (mycAudio != NULL) {
        mycAudio->puase();
    }

}

void MYCFFmpeg::resume() {
    if (mycAudio != NULL) {
        mycAudio->resume();
    }
}

void MYCFFmpeg::release() {
    if (LOG_DEBUG) {
        LOGE("FFmpeg release");
    }
    if (playStatus->exit) {
        return;
    }
    playStatus->exit = true;
    pthread_mutex_lock(&init_mutex);
    int spleepCount = 0;
    while (!exit) {
        if (spleepCount > 1000) {
            exit = true;
        }

        if (LOG_DEBUG) {
            LOGE("wait ffmpeg exit %d", spleepCount);
        }
        spleepCount++;
        av_usleep(1000 * 10);
    }

    if (LOG_DEBUG) {
        LOGE("释放 Audio");
    }

    if (mycAudio != NULL) {
        mycAudio->release();
        delete (mycAudio);
        mycAudio = NULL;
    }

    if (LOG_DEBUG) {
        LOGE("释放 封装格式上下文");
    }
    if (avFormatContext != NULL) {
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);
        avFormatContext = NULL;
    }

    if (playStatus != NULL) {
        playStatus = NULL;
    }

    if (LOG_DEBUG) {
        LOGE("释放 callJava");
    }
    if (callbackJava != NULL) {
        callbackJava = NULL;
    }

    pthread_mutex_unlock(&init_mutex);


}
