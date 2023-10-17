/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-16     misonyo      first implementation.
 */
/*
 * 程序清单：这是一个串口设备 DMA 接收使用例程
 * 例程导出了 uart_dma_sample 命令到控制终端
 * 命令调用格式：uart_dma_sample uart3
 * 命令解释：命令第二个参数是要使用的串口设备名称，为空则使用默认的串口设备
 * 程序功能：通过串口输出字符串"hello RT-Thread!"，并通过串口输出接收到的数据，然后打印接收到的数据。
*/

#include <rtthread.h>
#include "alex.h"

#define DBG_TAG "uart_dma_sample"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#define THREAD_PRIORITY         16
#define THREAD_STACK_SIZE       2048
#define THREAD_TIMESLICE        10

static rt_thread_t tid1 = RT_NULL;

#define SAMPLE_UART_NAME       "uart1"      /* 串口设备名称 */

/* 串口设备句柄 */
static rt_device_t serial;

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    send_msg_t msg;
    rt_err_t result;
    rt_uint16_t rx_length, putsize;

    static rt_uint8_t rx_buffer[RT_SERIAL_RB_BUFSZ];
    rx_length = rt_device_read(dev, 0, rx_buffer, size);
    LOG_HEX("UART-RX", 16, rx_buffer, size);

    LOG_D("write ringbuffer size: %d", rx_length);
    putsize = rt_ringbuffer_put(lora_tx_rb, (rt_uint8_t *)rx_buffer, rx_length);
    if (putsize != rx_length)
    {
        LOG_E("lora tx ringbuffer full!");
        msg.size = putsize;
    }
    else
    {
        msg.size = rx_length;
    }
    msg.dev = dev;
    msg.type = MSG_TYPE_DATA;

    LOG_D("msg data size: %d", msg.size);
    result = rt_mq_send(&lora_tx_mq, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
        LOG_E("Lora TX message queue full!");
    }
    return result;
}

static void serial_thread_entry(void *parameter)
{
    send_msg_t msg;
    rt_err_t result;
    static rt_uint8_t tx_buffer[RT_SERIAL_RB_BUFSZ];

    while (1)
    {
        rt_memset(&msg, 0, sizeof(msg));
        result = rt_mq_recv(&uart_tx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            do
            {
                if (RT_SERIAL_RB_BUFSZ >= msg.size)
                {
                    rt_ringbuffer_get(uart_tx_rb, tx_buffer, msg.size);
                    /* Output messages through serial device */
                    LOG_HEX("UART-TX", 16, tx_buffer, msg.size);
                    rt_device_write(serial, 0, tx_buffer, msg.size);
                    msg.size = 0;
                }
                else
                {
                    rt_ringbuffer_get(uart_tx_rb, tx_buffer, RT_SERIAL_RB_BUFSZ);
                    /* Output messages through serial device */
                    LOG_HEX("UART-TX", 16, tx_buffer, RT_SERIAL_RB_BUFSZ);
                    rt_device_write(serial, 0, tx_buffer, RT_SERIAL_RB_BUFSZ);
                    msg.size= msg.size - RT_SERIAL_RB_BUFSZ;
                }
            } while(msg.size);
        }
    }
}

int uart_dma_sample_start(void)
{
    rt_err_t ret = RT_EOK;
//    static char str[] = "UART DMA Sample\r\n";

    /* 查找串口设备 */
    serial = rt_device_find(SAMPLE_UART_NAME);
    if (!serial)
    {
        LOG_E("find %s failed!\n", SAMPLE_UART_NAME);
        return RT_ERROR;
    }

    /* 以 DMA接收及DMA发送方式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);

    /* 创建 serial 线程 */
    tid1 = rt_thread_create("serial", serial_thread_entry,
                            RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY,
                            THREAD_TIMESLICE);
    /* 创建成功则启动线程 */
    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1);
    }
    else
    {
        ret = RT_ERROR;
    }

    return ret;
}
