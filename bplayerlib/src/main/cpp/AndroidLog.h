//
// Created by Administrator on 2018/12/26.
//

#ifndef JNITHREAD_ANDROIDLOG_H
#define JNITHREAD_ANDROIDLOG_H

#endif //JNITHREAD_ANDROIDLOG_H

#include "android/log.h"

#define LOG_DEBUG true

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO,"Bplayer",FORMAT,##__VA_ARGS__);
#define LOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG,"Bplayer",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"Bplayer",FORMAT,##__VA_ARGS__);
