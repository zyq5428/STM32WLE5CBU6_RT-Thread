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
#include "alex.h"
#include "led_sample.h"
#include "uart_dma_sample.h"
#include "alarm_sample.h"
#include "subghz_phy_app.h"

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

/* 指向事件集的指针 */
rt_event_t rtc_event = RT_NULL;

/* 消息队列控制块 */
struct rt_messagequeue mq;
struct rt_messagequeue lora_tx_mq;
struct rt_messagequeue uart_tx_mq;

/* 消息队列中用到的放置消息的内存池 */
rt_uint8_t msg_pool[64];
rt_uint8_t rx_msg_pool[512];
rt_uint8_t tx_msg_pool[512];

/* 环形缓冲区 */
struct rt_ringbuffer *lora_tx_rb;
struct rt_ringbuffer *uart_tx_rb;
#define RING_BUFFER_LEN     1024

int main(void)
{
    rt_err_t    result;

    /* 创建一个事件集 */
    rtc_event = rt_event_create("rtc_event", RT_IPC_FLAG_PRIO);

    /* 初始化消息队列 */
    /* 初始化多任务消息队列 */
    result = rt_mq_init(&mq,
                        "mqt",
                        &msg_pool[0],             /* 内存池指向 msg_pool */
                        sizeof(MsgData_t),          /* 每个消息的大小是 1 字节 */
                        sizeof(msg_pool),        /* 内存池的大小是 msg_pool 的大小 */
                        RT_IPC_FLAG_PRIO);       /* 如果有多个线程等待，优先级大小的方法分配消息 */

    if (result != RT_EOK)
    {
        LOG_E("init task message queue failed.");
    }

    /* 初始化串口消息队列 */
    result = rt_mq_init(&lora_tx_mq,
                        "lora_tx_mq",
                        rx_msg_pool,                 /* 存放消息的缓冲区 */
                        sizeof(send_msg_t),    /* 一条消息的最大长度 */
                        sizeof(rx_msg_pool),         /* 存放消息的缓冲区大小 */
                        RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */
    if (result != RT_EOK)
    {
        LOG_E("init uart rx message queue failed.");
    }
    result = rt_mq_init(&uart_tx_mq,
                        "uart_tx_mq",
                        tx_msg_pool,                 /* 存放消息的缓冲区 */
                        sizeof(send_msg_t),    /* 一条消息的最大长度 */
                        sizeof(tx_msg_pool),         /* 存放消息的缓冲区大小 */
                        RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */
    if (result != RT_EOK)
    {
        LOG_E("init uart tx message queue failed.");
    }

    /* 初始化串口环形缓冲队列 */
    lora_tx_rb = rt_ringbuffer_create(RING_BUFFER_LEN);
    if (lora_tx_rb == RT_NULL)
    {
        LOG_E("Can't create ringbuffer");
    }
    uart_tx_rb = rt_ringbuffer_create(RING_BUFFER_LEN);
    if (uart_tx_rb == RT_NULL)
    {
        LOG_E("Can't create ringbuffer");
    }

    /* MSH stack size 1024 */

//    /* Start the LED thread, priority 25 */
//    led_sample_start();
    /* Start the uart_dma thread, priority 16 */
    uart_dma_sample_start();
//    /* 启动alarm线程,优先级18 */
//    alarm_sample_start();
//    /* 启动oled线程,优先级23  */
//    oled_sample_start();
//    /* 启动RTC线程,优先级18 */
//    rtc_sample_start();
//    /* 启动光传感器线程,优先级21 */
//    gy30_sample_start();
    /* Start the Subghz thread, priority 19 */
    SubghzApp_Init();

    return RT_EOK;
}
