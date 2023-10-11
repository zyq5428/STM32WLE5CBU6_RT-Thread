

/* Includes ------------------------------------------------------------------*/
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "subghz_phy_app.h"
#include "radio.h"
#include "alex.h"

#define DBG_TAG "subghz"
#define DBG_LVL DBG_WARNING
#include <rtdbg.h>

/* USER CODE BEGIN Includes */
#include "lora-radio-timer.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
#define THREAD_PRIORITY         19
#define THREAD_STACK_SIZE       8096
#define THREAD_TIMESLICE        10

// Ping pong event
#define EV_RADIO_INIT            0x0001
#define EV_RADIO_TX_START        0x0002
#define EV_RADIO_TX_DONE         0x0004
#define EV_RADIO_TX_TIMEOUT      0x0008
#define EV_RADIO_RX_DONE         0x0010
#define EV_RADIO_RX_TIMEOUT      0x0020
#define EV_RADIO_RX_ERROR        0x0040
#define EV_RADIO_ALL             (EV_RADIO_INIT \
        | EV_RADIO_TX_START | EV_RADIO_TX_DONE | EV_RADIO_TX_TIMEOUT \
        | EV_RADIO_RX_DONE | EV_RADIO_RX_TIMEOUT | EV_RADIO_RX_ERROR)

/* USER CODE BEGIN PTD */
typedef enum
{
  RX,
  RX_TIMEOUT,
  RX_ERROR,
  TX,
  TX_TIMEOUT,
} States_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED_ORANGE      GET_PIN(B, 3)
#define LED_RED         LED_ORANGE
#define LED_GREEN       GET_PIN(B, 4)
#define LED_BLUE        GET_PIN(B, 5)

/* Configurations */
/*Timeout*/
#define RX_TIMEOUT_VALUE              3000
#define TX_TIMEOUT_VALUE              3000

/*Size of the payload to be sent*/
/* Size must be greater of equal the PING and PONG*/
#define MAX_APP_BUFFER_SIZE          255
#if (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE)
#error PAYLOAD_LEN must be less or equal than MAX_APP_BUFFER_SIZE
#endif /* (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE) */
/* wait for remote to be in Rx, before sending a Tx frame*/
#define RX_TIME_MARGIN                200
/* Afc bandwidth in Hz */
#define FSK_AFC_BANDWIDTH             83333
/* LED blink Period*/
#define LED_PERIOD_MS                 200
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;
/* USER CODE BEGIN PV */
static struct rt_event radio_event;
/*Ping Pong FSM states */
static States_t State = RX;
/* App Rx Buffer*/
//static uint8_t BufferRx[MAX_APP_BUFFER_SIZE];
/* App Tx Buffer*/
//static uint8_t BufferTx[MAX_APP_BUFFER_SIZE];
/* Lora RX messagequeue */
static struct rt_messagequeue rx_mq;
/* messagequeue pool */
static rt_uint8_t msg_pool[32];
/* Lora RX ringbuffer */
static struct rt_ringbuffer *rx_rb;
#define LORA_RING_BUFFER_LEN 128

/* App Tx packet*/
static lora_data_pkt_t lora_tx_data;
/* App ACK packet*/
//static lora_ack_pkt_t lora_tx_ack;
static lora_ack_pkt_t lora_rx_ack;
/* App Rx packet*/
static lora_data_pkt_t lora_rx_data;
/* Lora data variable*/
static send_msg_t lora_tx_msg;
static send_msg_t lora_rx_msg;
//bool ack_check_enable = true;
bool ack_check_enable = false;
bool last_tx_isOK = true;
bool transfer_enable = false;
static rt_uint8_t wait_send_len = 0;
/* Last  Received Buffer Size*/
//uint16_t RxBufferSize = 0;
/* Last  Received packer Rssi*/
int8_t RssiValue = 0;
/* Last  Received packer SNR (in Lora modulation)*/
int8_t SnrValue = 0;
/* Led Timers objects*/
static rtick_timer_event_t timerLed;

/* random delay to make sure 2 devices will sync*/
/* the closest the random delays are, the longer it will
   take for the devices to sync when started simultaneously*/
static int32_t random_delay;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief Function to be executed on Radio Tx Done event
 */
static void OnTxDone(void);

/**
  * @brief Function to be executed on Radio Rx Done event
  * @param  payload ptr of buffer received
  * @param  size buffer size
  * @param  rssi
  * @param  LoraSnr_FskCfo
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
  * @brief Function executed on Radio Tx Timeout event
  */
static void OnTxTimeout(void);

/**
  * @brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/**
  * @brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/* USER CODE BEGIN PFP */
/**
  * @brief  Function executed on when led timer elapses
  * @param  context ptr of LED context
  */
static void send_ack(rt_uint8_t target, rt_uint8_t ack);

/**
  * @brief  Function executed on when led timer elapses
  * @param  context ptr of LED context
  */
static void OnledEvent(void);

/**
  * @brief PingPong state machine implementation
  */
static void lora_app_thread_entry(void* parameter);
/* USER CODE END PFP */

/*!
 * lora radio test thread
 */
static rt_thread_t lora_radio_thread = RT_NULL;

/* Exported functions ---------------------------------------------------------*/
int SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */
  LOG_I( "PING PONG");
  //hw_rtc_init();
  /* Print APP version*/
  LOG_I("APP_VERSION= V%X.%X.%X",
          (uint8_t)(APP_VERSION_MAIN),
          (uint8_t)(APP_VERSION_SUB1),
          (uint8_t)(APP_VERSION_SUB2));

  /* Led Timers*/
  rt_pin_mode(LED_GREEN, PIN_MODE_OUTPUT);
  rt_pin_mode(LED_RED, PIN_MODE_OUTPUT);
  rt_pin_write(LED_GREEN, PIN_LOW);
  rt_pin_write(LED_RED, PIN_HIGH);

  TimerInit(&timerLed, OnledEvent);
  TimerSetValue(&timerLed, LED_PERIOD_MS);
  TimerStart(&timerLed);
  /* USER CODE END SubghzApp_Init_1 */
  rt_event_init(&radio_event, "ev_lora", RT_IPC_FLAG_FIFO);
  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

  /* USER CODE BEGIN SubghzApp_Init_2 */
  /*calculate random delay for synchronization*/
  random_delay = (Radio.Random()) >> 22; /*10bits random e.g. from 0 to 1023 ms*/

  /* Radio Set frequency */
  Radio.SetChannel(RF_FREQUENCY);

  /* Radio configuration */
#if ((USE_MODEM_LORA == 1) && (USE_MODEM_FSK == 0))
  LOG_I("---------------");
  LOG_I("LORA_MODULATION");
  LOG_I("LORA_BW=%d kHz", (1 << LORA_BANDWIDTH) * 125);
  LOG_I("LORA_SF=%d", LORA_SPREADING_FACTOR);

  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  Radio.SetMaxPayloadLength(MODEM_LORA, MAX_APP_BUFFER_SIZE);

#elif ((USE_MODEM_LORA == 0) && (USE_MODEM_FSK == 1))
  LOG_I("---------------");
  LOG_I("FSK_MODULATION");
  LOG_I("FSK_BW=%d Hz", FSK_BANDWIDTH);
  LOG_I("FSK_DR=%d bits/s", FSK_DATARATE);

  Radio.SetTxConfig(MODEM_FSK, TX_OUTPUT_POWER, FSK_FDEV, 0,
                    FSK_DATARATE, 0,
                    FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, 0, TX_TIMEOUT_VALUE);

  Radio.SetRxConfig(MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE,
                    0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
                    0, FSK_FIX_LENGTH_PAYLOAD_ON, 0, true,
                    0, 0, false, true);

  Radio.SetMaxPayloadLength(MODEM_FSK, MAX_APP_BUFFER_SIZE);

#else
#error "Please define a modulation in the subghz_phy_app.h file."
#endif /* USE_MODEM_LORA | USE_MODEM_FSK */

  /*fills tx buffer*/
//  rt_memset(BufferTx, 0x0, MAX_APP_BUFFER_SIZE);

  LOG_I("rand=%d", random_delay);
  /*starts reception*/
  Radio.Rx(RX_TIMEOUT_VALUE + random_delay);

  rt_err_t result;
  result = rt_mq_init(&rx_mq,
                      "rx_mq",
                      msg_pool,                 /* 存放消息的缓冲区 */
                      sizeof(receive_msg),    /* 一条消息的最大长度 */
                      sizeof(msg_pool),         /* 存放消息的缓冲区大小 */
                      RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */
  if (result != RT_EOK)
  {
      LOG_E("init lora rx message queue failed.");
  }

  rx_rb = rt_ringbuffer_create(LORA_RING_BUFFER_LEN);
  if (rx_rb == RT_NULL)
  {
      LOG_E("Can't create ringbuffer");
  }

  /*register task to to be run in while(1) after Radio IT*/
  lora_radio_thread = rt_thread_create("lora_app",
                                          lora_app_thread_entry,
                                          RT_NULL,
                                          THREAD_STACK_SIZE,
                                          THREAD_PRIORITY,
                                          THREAD_TIMESLICE);
  if (lora_radio_thread != RT_NULL)
  {
      rt_thread_startup(lora_radio_thread);
  }
  else
        LOG_E("lora radio test thread create failed!\n");
  /* USER CODE END SubghzApp_Init_2 */
  return 0;
}

/* USER CODE BEGIN EF */
//MSH_CMD_EXPORT(SubghzApp_Init,SubghzApp Shell Cmd);
/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/

static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
  LOG_D("OnTxDone");
  /* Update the State of the FSM*/
  State = TX;
  /* Run PingPong process in background*/
  rt_event_send(&radio_event, EV_RADIO_TX_DONE);
  /* USER CODE END OnTxDone */
}

static void put_rx_rb(uint8_t *payload, uint16_t size, receive_msg rx_msg)
{
    rt_size_t rx_size = 0;
    if (rx_msg.type != LORA_TYPE_TRS)
    {
        rx_msg.type = payload[3];
    }
    rx_size = rt_ringbuffer_get_size(rx_rb) - rt_ringbuffer_data_len(rx_rb);
    if (rx_size >= size)
    {
        rx_size = rt_ringbuffer_put(rx_rb, payload, size);
        rt_err_t result;
        rx_msg.size = rx_size;
        result = rt_mq_send(&rx_mq, &rx_msg, sizeof(rx_msg));
        if (result == -RT_EFULL)
        {
            rt_pin_write(LED_GREEN, PIN_HIGH);
            rt_pin_write(LED_ORANGE, PIN_HIGH);
        }
        else
        {
            /* Update the State of the FSM*/
            State = RX;
        }
    }
    else
    {
        State = RX_ERROR;
    }
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */
  LOG_I("OnRxDone");
#if ((USE_MODEM_LORA == 1) && (USE_MODEM_FSK == 0))
  LOG_I("RssiValue=%d dBm, SnrValue=%ddB", rssi, LoraSnr_FskCfo);
  /* Record payload Signal to noise ratio in Lora*/
  SnrValue = LoraSnr_FskCfo;
#endif /* USE_MODEM_LORA | USE_MODEM_FSK */
#if ((USE_MODEM_LORA == 0) && (USE_MODEM_FSK == 1))
  LOG_D("RssiValue=%d dBm, Cfo=%dkHz", rssi, LoraSnr_FskCfo);
  SnrValue = 0; /*not applicable in GFSK*/
#endif /* USE_MODEM_LORA | USE_MODEM_FSK */
  /* Record Received Signal Strength*/
  RssiValue = rssi;
  /* Record payload content*/
  LOG_I("payload. size=%d ", size);

  //  LOG_HEX("RX", 16, BufferRx, size);
    for (int i = 0; i < 8; i++)
    {
      LOG_I("%02X", payload[i]);
    }

  /* copy data to ringbuffer*/
  receive_msg rx_msg;
  if ((payload[0] == 0xA5) || (payload[0] == 0xFF))
  {
      put_rx_rb(payload, size, rx_msg);
  }
  else
  {
      if (transfer_enable)
      {
          rx_msg.type = LORA_TYPE_TRS;
          put_rx_rb(payload, size, rx_msg);
      }
      else
      {
          State = RX_ERROR;
      }
  }

  /* Run PingPong process in background*/
  rt_event_send(&radio_event, EV_RADIO_RX_DONE);
  /* USER CODE END OnRxDone */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
  LOG_I("OnTxTimeout");
  /* Update the State of the FSM*/
  State = TX_TIMEOUT;
  /* Run PingPong process in background*/
  rt_event_send(&radio_event, EV_RADIO_TX_TIMEOUT);
  /* USER CODE END OnTxTimeout */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */
  LOG_D("OnRxTimeout");
  /* Update the State of the FSM*/
  State = RX_TIMEOUT;
  /* Run PingPong process in background*/
  rt_event_send(&radio_event, EV_RADIO_RX_TIMEOUT);
  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
  LOG_D("OnRxError");
  /* Update the State of the FSM*/
  State = RX_ERROR;
  /* Run PingPong process in background*/
  rt_event_send(&radio_event, EV_RADIO_RX_ERROR);
  /* USER CODE END OnRxError */
}

static void set_tx_data(void)
{
    lora_tx_data.dst_addr = TARGET_ADDRESS;
    lora_tx_data.src_addr = SOURCE_ADDRESS;
    lora_tx_data.mid_addr = 0x00;
}

static void send_last_data(void)
{
    for (int i = 0; i < 8; i++)
    {
        LOG_I("%02X", ((rt_uint8_t *)&lora_tx_data)[i]);
    }
    Radio.Send((uint8_t *)&lora_tx_data, LORA_DATA_HEADER_LEN + lora_tx_data.len);
}

static void send_ready_data(void)
{
    rt_uint8_t size = 0;
    if (wait_send_len <= LORA_PAYLOAD_LEN)
    {
        lora_tx_data.len = wait_send_len;
    }
    else
    {
        lora_tx_data.len = LORA_PAYLOAD_LEN;
    }
    size = (rt_uint8_t)(rt_ringbuffer_get(lora_tx_rb, lora_tx_data.payload, lora_tx_data.len));
    if (lora_tx_data.len != size)
    {
        LOG_E("From lora_tx_rb get data error");
        lora_tx_data.len = size;
    }
    wait_send_len = wait_send_len - lora_tx_data.len;

    LOG_I("Tx start");
    LOG_I("send payload size=%d", lora_tx_data.len);
    for (int i = 0; i < 8; i++)
    {
        LOG_I("%02X", ((rt_uint8_t *)&lora_tx_data)[i]);
    }
    Radio.Send((uint8_t *)&lora_tx_data, LORA_DATA_HEADER_LEN + lora_tx_data.len);
}

static void send_ack(rt_uint8_t target, rt_uint8_t ack)
{
    lora_tx_data.dst_addr = target;
    lora_tx_data.src_addr = SOURCE_ADDRESS;
    lora_tx_data.mid_addr = 0x00;
    lora_tx_data.type = LORA_TYPE_ACK;
    lora_tx_data.total_len = ack;

    rt_thread_mdelay(Radio.GetWakeupTime() + RX_TIME_MARGIN);
    LOG_I("Send ACK");
    Radio.Send((uint8_t *)&lora_tx_data, LORA_ACK_LEN);
}

static void tx_nack_process(void)
{
    static rt_uint8_t rx_timeout_count = 0;
    if (rx_timeout_count == 2)
    {
        /* give up data packet */
        rt_ringbuffer_get(lora_tx_rb, lora_tx_data.payload, wait_send_len);
        wait_send_len = 0;
        last_tx_isOK = true;
        rx_timeout_count = 0;
        /* Start a new round of data reception */
        LOG_I("Rx start");
        Radio.Rx(RX_TIMEOUT_VALUE);
    }
    else
    {
        rx_timeout_count++;
        /* resend data */
        rt_thread_mdelay(Radio.GetWakeupTime() + RX_TIME_MARGIN + random_delay);
        send_last_data();
    }
}

static void lora_cfg_process(void)
{
    LOG_I("Lora configure process");
}

static void lora_send_data(void)
{
    rt_err_t result = 0;
    rt_uint8_t buf[128];
    if (wait_send_len == 0)
    {
        /* get lora tx message */
        result = rt_mq_recv(&lora_tx_mq, &lora_tx_msg, sizeof(lora_tx_msg), 10);
        if (result == RT_EOK)
        {
            if (lora_tx_msg.type == MSG_TYPE_DATA)
            {
                lora_tx_data.type = lora_tx_msg.type;
                wait_send_len = lora_tx_msg.size;
                lora_tx_data.total_len = wait_send_len;
                send_ready_data();
            }
            else if (lora_tx_msg.type == MSG_TYPE_CFG)
            {
                lora_tx_data.type = lora_tx_msg.type;
                lora_cfg_process();
            }
            else
            {
                /* give up this message */
                rt_ringbuffer_get(lora_tx_rb, buf, lora_tx_msg.size);
            }
        }
        else
        {
            LOG_D("Rx start");
            Radio.Rx(RX_TIMEOUT_VALUE);
        }
    }
    else
    {
        send_ready_data();
    }
}

void led_control(void)
{
    rt_pin_write(LED_GREEN, PIN_LOW);
    rt_pin_write(LED_ORANGE, PIN_LOW);
    switch (State)
    {
    case RX:
        rt_pin_write(LED_GREEN, PIN_HIGH);
        break;
    case TX:
        rt_pin_write(LED_ORANGE, PIN_HIGH);
        break;
    case RX_TIMEOUT:
        break;
    case RX_ERROR:
        break;
    case TX_TIMEOUT:
        break;
    default:
        break;
    }
}

static void rx_data_process(receive_msg rx_msg)
{
    rt_err_t result = 0;
    rt_size_t rb_size = 0;
    switch (rx_msg.type) {
        case LORA_TYPE_DATA:
            rb_size = rt_ringbuffer_get(rx_rb, (rt_uint8_t *)&lora_rx_data, rx_msg.size);
            if (rb_size != rx_msg.size)
            {
                LOG_E("From rx_rb get data error");
            }
            else
            {
                LOG_I("Send message to Uart");
                rb_size = rt_ringbuffer_get_size(uart_tx_rb) - rt_ringbuffer_data_len(uart_tx_rb);
                if (rb_size >= lora_tx_data.len)
                {
                    rb_size = rt_ringbuffer_put(uart_tx_rb, lora_rx_data.payload, lora_rx_data.len);
                    lora_rx_msg.dev = RT_NULL;
                    lora_rx_msg.type = MSG_TYPE_DATA;
                    lora_rx_msg.size = lora_rx_data.len;
                    result = rt_mq_send(&uart_tx_mq, &lora_rx_msg, sizeof(lora_rx_msg));
                    if (result == -RT_EFULL)
                    {
                        LOG_E("Uart TX message queue full!");
                    }
                    if (ack_check_enable)
                    {
                        send_ack(lora_rx_data.src_addr, LORA_ACK_OK);
                    }
                    else
                    {
                        LOG_D("Rx start");
                        Radio.Rx(RX_TIMEOUT_VALUE);
                    }
                }
                else
                {
                    LOG_E("uart tx ringbuffer full!");
                    send_ack(lora_rx_data.src_addr, LORA_ACK_RX_FULL);
                }
            }
            break;
        case LORA_TYPE_ACK:
            rb_size = rt_ringbuffer_get(rx_rb, (rt_uint8_t *)&lora_rx_ack, rx_msg.size);
            if (rb_size != rx_msg.size)
            {
                LOG_E("From rx_rb get data error");
            }
            else
            {
                switch (lora_rx_ack.ack) {
                    case LORA_ACK_OK:
                        LOG_I("ACK is OK");
                        last_tx_isOK = true;
                        lora_send_data();
                        break;
                    default:
                        LOG_E("ACK error!");
                        last_tx_isOK = false;
                        LOG_D("Rx start");
                        Radio.Rx(RX_TIMEOUT_VALUE);
                        break;
                }
            }
            break;
        case LORA_TYPE_CFG:
            break;
        case LORA_TYPE_TRS:
            break;
        default:
            break;
    }
}

/* USER CODE BEGIN PrFD */
static void lora_app_thread_entry(void* parameter)
{
  Radio.Sleep();

  LOG_D( "On lora_ping_pong_thread_entry");
  rt_uint32_t ev = 0;
  receive_msg rx_msg;
  static rt_uint8_t tx_timeout_count = 0;
  rt_err_t result;

  TimerStop(&timerLed);
  set_tx_data();

  while( 1 )
  {
      if (rt_event_recv(&radio_event,
                        EV_RADIO_ALL,
                        RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                        RT_WAITING_FOREVER,
                        &ev) == RT_EOK)
      {
          led_control();
          switch (State)
          {
            case RX:
              result = rt_mq_recv(&rx_mq, &rx_msg, sizeof(rx_msg), 10);
              if (result == RT_EOK)
              {
                  rx_data_process(rx_msg);
              }
              else
              {
                  LOG_E("Failed to get message from rx queue!");
                  LOG_I("Rx start");
                  Radio.Rx(RX_TIMEOUT_VALUE);
              }
              break;
            case TX:
              if (ack_check_enable)
              {
                  if (lora_tx_data.type == LORA_TYPE_ACK)
                  {
                      last_tx_isOK = true;
                  }
                  else
                  {
                      LOG_I("Sent successfully, waiting for ACK");
                      last_tx_isOK = false;
                  }
                  LOG_D("TX to Rx");
                  Radio.Rx(RX_TIMEOUT_VALUE);
              }
              else
              {
                  lora_send_data();
              }
              break;
            case RX_TIMEOUT:
#if defined (LOW_POWER_DISABLE) && (LOW_POWER_DISABLE == 0)
              Radio.Sleep();
              break;
#endif
            case RX_ERROR:
              if (last_tx_isOK == true)
              {
                  lora_send_data();
              }
              else
              {
                  tx_nack_process();
              }
              break;
            case TX_TIMEOUT:
              if (lora_tx_data.type == LORA_TYPE_ACK)
              {
                  LOG_E("ACK sending failed!");
                  Radio.Rx(RX_TIMEOUT_VALUE);
              }
              else
              {
                  if (tx_timeout_count == 2)
                  {
                      last_tx_isOK = false;
                      tx_timeout_count = 0;
                      LOG_D("Rx start");
                      Radio.Rx(RX_TIMEOUT_VALUE);
                  }
                  else
                  {
                      tx_timeout_count++;
                      send_last_data();
                  }
              }
              break;
            default:
              break;
          }
      }
    }
}

static void OnledEvent(void)
{
    static uint32_t i = 0;
    i++;

    if (i%2 == 0)
    {
        rt_pin_write(LED_GREEN, PIN_LOW);
        rt_pin_write(LED_RED, PIN_LOW);
    }
    else
    {
        rt_pin_write(LED_GREEN, PIN_HIGH);
        rt_pin_write(LED_RED, PIN_HIGH);
    }

    TimerSetValue(&timerLed, LED_PERIOD_MS);
    TimerStart(&timerLed);
}

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
