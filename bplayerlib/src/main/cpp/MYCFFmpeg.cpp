//
// Created by Administrator on 2018/12/27.
//



#include "MYCFFmpeg.h"

MYCFFmpeg::~MYCFFmpeg() {

}

MYCFFmpeg::MYCFFmpeg(MYCJavaCallback *callback, const char *url) {
    this->callbackJava = callback;
    this->url = url;


}

void *decodeFFmpeg(void *data) {
    MYCFFmpeg *mycfFmpeg = (MYCFFmpeg *) (data);

    mycfFmpeg->decodeFFmpegThread();
    pthread_exit(&mycfFmpeg->decodeThread);


}

void MYCFFmpeg::prepard() {

    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);

}

void MYCFFmpeg::decodeFFmpegThread() {

    //1、注册解码器
    av_register_all();
    avformat_network_init();
    avFormatContext = avformat_alloc_context();

    //2、打开文件或网络流
    if (avformat_open_input(&avFormatContext, url, NULL, NULL) != 0) {
        if (LOG_DEBUG) {
            LOGE("cannot open url : %s ", url);
        }
        return;
    }

    //3、获取流信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        if (LOG_DEBUG) {
            LOGE("cannot find stream from url : %s ", url);
        }
        return;
    }

    //4、找到音频解码器类型
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (mycAudio == NULL) {
                mycAudio = new MYCAudio();
            }
            mycAudio->streamIndex = i;
            mycAudio->codecpar = avFormatContext->streams[i]->codecpar;
        }
    }

    if (!mycAudio) {
        if (LOG_DEBUG) {
            LOGE("cannot find decoder");
        }
        return;
    }
    //5、找到音频解码器
    AVCodec *dec = avcodec_find_decoder(mycAudio->codecpar->codec_id);
    if (!dec) {
        if (LOG_DEBUG) {
            LOGE("cannot find decoder");
        }
        return;
    }

    //6、利用音频解码器创建解码器上下文
    mycAudio->avCodecContext = avcodec_alloc_context3(dec);
    if (!mycAudio->avCodecContext) {
        if (LOG_DEBUG) {
            LOGE("cannot alloc new decoder ctx");
        }
        return;
    }

    //填充初始化好的解码器参数
    if (avcodec_parameters_to_context(mycAudio->avCodecContext, mycAudio->codecpar) < 0) {
        if (LOG_DEBUG) {
            LOGE("cannot fill decoder ctx");
        }
        return;
    }

    //7、打开音频解码器
    if (avcodec_open2(mycAudio->avCodecContext, dec, 0) != 0) {
        if (LOG_DEBUG) {
            LOGE("cannot open audio stream");
        }
        return;
    }

    callbackJava->onCallPrepared(THREAD_CHILD);

}

void MYCFFmpeg::start() {

    if(mycAudio == NULL) {
        if (LOG_DEBUG) {
            LOGE("audio is null");
        }
        return;
    }
    int count = 0;
    while (1) {
        AVPacket *avPacket = av_packet_alloc();
        if(av_read_frame(avFormatContext, avPacket) == 0) {

            if(avPacket->stream_index == mycAudio-> streamIndex) {
                count++;
                if (LOG_DEBUG) {
                    LOGE("decode %d frame", count);
                }
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
            } else {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
            }

        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            break;
        }
    }

}
