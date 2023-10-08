/*
** 程序清单：这是一个 RTC 设备使用例程
** 程序功能：设置RTC时间，创建闹钟，模式：每秒触发，启动闹钟
**/

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define DBG_TAG "alarm_sample"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#define THREAD_PRIORITY         18
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        10

static rt_thread_t tid = RT_NULL;

#define BLUE_LED_PIN        GET_PIN(B, 5)

void user_alarm_callback(rt_alarm_t alarm, time_t timestamp)
{
    static rt_uint8_t pin_statu = 0;
//    rt_kprintf("user alarm callback function.\n");
    if (pin_statu == 0)
    {
        rt_pin_write(BLUE_LED_PIN, PIN_LOW);
        pin_statu = 1;
    }
    else {
        rt_pin_write(BLUE_LED_PIN, PIN_HIGH);
        pin_statu = 0;
    }
}

static void alarm_sample(void *parameter)
{
    rt_device_t device = RT_NULL;

    device = rt_device_find("rtc");
    if (!device)
    {
      LOG_E("find RTC failed!");
      return ;
    }

    struct rt_alarm_setup setup;
    struct rt_alarm * alarm = RT_NULL;
    static time_t now;
    struct tm p_tm;

    if (alarm != RT_NULL)
        return;

    /* 获取当前时间戳，并把下一秒时间设置为闹钟时间 */
    now = time(NULL) + 3;
    gmtime_r(&now,&p_tm);

    setup.flag = RT_ALARM_MINUTE;
    setup.wktime.tm_year = p_tm.tm_year;
    setup.wktime.tm_mon = p_tm.tm_mon;
    setup.wktime.tm_mday = p_tm.tm_mday;
    setup.wktime.tm_wday = p_tm.tm_wday;
    setup.wktime.tm_hour = p_tm.tm_hour;
    setup.wktime.tm_min = p_tm.tm_min;
    setup.wktime.tm_sec = p_tm.tm_sec;

    alarm = rt_alarm_create(user_alarm_callback, &setup);
    if(RT_NULL != alarm)
    {
        rt_alarm_start(alarm);
    }
}

/* 线程的初始化 */
int alarm_sample_start(void)
{
    /* 创建线程，名称是thread，入口是thread_entry */
    tid = rt_thread_create("alarm",
                            alarm_sample,
                            RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY,
                            THREAD_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程  */
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    else
        LOG_E("thread [%s] create failed");

    return RT_EOK;
}
