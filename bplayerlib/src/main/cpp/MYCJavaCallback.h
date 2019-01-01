//
// Created by Administrator on 2018/12/27.
//

#ifndef MYCPLAYER_MYCJAVACALLBACK_H
#define MYCPLAYER_MYCJAVACALLBACK_H


#include "jni.h"
#include <linux/stddef.h>
#include "AndroidLog.h"

#define THREAD_MAIN 0
#define THREAD_CHILD 1

class MYCJavaCallback {

public:

    JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;
    jmethodID jmid_prepared;
    jmethodID jmid_load;
    jmethodID jmid_timeinfo;
    jmethodID jmid_error;
    jmethodID jmid_complete;

    jmethodID jmid_renderyuv;


public:

    MYCJavaCallback(JavaVM *jvm, JNIEnv *env, jobject *obj);

    ~MYCJavaCallback();

    void onCallPrepared(int type);

    void onCallLoad(int type, bool load);

    void onCallTimeInfo(int type, int current, int total);

    void onCallError(int type, int code, char *smg);

    void onComplete(int type);

    void onCallRenderYUV(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv);

};


#endif //MYCPLAYER_MYCJAVACALLBACK_H
