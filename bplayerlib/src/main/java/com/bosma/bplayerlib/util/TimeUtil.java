/**
 * Copyright (C), 2017-2018, BOSMA-广州博冠智能科技有限公司
 * FileName: TimeUtil
 * Author: Administrator
 * Date: 2018/12/30 11:36
 * Description: 时间格式化辅助类
 * History:
 * <author> <time> <version> <desc>
 * 作者姓名 修改时间 版本号 描述
 */
package com.bosma.bplayerlib.util;

/**
 * @ClassName: TimeUtil
 * @Description: 类作用描述
 * @Author: Administrator
 * @Date: 2018/12/30 11:36
 */
public class TimeUtil {

    /**
     * format times
     *
     * @param secds
     * @param totalsecds
     * @return
     */
    public static String secdsToDateFormat(int secds, int totalsecds) {
        long hours = secds / (60 * 60);
        long minutes = (secds % (60 * 60)) / (60);
        long seconds = secds % (60);

        String sh = "00";
        if (hours > 0) {
            if (hours < 10) {
                sh = "0" + hours;
            } else {
                sh = hours + "";
            }
        }
        String sm = "00";
        if (minutes > 0) {
            if (minutes < 10) {
                sm = "0" + minutes;
            } else {
                sm = minutes + "";
            }
        }

        String ss = "00";
        if (seconds > 0) {
            if (seconds < 10) {
                ss = "0" + seconds;
            } else {
                ss = seconds + "";
            }
        }
        if (totalsecds >= 3600) {
            return sh + ":" + sm + ":" + ss;
        }
        return sm + ":" + ss;

    }
}