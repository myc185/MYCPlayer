//
// Created by Administrator on 2018/12/27.
//

#ifndef MYCPLAYER_MYCJAVACALLBACK_H
#define MYCPLAYER_MYCJAVACALLBACK_H

#include <cwchar>
#include "jni.h"
#include "AndroidLog.h"

#define THREAD_MAIN 0
#define THREAD_CHILD 1

class MYCJavaCallback {

public:

    JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;
    jmethodID jmid_prepared;

    jmethodID  jmid_load;


public:

    MYCJavaCallback(JavaVM *jvm, JNIEnv *env, jobject *obj);
    ~MYCJavaCallback();

    void onCallPrepared(int type);

    void onCallLoad(int type, bool load);


};


#endif //MYCPLAYER_MYCJAVACALLBACK_H
