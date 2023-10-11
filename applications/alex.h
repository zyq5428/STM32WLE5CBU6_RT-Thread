#ifndef APPLICATIONS_ALEX_H_
#define APPLICATIONS_ALEX_H_

#include <rtdevice.h>

#define MSG_TYPE_DATA   0x00
#define MSG_TYPE_CFG    0x01

/* 发送消息数据结构 */
typedef struct __send_msg
{
    /* What device does the data come from? */
    rt_device_t dev;
    /*
     * message type
     * data
     * configuration instructions
     */
    rt_uint8_t  type;
    /*
     * valid information
     * data size
     * or
     * instruction
     */
    rt_size_t size;
} send_msg_t;

/* 发送消息数据结构 */
typedef struct __receive_msg
{
    /*
     * message type
     * DATA/CFG/ACK/TRS
     * configuration instructions
     */
    rt_uint8_t  type;
    /*
     * valid information
     * data size
     * or
     * instruction
     */
    rt_size_t size;
} receive_msg;

#define LORA_TYPE_DATA   0x00
#define LORA_TYPE_CFG    0x01
#define LORA_TYPE_ACK    0x02
#define LORA_TYPE_TRS    0x04
#define LORA_PAYLOAD_LEN 0x20

typedef enum LORA_DATA_TYPE {
    LORA_DATA_SEND = 0,
    LORA_DATA_RESEND = 1
} LORA_DATA_TYPE;

/* lora data packet */
typedef struct __lora_data_pkt
{
    /* destination address */
    rt_uint8_t dst_addr;
    /* source address */
    rt_uint8_t src_addr;
    /* transit address */
    rt_uint8_t mid_addr;
    /* type of data
     * data
     * configuration instructions
     * ACK
     */
    rt_uint8_t type;
    /* data total length */
    rt_uint8_t total_len;
    /* data actual length */
    rt_uint8_t len;
    /* type of data */
    rt_uint8_t payload[LORA_PAYLOAD_LEN];
} lora_data_pkt_t;
#define LORA_DATA_HEADER_LEN 0x06

#define LORA_ACK_OK             0x00
#define LORA_ACK_DATA_ERROR     0x01
#define LORA_ACK_CFG_ERROR      0x02
#define LORA_ACK_REQUEST_SEND   0x04
#define LORA_ACK_RX_FULL        0x08

/* lora ack packet */
typedef struct __lora_ack_pkt
{
    /* destination address */
    rt_uint8_t dst_addr;
    /* source address */
    rt_uint8_t src_addr;
    /* transit address */
    rt_uint8_t mid_addr;
    /* type of data */
    rt_uint8_t type;
    /* data length */
    rt_uint8_t ack;
} lora_ack_pkt_t;
#define LORA_ACK_LEN 0x05

typedef struct __MsgData_t{
    rt_uint8_t from;    /* 消息发件人 */
    rt_uint8_t to;    /* 消息收件人 */
    void *data_ptr;    /* 消息数据结构指针 */
    rt_uint32_t data_size;   /* 数据块大小   */
} MsgData_t;
typedef struct __MsgData_t *MsgData_ptr;

typedef enum LED_Name {
    RED     = 0,
    GREEN   = 1,
    BLUE,
    ORANGE,

    LED_COUNT
} LED_Name;

typedef struct {
    uint16_t lux;    /* 光照值 */
} Gy30Data_t;

typedef enum TASK_Name {
    TASK_LED        = 0,
    TASK_GY30       = 1,
    TASK_OLED,
    TASK_RTC,

    TASK_COUNT
} TASK_Name;

#define EVENT_FLAG3 (1 << 3)
#define EVENT_FLAG5 (1 << 5)

/* 指向事件集的指针 */
rt_event_t rtc_event;

/* 消息队列控制块 */
extern struct rt_messagequeue mq;
extern struct rt_messagequeue lora_tx_mq;
extern struct rt_messagequeue uart_tx_mq;

/* 消息队列中用到的放置消息的内存池 */
//rt_uint8_t msg_pool[32];
//rt_uint8_t rx_msg_pool[32];
//rt_uint8_t tx_msg_pool[32];

/* 环形缓冲区 */
extern struct rt_ringbuffer *lora_tx_rb;
extern struct rt_ringbuffer *uart_tx_rb;

#endif /* APPLICATIONS_MAIN_H_ */
