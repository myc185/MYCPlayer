/**
 * Copyright (C), 2017-2019, BOSMA-广州博冠智能科技有限公司
 * FileName: YCVIdeoSupporttUtil
 * Author: Administrator
 * Date: 2019/01/03 22:36
 * Description: 时间格式化辅助类
 * History:
 * <author> <time> <version> <desc>
 * 作者姓名 修改时间 版本号 描述
 */
package com.bosma.bplayerlib.util;

import android.media.MediaCodecList;

import java.util.HashMap;
import java.util.Map;

/**
 * @ClassName: YCVIdeoSupporttUtil
 * @Description: 类作用描述
 * @Author: Administrator
 * @Date: 2019/01/03 22:36
 */
public class YCVIdeoSupporttUtil {

    private static Map<String, String> codecMap = new HashMap<>();

    static {
        codecMap.put("h264", "video/avc");
    }

    public static String findVideoCodecName(String ffcodename) {
        if (codecMap.containsKey(ffcodename)) {
            return codecMap.get(ffcodename);
        }
        return "";
    }

    public static boolean isSupportCodec(String ffcodecname) {
        boolean supportvideo = false;
        int count = MediaCodecList.getCodecCount();
        for (int i = 0; i < count; i++) {
            String[] tyeps = MediaCodecList.getCodecInfoAt(i).getSupportedTypes();
            for (int j = 0; j < tyeps.length; j++) {
                if (tyeps[j].equals(findVideoCodecName(ffcodecname))) {
                    supportvideo = true;
                    break;
                }
            }
            if (supportvideo) {
                break;
            }
        }
        return supportvideo;
    }
}
