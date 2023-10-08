#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define DBG_TAG "led_sample"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       256
#define THREAD_TIMESLICE        5

static rt_thread_t tid = RT_NULL;

#define ORANGE_LED_PIN      GET_PIN(B, 3)
#define GREEN_LED_PIN       GET_PIN(B, 4)
#define BLUE_LED_PIN        GET_PIN(B, 5)

/* 线程的入口函数 */
static void led_entry(void *parameter)
{
    rt_base_t led_pin_num = 0;
    /* 获取引脚编号 */
    led_pin_num = GREEN_LED_PIN;
    LOG_D("LED pin num is: %d", led_pin_num);
    /* 引脚为输出模式 */
    rt_pin_mode(led_pin_num, PIN_MODE_OUTPUT);
    rt_pin_mode(BLUE_LED_PIN, PIN_MODE_OUTPUT);
    /* 默认低电平(LED OFF) */
    rt_pin_write(led_pin_num, PIN_LOW);

    while (1)
    {
        /* 设置低电平(LED OFF) */
        rt_pin_write(led_pin_num, PIN_LOW);
        LOG_D("LED OFF");
        rt_thread_mdelay(1000);

        /* 设置高电平(LED ON) */
        rt_pin_write(led_pin_num, PIN_HIGH);
        LOG_D("LED ON");
        rt_thread_mdelay(1000);
    }
}

/* 线程的初始化 */
int led_sample_start(void)
{
    /* 创建线程，名称是thread，入口是thread_entry */
    tid = rt_thread_create("led",
                            led_entry,
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
