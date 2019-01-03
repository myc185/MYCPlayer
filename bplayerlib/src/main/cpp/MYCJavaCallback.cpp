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
    this->jmid_complete = env->GetMethodID(jclz, "onCallComplete", "()V");
    this->jmid_renderyuv = env->GetMethodID(jclz, "onCallRenderYUV", "(II[B[B[B)V");
    this->jmid_supportvideo = env->GetMethodID(jclz, "onCallIsSupportMediaCodec",
                                               "(Ljava/lang/String;)Z");


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

void MYCJavaCallback::onComplete(int type) {
    if (type == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jobj, jmid_complete);

    } else if (type == THREAD_CHILD) {
        JNIEnv *jniEn;
        if (javaVM->AttachCurrentThread(&jniEn, 0) != JNI_OK) {
            if (LOG_DEBUG) {
                LOGE("Get child thread jnienv error");
            }
            return;
        }
        jniEn->CallVoidMethod(jobj, jmid_complete);
        javaVM->DetachCurrentThread();
    }


}

void
MYCJavaCallback::onCallRenderYUV(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv) {

    JNIEnv *jniEn;
    if (javaVM->AttachCurrentThread(&jniEn, 0) != JNI_OK) {
        if (LOG_DEBUG) {
            LOGE("Get child thread jnienv error");
        }
        return;
    }

    jbyteArray y = jniEn->NewByteArray(width * height);
    jniEn->SetByteArrayRegion(y, 0, width * height, (const jbyte *) (fy));

    jbyteArray u = jniEn->NewByteArray(width * height / 4);
    jniEn->SetByteArrayRegion(u, 0, width * height / 4, (const jbyte *) (fu));


    jbyteArray v = jniEn->NewByteArray(width * height / 4);
    jniEn->SetByteArrayRegion(v, 0, width * height / 4, (const jbyte *) (fv));


    jniEn->CallVoidMethod(jobj, jmid_renderyuv, width, height, y, u, v);
    jniEn->DeleteLocalRef(y);
    jniEn->DeleteLocalRef(u);
    jniEn->DeleteLocalRef(v);
    javaVM->DetachCurrentThread();


}

bool MYCJavaCallback::onCallIsSupportVideo(const char *ffcodename) {
    JNIEnv *jniEn;
    bool support = false;
    if (javaVM->AttachCurrentThread(&jniEn, 0) != JNI_OK) {
        if (LOG_DEBUG) {
            LOGE("Get child thread jnienv error");
        }
        return support;
    }

    jstring jtype = jniEn->NewStringUTF(ffcodename);
    support = jniEn->CallBooleanMethod(jobj, jmid_supportvideo, jtype);
    jniEn->DeleteLocalRef(jtype);
    javaVM->DetachCurrentThread();

    return support;
}
