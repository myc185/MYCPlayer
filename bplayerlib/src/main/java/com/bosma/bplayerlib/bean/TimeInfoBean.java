/**
 * Copyright (C), 2017-2018, BOSMA-广州博冠智能科技有限公司
 * FileName: TimeInfoBean
 * Author: Administrator
 * Date: 2018/12/30 11:09
 * Description: 播放时间信息
 * History:
 * <author> <time> <version> <desc>
 * 作者姓名 修改时间 版本号 描述
 */
package com.bosma.bplayerlib.bean;

/**
 * @ClassName: TimeInfoBean
 * @Description: 类作用描述
 * @Author: Administrator
 * @Date: 2018/12/30 11:09
 */
public class TimeInfoBean {

    private int currentTime;
    private int totalTime;

    public int getCurrentTime() {
        return currentTime;
    }

    public void setCurrentTime(int currentTime) {
        this.currentTime = currentTime;
    }

    public int getTotalTime() {
        return totalTime;
    }

    public void setTotalTime(int totalTime) {
        this.totalTime = totalTime;
    }

    @Override
    public String toString() {
        return "TimeInfoBean{" +
                "currentTime=" + currentTime +
                ", totalTime=" + totalTime +
                '}';
    }
}