//
// Created by Administrator on 2018/12/27.
//

#include "MYCAudio.h"


MYCAudio::MYCAudio(MYCPlayStatus *playStatus, int sample_rate) {
    this->playStatus = playStatus;
    this->queue = new MYCQueue(playStatus);
    this->sample_rate = sample_rate;
    buffer = static_cast<uint8_t *>(av_malloc(44100 * 2 * 2));

}

MYCAudio::~MYCAudio() {

}


void *decodePlay(void *data) {
    MYCAudio *mycAudio = (MYCAudio *) (data);
    mycAudio->initOpenSLES();
    pthread_exit(&mycAudio->thread_play);

}

void MYCAudio::play() {


    pthread_create(&thread_play, NULL, decodePlay, this);


}

//FILE *outFile = fopen("/storage/emulated/0/mymusic.pcm", "w");

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

        ret = avcodec_send_packet(avCodecContext, avPacket);
        if (ret != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        avFrame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, avFrame);

        if (ret == 0) {

            if (avFrame->channels > 0 && avFrame->channel_layout == 0) {
                avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
            } else if (avFrame->channels == 0 && avFrame->channel_layout > 0) {
                avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
            }

            //重采样上下文
            SwrContext *swr_ctx;
            swr_ctx = swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO,
                    AV_SAMPLE_FMT_S16,
                    avFrame->sample_rate,
                    avFrame->channel_layout,
                    static_cast<AVSampleFormat>(avFrame->format),
                    avFrame->sample_rate,
                    NULL,
                    NULL
            );

            if (!swr_ctx || swr_init(swr_ctx) < 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;

                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame = NULL;
                if (swr_ctx != NULL) {
                    swr_free(&swr_ctx);
                    swr_ctx = NULL;
                }
                continue;
            }

            int nb = swr_convert(swr_ctx,
                                 &buffer,
                                 avFrame->nb_samples,
                                 (const uint8_t **) (avFrame->data),
                                 avFrame->nb_samples);
            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);


//            fwrite(buffer, 1, data_size, outFile);//保存文件
            if (LOG_DEBUG) {
                LOGD("data size is %d : ", data_size);
            }

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            swr_free(&swr_ctx);
            swr_ctx = NULL;
            break;

        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            continue;
        }


    }


    return data_size;
}


void pcmBufferCallback(SLAndroidSimpleBufferQueueItf buf, void *context) {

    MYCAudio *mycAudio = (MYCAudio *) (context);

    if (mycAudio != NULL) {
        int buffer_size = mycAudio->resampleAudio();
        if (buffer_size > 0) {
            (*mycAudio->pcmBufferQueue)->Enqueue(mycAudio->pcmBufferQueue, mycAudio->buffer,
                                                 buffer_size);
        }

    }

}

void MYCAudio::initOpenSLES() {

    SLresult result;
    //第一步
    //创建
    result = slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    //realize
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    //Get engineEngine
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    //第二步：创建混音器
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};//都只放一个
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};

    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    (void) result;
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    (void) result;
    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentReverb);
    if (SL_RESULT_SUCCESS == result) {
        (*outputMixEnvironmentReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentReverb,
                                                                        &reverbSettings);
        (void) result;
    }


    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, NULL};
    //创建播放器
    SLDataLocator_AndroidBufferQueue androidBufferQueue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                           2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2,
            getCurrentSampleRateForOpensles(sample_rate),//采样率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//左前方右前方
            SL_BYTEORDER_LITTLEENDIAN

    };


    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};

    SLDataSource slDataSource = {&androidBufferQueue, &pcm};

    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource, &audioSnk, 1,
                                       ids, req);

    (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_BUFFERQUEUE, &pcmBufferQueue);

    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallback, this);

    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);//播放状态
    pcmBufferCallback(pcmBufferQueue, this);//执行回调

}

int MYCAudio::getCurrentSampleRateForOpensles(int sample_rate) {
    int rate = 0;
    switch (sample_rate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }


    return rate;
}

