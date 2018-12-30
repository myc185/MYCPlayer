/**
 * Copyright (C), 2017-2018, BOSMA-广州博冠智能科技有限公司
 * FileName: OnTimeInfoListener
 * Author: Administrator
 * Date: 2018/12/30 11:08
 * Description: 播放事件回调
 * History:
 * <author> <time> <version> <desc>
 * 作者姓名 修改时间 版本号 描述
 */
package com.bosma.bplayerlib.listener;

import com.bosma.bplayerlib.bean.TimeInfoBean;

/**
 * @ClassName: OnTimeInfoListener
 * @Description: 类作用描述
 * @Author: Administrator
 * @Date: 2018/12/30 11:08
 */
public interface OnTimeInfoListener {

    void onTImeInfo(TimeInfoBean timeInfoBean);


}