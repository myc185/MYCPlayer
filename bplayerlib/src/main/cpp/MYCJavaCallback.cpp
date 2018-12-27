//
// Created by Administrator on 2018/12/27.
//

#include "MYCJavaCallback.h"

MYCJavaCallback::MYCJavaCallback(JavaVM *jvm, JNIEnv *env, jobject *obj) {

    this->javaVM = jvm;
    this->jniEnv = env;
    this->jobj = env->NewGlobalRef(*obj);

    jclass jclz = jniEnv->GetObjectClass(jobj);
    if (!jclz) {
        if (LOG_DEBUG) {
            LOGE("get prepared jclass wrong")
        }
        return;
    }

    jmid_prepared = env->GetMethodID(jclz, "onCallPrepared", "()V");


}

MYCJavaCallback::~MYCJavaCallback() {

}

void MYCJavaCallback::onCallPrepared(int type) {
    if (type == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jobj, jmid_prepared);
    } else if (type == THREAD_CHILD) {
        JNIEnv *jniEn;
        if (javaVM->AttachCurrentThread(&jniEn, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("Get child thread jnienv error");
            }
            return;
        }
        jniEn->CallVoidMethod(jobj, jmid_prepared);
        javaVM->DetachCurrentThread();
    }


}
