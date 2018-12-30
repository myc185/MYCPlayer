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

    this->jmid_prepared = env->GetMethodID(jclz, "onCallPrepared", "()V");
    this->jmid_load = env->GetMethodID(jclz, "onCallLoad", "(Z)V");
    this->jmid_timeinfo = env->GetMethodID(jclz, "onCallTimeInfo", "(II)V");
    this->jmid_error = env->GetMethodID(jclz, "onCallError", "(ILjava/lang/String;)V");


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

void MYCJavaCallback::onCallLoad(int type, bool load) {
    if (type == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jobj, jmid_load, load);
    } else if (type == THREAD_CHILD) {
        JNIEnv *jniEn;
        if (javaVM->AttachCurrentThread(&jniEn, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("Get child thread jnienv error");
            }
            return;
        }
        jniEn->CallVoidMethod(jobj, jmid_load, load);
        javaVM->DetachCurrentThread();
    }

}

void MYCJavaCallback::onCallTimeInfo(int type, int current, int total) {
    if (type == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jobj, jmid_timeinfo, current, total);
    } else if (type == THREAD_CHILD) {
        JNIEnv *jniEn;
        if (javaVM->AttachCurrentThread(&jniEn, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("Get child thread jnienv error");
            }
            return;
        }
        jniEn->CallVoidMethod(jobj, jmid_timeinfo, current, total);
        javaVM->DetachCurrentThread();
    }

}

void MYCJavaCallback::onCallError(int type, int code, char *msg) {
    if (type == THREAD_MAIN) {
        jstring jmsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmid_error, code, jmsg);
        jniEnv->DeleteLocalRef(jmsg);

    } else if (type == THREAD_CHILD) {
        JNIEnv *jniEn;
        if (javaVM->AttachCurrentThread(&jniEn, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("Get child thread jnienv error");
            }
            return;
        }
        jstring jmsg = jniEn->NewStringUTF(msg);
        jniEn->CallVoidMethod(jobj, jmid_error, code, jmsg);
        jniEn->DeleteLocalRef(jmsg);
        javaVM->DetachCurrentThread();
    }
}
