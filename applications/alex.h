#ifndef APPLICATIONS_ALEX_H_
#define APPLICATIONS_ALEX_H_

typedef struct __MsgData_t{
    rt_uint8_t from;    /* 消息发件人 */
    rt_uint8_t to;    /* 消息收件人 */
    void *data_ptr;    /* 消息数据结构指针 */
    rt_uint32_t data_size;   /* 数据块大小   */
} MsgData_t;
typedef struct __MsgData_t *MsgData_ptr;

typedef struct {
    uint16_t lux;    /* 光照值 */
} Gy30Data_t;

typedef enum TASK_Name {
    TASK_LED        = 0,
    TASK_GY30       = 1,
    TASK_OLED,
    TASK_RTC,

    TASK_CCOUNT
} TASK_Name;

#define EVENT_FLAG3 (1 << 3)
#define EVENT_FLAG5 (1 << 5)

/* 指向事件集的指针 */
extern rt_event_t rtc_event;

/* 消息队列控制块 */
extern struct rt_messagequeue mq;

#endif /* APPLICATIONS_MAIN_H_ */
