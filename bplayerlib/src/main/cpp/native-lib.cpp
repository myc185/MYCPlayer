#include <jni.h>
#include <string>
#include "android/log.h"
#include "AndroidLog.h"
#include "MYCJavaCallback.h"
#include "MYCFFmpeg.h"


extern "C" {
#include <libavformat/avformat.h>
}

JavaVM *javaVM = NULL;

MYCJavaCallback *callbackJava = NULL;
MYCFFmpeg *mycfFmpeg = NULL;

MYCPlayStatus *playStatus = NULL;
pthread_t thread_start;

bool nexit = true;

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    jint result = -1;
    javaVM = vm;
    JNIEnv *env;
    if (vm->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }

    return JNI_VERSION_1_4;

}


extern "C" JNIEXPORT jstring JNICALL
Java_com_bosma_bplayerlib_Demo_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Welcome to BPlayer!";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bosma_bplayerlib_Demo_testFfmpeg(JNIEnv *env, jobject instance) {

    av_register_all();
    AVCodec *c_temp = av_codec_next(NULL);
    while (c_temp != NULL) {
        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                LOGI("[Video]:%s", c_temp->name);
                break;
            case AVMEDIA_TYPE_AUDIO:
                LOGI("[Audio]:%s", c_temp->name);
                break;
            default:
                LOGI("[Other]:%s", c_temp->name);
                break;
        }
        c_temp = c_temp->next;
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_bosma_bplayerlib_player_MYCPlayer_n_1prepared(JNIEnv *env, jobject instance,
                                                       jstring source_) {
    const char *source = env->GetStringUTFChars(source_, 0);

    if (mycfFmpeg == NULL) {

        if (callbackJava == NULL) {
            callbackJava = new MYCJavaCallback(javaVM, env, &instance);
        }
        callbackJava->onCallLoad(THREAD_MAIN, true);
        playStatus = new MYCPlayStatus();

        mycfFmpeg = new MYCFFmpeg(playStatus, callbackJava, source);
        mycfFmpeg->prepard();
    }

//    env->ReleaseStringUTFChars(source_, source);
}

void *startCallback(void *data) {
    MYCFFmpeg *fmpeg = (MYCFFmpeg *) (data);
    fmpeg->start();
    pthread_exit(&thread_start);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bosma_bplayerlib_player_MYCPlayer_n_1start(JNIEnv *env, jobject instance) {

    if (mycfFmpeg == NULL) {
        return;
    }
    pthread_create(&thread_start, NULL, startCallback, mycfFmpeg);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_bosma_bplayerlib_player_MYCPlayer_n_1pause(JNIEnv *env, jobject instance) {

    if (mycfFmpeg == NULL) {
        return;
    }
    mycfFmpeg->pause();


}

extern "C"
JNIEXPORT void JNICALL
Java_com_bosma_bplayerlib_player_MYCPlayer_n_1resume(JNIEnv *env, jobject instance) {

    if (mycfFmpeg == NULL) {
        return;
    }
    mycfFmpeg->resume();


}

extern "C"
JNIEXPORT void JNICALL
Java_com_bosma_bplayerlib_player_MYCPlayer_n_1stop(JNIEnv *env, jobject instance) {

    if (!nexit) {
        return;
    }

    jclass jlz = env->GetObjectClass(instance);
    jmethodID jmid_next = env->GetMethodID(jlz, "onCallNext", "()V");

    nexit = false;
    if (mycfFmpeg != NULL) {
        mycfFmpeg->release();
        delete (mycfFmpeg);
        mycfFmpeg = NULL;
        if (callbackJava != NULL) {
            delete (callbackJava);
            callbackJava = NULL;
        }

        if (playStatus != NULL) {
            delete (playStatus);
            playStatus = NULL;
        }
    }
    nexit = true;
    env->CallVoidMethod(instance, jmid_next);//播放下一曲

}


extern "C"
JNIEXPORT void JNICALL
Java_com_bosma_bplayerlib_player_MYCPlayer_n_1seek(JNIEnv *env, jobject instance, jint secds) {

    if (mycfFmpeg == NULL) {
        return;
    }
    mycfFmpeg->seek(secds);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_bosma_bplayerlib_player_MYCPlayer_n_1duration(JNIEnv *env, jobject instance) {


    if (mycfFmpeg != NULL) {
        return mycfFmpeg->duration;
    }


}

extern "C"
JNIEXPORT void JNICALL
Java_com_bosma_bplayerlib_player_MYCPlayer_n_1volume(JNIEnv *env, jobject instance, jint percent) {

    if (mycfFmpeg != NULL) {
        mycfFmpeg->setVolume(percent);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_bosma_bplayerlib_player_MYCPlayer_n_1mute(JNIEnv *env, jobject instance, jint mute) {

    if (mycfFmpeg != NULL) {
        mycfFmpeg->setMute(mute);
    }


}