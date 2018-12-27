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

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    jint  result = -1;
    javaVM = vm;
    JNIEnv *env;
    if(vm -> GetEnv((void **)(&env), JNI_VERSION_1_4) != JNI_OK) {

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

    if(mycfFmpeg == NULL) {

        if(callbackJava == NULL) {
            callbackJava = new MYCJavaCallback(javaVM, env, &instance);
        }

        mycfFmpeg = new MYCFFmpeg(callbackJava, source);
        mycfFmpeg->prepard();
    }

//    env->ReleaseStringUTFChars(source_, source);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_bosma_bplayerlib_player_MYCPlayer_n_1start(JNIEnv *env, jobject instance) {

    if(mycfFmpeg == NULL) {
        return;
    }
    mycfFmpeg->start();
}