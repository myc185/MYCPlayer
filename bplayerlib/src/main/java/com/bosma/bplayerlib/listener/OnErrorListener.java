/**
 * Copyright (C), 2017-2018, BOSMA-广州博冠智能科技有限公司
 * FileName: OnErrorListener
 * Author: Administrator
 * Date: 2018/12/30 8:41
 * Description: 播放状态回调
 * History:
 * <author> <time> <version> <desc>
 * 作者姓名 修改时间 版本号 描述
 */
package com.bosma.bplayerlib.listener;

/**
 * @ClassName: OnErrorListener
 * @Description: 类作用描述
 * @Author: Administrator
 * @Date: 2018/12/30 8:41
 */
public interface OnErrorListener {

    void onError(int code, String msg);
}
