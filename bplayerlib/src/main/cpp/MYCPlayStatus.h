//
// Created by Administrator on 2018/12/28.
//

#ifndef MYCPLAYER_MYCPLAYSTATUS_H
#define MYCPLAYER_MYCPLAYSTATUS_H


class MYCPlayStatus {

public:

    bool exit;
    bool load = true;
    bool seek = false;
    bool pause = false;

public:
    MYCPlayStatus();

    ~MYCPlayStatus();


};


#endif //MYCPLAYER_MYCPLAYSTATUS_H
