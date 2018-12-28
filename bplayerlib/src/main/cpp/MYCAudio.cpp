//
// Created by Administrator on 2018/12/27.
//

#include "MYCAudio.h"


MYCAudio::MYCAudio(MYCPlayStatus *playStatus) {
    this->playStatus = playStatus;
    this->queue = new MYCQueue(playStatus);

}

MYCAudio::~MYCAudio() {

}

