/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-09-20     RT-Thread    first version
 */

#include <rtthread.h>
#include "led_sample.h"
#include "alex.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* 指向事件集的指针 */
rt_event_t rtc_event = RT_NULL;

/* 消息队列控制块 */
struct rt_messagequeue mq;
/* 消息队列中用到的放置消息的内存池 */
rt_uint8_t msg_pool[1024];

int main(void)
{
    rt_err_t    result;

    /* 创建一个事件集 */
    rtc_event = rt_event_create("rtc_event", RT_IPC_FLAG_PRIO);

    /* 初始化消息队列 */
    result = rt_mq_init(&mq,
                        "mqt",
                        &msg_pool[0],             /* 内存池指向 msg_pool */
                        sizeof(MsgData_t),          /* 每个消息的大小是 1 字节 */
                        sizeof(msg_pool),        /* 内存池的大小是 msg_pool 的大小 */
                        RT_IPC_FLAG_PRIO);       /* 如果有多个线程等待，优先级大小的方法分配消息 */

    if (result != RT_EOK)
    {
        LOG_E("init message queue failed.");
    }

    /* MSH stack size 1024 */

    /* 启动LED线程,优先级25 */
    led_sample_start();
//    /* 启动oled线程,优先级23  */
//    oled_sample_start();
//    /* 启动RTC线程,优先级18 */
//    rtc_sample_start();
//    /* 启动光传感器线程,优先级21 */
//    gy30_sample_start();

    return RT_EOK;
}
